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
                // TODO: make sure this actually destroys the actor
                m_player->sceneManager()->tree()->eraseActor( actor );
            }
        }

        void Unit::moveTo( int32_t tileX, int32_t tileY ) {
            // convert into way points
            std::vector<common::WayPoint> wayPoints;
            do {
                wayPoints.push_back( {tileX, tileY} );
                if ( tileX < (m_data.x / math::fix::precision) ) {
                    ++tileX;
                } else if ( tileX > (m_data.x / math::fix::precision) ) {
                    --tileX;
                }
                if ( tileY < (m_data.y / math::fix::precision) ) {
                    ++tileY;
                } else if ( tileY > (m_data.y / math::fix::precision) ) {
                    --tileY;
                }

            } while ( tileX != (m_data.x / math::fix::precision) || tileY != (m_data.y / math::fix::precision) );
            m_player->enqueueMessage( id(), common::UnitCommandMessage::moveMsg( wayPoints ) );
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
                sceneManager->tree()->addActor( actor );
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

        void Unit::clearRender( vl::SceneManagerActorTree* sceneManager ) {
            for ( auto* actor : m_actors ) {
                sceneManager->tree()->eraseActor( actor );
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
            matrix *= vl::mat4::getRotation( m_data.orientation * -1.0, 0, 0, 1 );

            // 1. move model to center of model, use default orientation
            matrix *= vl::mat4::getTranslation( 1.0 / -2.0,
                                                1.0 / -2.0, 0 );
            m_transform->setLocalMatrix( matrix );
        }

    }
}