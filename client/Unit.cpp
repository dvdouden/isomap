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
            //printf( "New unit for player [%s.%s], id [%d] %p\n", m_player->match()->player()->name().c_str(), m_player->name().c_str(), id(), this );
        }


        Unit::~Unit() {
            //printf( "Delete unit for player [%s.%s], id [%d] %p\n", m_player->match()->player()->name().c_str(), m_player->name().c_str(), id(), this );
            for ( auto* actor : m_actors ) {
                // this deletes the actor (reduces refcount and *poof*)
                m_player->sceneManager()->tree()->eraseActor( actor );
            }
        }

        void Unit::moveTo( uint32_t targetX, uint32_t targetY ) {
            m_commands = {};
            m_commands.push( {common::UnitCommandMessage::Move, targetX, targetY, 0} );
            if ( m_player->ai() != nullptr ) {
                m_player->ai()->onUnitActive( this );
            }
        }

        void Unit::doMove( Command& command ) {
            Structure* structure = nullptr;
            if ( command.id != 0 ) {
                printf( "Move to structure %d\n", command.id );
                // FIXME: can only move to own structures?
                structure = m_player->getStructure( command.id );
                if ( structure == nullptr ) {
                    printf( "Unknown structure!\n" );
                    return;
                }
            } else {
                printf( "Move to %d %d\n", command.x, command.y );
            }


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

            uint32_t targetIdx = command.y * width + command.x;
            uint32_t startIdx = tileY() * width + tileX();

            todo.push( {1, startIdx} );
            bool found = false;
            while ( !todo.empty() ) {
                auto tile = todo.top();
                todo.pop();
                auto value = nodeMap[tile.from];
                uint32_t tile_x = tile.from % width;
                uint32_t tile_y = tile.from / width;
                //printf( "Test %d %d\n", tile_x, tile_y);
                if ( structure != nullptr ) {
                    if ( structure->isAdjacentTo( tile_x, tile_y ) ) {
                        targetIdx = tile.from;
                        // update the target position in the command, now that we have found a place to go
                        command.x = targetIdx % width;
                        command.y = targetIdx / width;
                        printf( "Found tile %d %d to be adjacent to structure\n", command.x, command.y );
                        found = true;
                        break;
                    }
                } else if ( tile.from == targetIdx ) {
                    found = true;
                    break;
                }
                uint8_t canReach = m_player->terrain()->pathMap()[tile.from];
                //printf( "[%2d,%2d] %02X\n", tile_x, tile_y, canReach );

                // for now we're going to move in every direction, as long as we haven't traveled there yet.
                if ( canReach & common::path::bitDownLeft ) {
                    //printf( "down left\n" );
                    auto idx = tile.from - width - 1;
                    if ( nodeMap[idx].value == 0 ) {
                        nodeMap[idx].value = value.value + 14;
                        nodeMap[idx].from = tile.from;
                        todo.push( {value.value + 14, idx} );
                    }
                }
                if ( canReach & common::path::bitDown ) {
                    //printf( "down\n" );
                    auto idx = tile.from - width;
                    if ( nodeMap[idx].value == 0 ) {
                        nodeMap[idx].value = value.value + 10;
                        nodeMap[idx].from = tile.from;
                        todo.push( {value.value + 10, idx} );
                    }
                }
                if ( canReach & common::path::bitDownRight ) {
                    //printf( "down right\n" );
                    auto idx = tile.from - width + 1;
                    if ( nodeMap[idx].value == 0 ) {
                        nodeMap[idx].value = value.value + 14;
                        nodeMap[idx].from = tile.from;
                        todo.push( {value.value + 14, idx} );
                    }
                }
                if ( canReach & common::path::bitRight ) {
                    //printf( "right\n" );
                    auto idx = tile.from + 1;
                    if ( nodeMap[idx].value == 0 ) {
                        nodeMap[idx].value = value.value + 10;
                        nodeMap[idx].from = tile.from;
                        todo.push( {value.value + 10, idx} );
                    }
                }
                if ( canReach & common::path::bitUpRight ) {
                    //printf( "right up\n" );
                    auto idx = tile.from + width + 1;
                    if ( nodeMap[idx].value == 0 ) {
                        nodeMap[idx].value = value.value + 14;
                        nodeMap[idx].from = tile.from;
                        todo.push( {value.value + 14, idx} );
                    }
                }
                if ( canReach & common::path::bitUp ) {
                    //printf( "up\n" );
                    auto idx = tile.from + width;
                    if ( nodeMap[idx].value == 0 ) {
                        nodeMap[idx].value = value.value + 10;
                        nodeMap[idx].from = tile.from;
                        todo.push( {value.value + 10, idx} );
                    }
                }
                if ( canReach & common::path::bitUpLeft ) {
                    //printf( "up left\n" );
                    auto idx = tile.from + width - 1;
                    if ( nodeMap[idx].value == 0 ) {
                        nodeMap[idx].value = value.value + 14;
                        nodeMap[idx].from = tile.from;
                        todo.push( {value.value + 14, idx} );
                    }
                }
                if ( canReach & common::path::bitLeft ) {
                    //printf( "left\n" );
                    auto idx = tile.from - 1;
                    if ( nodeMap[idx].value == 0 ) {
                        nodeMap[idx].value = value.value + 10;
                        nodeMap[idx].from = tile.from;
                        todo.push( {value.value + 10, idx} );
                    }
                }
            }
            if ( found ) {
                printf( "Found a route!\n" );
                while ( targetIdx != startIdx ) {
                    int wayPointX = (int)(targetIdx % width);
                    int wayPointY = (int)(targetIdx / width);
                    targetIdx = nodeMap[targetIdx].from;
                    //int tile_x = (int)(targetIdx % width);
                    //int tile_y = (int)(targetIdx / width);
                    //unsigned char direction = getDirection( wayPointX - tile_x, wayPointY - tile_y );
                    //if ( wayPoints.empty() || direction != wayPoints.back().direction ) {
                    wayPoints.push_back( {wayPointX, wayPointY} );
                    //}
                    //printf( "[%d]: %d %d\n", nodeMap[targetIdx].value, tile_x, tile_y );
                }
                m_player->enqueueMessage( id(), common::UnitCommandMessage::moveMsg( wayPoints ) );
                command.messageSent = true;
            } else {
                printf( "No route!\n" );
            }
            delete[] nodeMap;
        }

        void Unit::construct( Structure* structure ) {
            //printf( "Unit %d construct %d\n", id(), structure->id() );
            if ( !m_type->canConstruct() ) {
                printf( "[%d] Construct command given to unit without construction abilities!\n", id() );
                return;
            }
            m_commands = {};
            if ( !structure->isAdjacentTo( tileX(), tileY() ) ) {
                m_commands.push(
                        {common::UnitCommandMessage::Move, 0, 0, structure->id()} );
            }
            m_commands.push( {common::UnitCommandMessage::Construct, 0, 0, structure->id()} );
            if ( m_player->ai() != nullptr ) {
                m_player->ai()->onUnitActive( this );
            }
        }

        void Unit::doConstruct( Command& command ) {
            m_player->enqueueMessage( id(), common::UnitCommandMessage::constructMsg( command.id ) );
            command.messageSent = true;
        }

        void Unit::processMessage( common::UnitServerMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            switch ( msg->type() ) {
                case common::UnitServerMessage::Construct:
                    m_data = msg->data();
                    break;

                case common::UnitServerMessage::Status:
                    m_data = msg->data();
                    break;

                case common::UnitServerMessage::MoveTo:
                    m_data = msg->data();
                    break;

                case common::UnitServerMessage::Stop:
                    m_data = msg->data();
                    if ( !m_commands.empty() ) {
                        if ( m_commands.front().type == common::UnitCommandMessage::Move &&
                             m_commands.front().x == tileX() &&
                             m_commands.front().y == tileY() ) {
                            m_commands.pop();
                        } else if ( m_commands.front().type == common::UnitCommandMessage::Construct ) {
                            m_commands.pop();
                        }

                        if ( m_commands.empty() && player()->ai() != nullptr ) {
                            player()->ai()->onUnitIdle( this );
                        }
                    }
                    break;

                default:
                    break;
            }
        }

        void Unit::update() {
            switch ( m_data.state ) {
                case common::Moving: {
                    int32_t oldTileX = tileX();
                    int32_t oldTileY = tileY();

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

                    if ( tileX() != oldTileX || tileY() != oldTileY ) {
                        m_player->terrain()->updateUnit( this, oldTileX, oldTileY );
                    }
                    break;
                }

                case common::Idle: {
                    updateCommandQueue();
                    break;
                }

                default:
                    break;
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

        void Unit::setVisible( const common::UnitData& data ) {
            m_data = data;
            if ( !m_visible ) {
                m_visible = true;
                for ( auto* actor : m_actors ) {
                    actor->setEnabled( true );
                }
            }
        }

        void Unit::setInvisible() {
            if ( m_visible ) {
                m_visible = false;
                for ( auto* actor : m_actors ) {
                    actor->setEnabled( false );
                }
            }
        }

        void Unit::updateCommandQueue() {
            if ( m_commands.empty() ) {
                return;
            }

            if ( !m_commands.empty() && !m_commands.front().messageSent ) {
                switch ( m_commands.front().type ) {
                    case common::UnitCommandMessage::Move:
                        doMove( m_commands.front() );
                        break;

                    case common::UnitCommandMessage::Construct:
                        doConstruct( m_commands.front() );
                        break;
                }
                if ( !m_commands.front().messageSent ) {
                    printf( "Unit [%d] failed to execute command %d\n", id(), m_commands.front().type );
                    while ( !m_commands.empty() ) {
                        // cancel commands
                        auto& command = m_commands.front();
                        if ( command.type == common::UnitCommandMessage::Construct ) {
                            if ( m_player->ai() ) {
                                m_player->ai()->onBuildStructureAccepted( command.id );
                            }
                        }
                        m_commands.pop();
                    }
                    if ( m_player->ai() ) {
                        m_player->ai()->onUnitStuck( this );
                    }
                }
            }
        }

        void Unit::dump() const {
            printf( "client Unit [%d] (%d:%s) at %d,%d,%d (%s) (%d commands queued)\n",
                    id(),
                    m_type->id(),
                    m_type->name().c_str(),
                    tileX(),
                    tileY(),
                    tileZ(),
                    stateName(),
                    m_commands.size()
            );
        }

    }
}