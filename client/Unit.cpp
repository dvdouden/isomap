#include <queue>

#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <vlCore/ResourceDatabase.hpp>

#include "Match.h"
#include "Player.h"
#include "Terrain.h"
#include "Unit.h"
#include "../common/UnitMessage.h"
#include "../util/math.h"
#include "ModelCache.h"

namespace isomap {
    namespace client {

        Unit::Unit( Player* player, const common::UnitData& data ) :
                m_player( player ),
                m_data( data ),
                m_type( common::UnitType::get( data.typeId ) ) {
        }


        Unit::~Unit() {
            for ( auto* actor : m_actors ) {
                // this deletes the actor (reduces refcount and *poof*)
                m_player->sceneManager()->tree()->eraseActor( actor );
            }
        }

        void Unit::moveTo( int32_t targetX, int32_t targetY ) {
            printf( "Move to %d %d\n", targetX, targetY );

            auto width = m_player->terrain()->width();
            auto height = m_player->terrain()->height();

            std::vector<common::WayPoint> wayPoints;

            // create buffer for A* algorithm
            struct node {
                uint32_t value = 0;
                uint32_t from = 0;

                bool operator>( const node& rhs ) const {
                    return value > rhs.value;
                }
            };

            auto* nodeMap = new node[width * height]();

            // create a todo list for the algorithm
            std::priority_queue<node, std::vector<node>, std::greater<>> todo;
            todo.push( {1, tileY() * width + tileX()} );
            while ( !todo.empty() ) {
                auto tile = todo.top();
                todo.pop();
                auto value = nodeMap[tile.from];
                unsigned int tile_x = tile.from % width;
                unsigned int tile_y = tile.from / width;
                //printf( "Test %d %d\n", tile_x, tile_y);
                if ( tile_x == targetX && tile_y == targetY ) {
                    break;
                }
                uint8_t canReach = m_player->terrain()->pathMap()[tile_y * width + tile_x];
                //printf( "[%2d,%2d] %02X\n", tile_x, tile_y, canReach );

                // for now we're going to move in every direction, as long as we haven't traveled there yet.
                if ( canReach & common::path::bitDownLeft ) {
                    //printf( "down left\n" );
                    auto idx = (tile_y - 1) * width + tile_x - 1;
                    if ( nodeMap[idx].value == 0 ) {
                        nodeMap[idx].value = value.value + 14;
                        nodeMap[idx].from = tile.from;
                        todo.push( {value.value + 14, idx} );
                    }
                }
                if ( canReach & common::path::bitDown ) {
                    //printf( "down\n" );
                    auto idx = (tile_y - 1) * width + tile_x;
                    if ( nodeMap[idx].value == 0 ) {
                        nodeMap[idx].value = value.value + 10;
                        nodeMap[idx].from = tile.from;
                        todo.push( {value.value + 10, idx} );
                    }
                }
                if ( canReach & common::path::bitDownRight ) {
                    //printf( "down right\n" );
                    auto idx = (tile_y - 1) * width + tile_x + 1;
                    if ( nodeMap[idx].value == 0 ) {
                        nodeMap[idx].value = value.value + 14;
                        nodeMap[idx].from = tile.from;
                        todo.push( {value.value + 14, idx} );
                    }
                }
                if ( canReach & common::path::bitRight ) {
                    //printf( "right\n" );
                    auto idx = (tile_y) * width + tile_x + 1;
                    if ( nodeMap[idx].value == 0 ) {
                        nodeMap[idx].value = value.value + 10;
                        nodeMap[idx].from = tile.from;
                        todo.push( {value.value + 10, idx} );
                    }
                }
                if ( canReach & common::path::bitUpRight ) {
                    //printf( "right up\n" );
                    auto idx = (tile_y + 1) * width + tile_x + 1;
                    if ( nodeMap[idx].value == 0 ) {
                        nodeMap[idx].value = value.value + 14;
                        nodeMap[idx].from = tile.from;
                        todo.push( {value.value + 14, idx} );
                    }
                }
                if ( canReach & common::path::bitUp ) {
                    //printf( "up\n" );
                    auto idx = (tile_y + 1) * width + tile_x;
                    if ( nodeMap[idx].value == 0 ) {
                        nodeMap[idx].value = value.value + 10;
                        nodeMap[idx].from = tile.from;
                        todo.push( {value.value + 10, idx} );
                    }
                }
                if ( canReach & common::path::bitUpLeft ) {
                    //printf( "up left\n" );
                    auto idx = (tile_y + 1) * width + tile_x - 1;
                    if ( nodeMap[idx].value == 0 ) {
                        nodeMap[idx].value = value.value + 14;
                        nodeMap[idx].from = tile.from;
                        todo.push( {value.value + 14, idx} );
                    }
                }
                if ( canReach & common::path::bitLeft ) {
                    //printf( "left\n" );
                    auto idx = (tile_y) * width + tile_x - 1;
                    if ( nodeMap[idx].value == 0 ) {
                        nodeMap[idx].value = value.value + 10;
                        nodeMap[idx].from = tile.from;
                        todo.push( {value.value + 10, idx} );
                    }
                }
            }
            uint32_t targetIdx = targetY * width + targetX;
            uint32_t startIdx = tileY() * width + tileX();
            if ( nodeMap[targetIdx].value > 0 ) {
                printf( "Found a route!\n" );
                while ( targetIdx != startIdx ) {
                    int wayPointX = (int)(targetIdx % width);
                    int wayPointY = (int)(targetIdx / width);
                    targetIdx = nodeMap[targetIdx].from;
                    int tile_x = (int)(targetIdx % width);
                    int tile_y = (int)(targetIdx / width);
                    //unsigned char direction = getDirection( wayPointX - tile_x, wayPointY - tile_y );
                    //if ( wayPoints.empty() || direction != wayPoints.back().direction ) {
                    wayPoints.push_back( {wayPointX, wayPointY} );
                    //}
                    //printf( "[%d]: %d %d\n", nodeMap[targetIdx].value, tile_x, tile_y );
                }
                m_player->enqueueMessage( id(), common::UnitCommandMessage::moveMsg( wayPoints ) );
            } else {
                printf( "No route!\n" );
            }


            delete[] nodeMap;
        }


