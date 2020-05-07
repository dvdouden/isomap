#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <vlCore/ResourceDatabase.hpp>

#include "Match.h"
#include "Player.h"
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

        common::UnitCommandMessage* Unit::moveTo( int32_t tileX, int32_t tileY ) {
            // convert into way points
            std::vector<common::UnitCommandMessage::WayPoint> wayPoints;
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
            return common::UnitCommandMessage::moveMsg( wayPoints );
        }

        void Unit::processMessage( common::UnitServerMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            switch ( msg->type() ) {
                case common::UnitServerMessage::Status:
                    m_data = msg->data();
                    break;

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
                actor->setObjectName( m_player->name() + " unit " + std::to_string( m_data.id ) + "-" + std::to_string( m_actors.size() ) );
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
            vl::mat4 matrix = vl::mat4::getTranslation(
                    (m_data.x / math::fix::fPrecision),
                    (m_data.y / math::fix::fPrecision),
                    (m_data.z / math::fix::fPrecision) * ::sqrt( 2.0 / 3.0 ) / 2.0 );

            matrix *= vl::mat4::getRotation( m_data.orientation, 0, 0, 1 );
            m_transform->setLocalMatrix( matrix );
        }

    }
}