        void Unit::processMessage( common::UnitServerMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            switch ( msg->type() ) {
                case common::UnitServerMessage::Status:
                    m_data = msg->data();
                    break;

                case common::UnitServerMessage::MoveTo:
                    m_data = msg->data();
                    break;

                case common::UnitServerMessage::Stop:
                    m_data = msg->data();
                    break;

                default:
                    break;
            }
        }

        void Unit::update() {
            if ( m_data.motionState == common::Moving ) {
                m_data.updateMotion();

                // FIXME: move out of bounds check to somewhere else
                if ( m_data.x < 0 ) {
                    m_data.x = 0;
                } else if ( m_data.x >= m_player->terrain()->width() * math::fix::precision ) {
                    m_data.x = (m_player->terrain()->width() - 1) * math::fix::precision;
                }
                if ( m_data.y < 0 ) {
                    m_data.y = 0;
                } else if ( m_data.y >= m_player->terrain()->height() * math::fix::precision ) {
                    m_data.y = (m_player->terrain()->height() - 1) * math::fix::precision;
                }

                // FIXME: move height calculation to somewhere else
                m_data.z = m_player->terrain()->heightMap()[tileY() * m_player->terrain()->width() + tileX()] *
                           math::fix::precision;
            }
        }

        void Unit::initRender( vl::RenderingAbstract* rendering, vl::SceneManagerActorTree* sceneManager ) {
            m_transform = new vl::Transform;
            rendering->as<vl::Rendering>()->transform()->addChild( m_transform.get() );

            m_effect = new vl::Effect;
            m_effect->shader()->setRenderState( new vl::Light, 0 );
            m_effect->shader()->gocMaterial()->setDiffuse( m_player->color() );
            m_effect->shader()->enable( vl::EN_DEPTH_TEST );
            m_effect->shader()->enable( vl::EN_LIGHTING );
            m_effect->lod( 0 )->push_back( new vl::Shader );
            m_effect->shader( 0, 1 )->enable( vl::EN_BLEND );
            m_effect->shader( 0, 1 )->enable( vl::EN_DEPTH_TEST );
            m_effect->shader( 0, 1 )->setRenderState( m_effect->shader()->getMaterial() );
            m_effect->shader( 0, 1 )->setRenderState( m_effect->shader()->getLight( 0 ), 0 );

            vl::ResourceDatabase* resource_db = ModelCache::get( m_type->name() );

            for ( auto& ires : resource_db->resources() ) {
                auto* act = ires->as<vl::Actor>();

                if ( !act )
                    continue;

                auto* geom = act->lod( 0 )->as<vl::Geometry>();

                vl::Actor* actor = sceneManager->tree()->addActor( geom, m_effect.get(), m_transform.get() );
                actor->setObjectName( m_player->name() + " unit " + std::to_string( m_data.id ) + "-" +
                                      std::to_string( m_actors.size() ) );
                m_actors.push_back( actor );

                if ( geom && geom->normalArray() ) {
                    actor->effect()->shader()->enable( vl::EN_LIGHTING );
                    actor->effect()->shader()->gocLightModel()->setTwoSide( true );
                }

                if ( geom && !geom->normalArray() ) {
                    actor->effect()->shader()->disable( vl::EN_LIGHTING );
                }
            }
        }

        void Unit::render() {
            // remember, operations are done in reverse order here

            // 4. translate to actual position
            vl::mat4 matrix = vl::mat4::getTranslation(
                    vl::real( m_data.x ) / math::fix::fPrecision,
                    vl::real( m_data.y ) / math::fix::fPrecision,
                    vl::real( m_data.z ) / math::fix::fPrecision * ::sqrt( 2.0 / 3.0 ) / 2.0 );
            // 3. move model back to 0,0 make sure to use new orientation
            matrix *= vl::mat4::getTranslation( 1.0 / 2.0,
                                                1.0 / 2.0, 0 );
            // 2. rotate to correct orientation
            matrix *= vl::mat4::getRotation( m_data.orientation * -45.0, 0, 0, 1 );

            // 1. move model to center of model, use default orientation
            matrix *= vl::mat4::getTranslation( 1.0 / -2.0,
                                                1.0 / -2.0, 0 );
            m_transform->setLocalMatrix( matrix );
        }

        void Unit::setVisible( bool visible ) {
            if ( m_visible == visible ) {
                return;
            }
            m_visible = visible;
            for ( auto* actor : m_actors ) {
                actor->setEnabled( visible );
            }
        }

    }
}