#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>

#include "Structure.h"
#include "../common/StructureMessage.h"
#include "../util/math.h"

namespace isomap {
    namespace client {

        /*common::UnitCommandMessage* Structure::moveTo( int32_t tileX, int32_t tileY ) {
            // convert into way points
            std::vector<common::UnitCommandMessage::WayPoint> wayPoints;
            do {
                wayPoints.push_back( {tileX, tileY} );
                if ( tileX < (m_x / math::fix::precision) ) {
                    ++tileX;
                } else if ( tileX > (m_x / math::fix::precision) ) {
                    --tileX;
                }
                if ( tileY < (m_y / math::fix::precision) ) {
                    ++tileY;
                } else if ( tileY > (m_y / math::fix::precision) ) {
                    --tileY;
                }

            } while ( tileX != (m_x / math::fix::precision) || tileY != (m_y / math::fix::precision) );
            return common::UnitCommandMessage::moveMsg( wayPoints );
        }*/

        void Structure::processMessage( common::StructureServerMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            switch ( msg->type() ) {
                case common::StructureServerMessage::Status:
                    m_x = msg->x();
                    m_y = msg->y();
                    m_z = msg->z();
                    break;

                default:
                    break;
            }
        }

        void Structure::initRender( vl::RenderingAbstract* rendering ) {
            m_transform = new vl::Transform;
            rendering->as<vl::Rendering>()->transform()->addChild( m_transform.get() );

            m_geom = vl::makeBox( vl::vec3( 0, 0, 0 ), 2.0, 2.0, 1.5 );
            m_geom->computeNormals();

            m_effect = new vl::Effect;
            m_effect->shader()->gocMaterial()->setColorMaterialEnabled( true );
            m_effect->shader()->gocMaterial()->setDiffuse( vl::crimson );
            m_effect->shader()->enable( vl::EN_DEPTH_TEST );
            m_effect->shader()->enable( vl::EN_LIGHTING );
            m_effect->lod( 0 )->push_back( new vl::Shader );
            m_effect->shader( 0, 1 )->enable( vl::EN_BLEND );
            //m_effect->shader(0,1)->enable(vl::EN_LINE_SMOOTH);
            m_effect->shader( 0, 1 )->enable( vl::EN_DEPTH_TEST );
            m_effect->shader( 0, 1 )->enable( vl::EN_POLYGON_OFFSET_LINE );
            m_effect->shader( 0, 1 )->gocPolygonOffset()->set( -1.0f, -1.0f );
            m_effect->shader( 0, 1 )->gocPolygonMode()->set( vl::PM_LINE, vl::PM_LINE );
            m_effect->shader( 0, 1 )->gocColor()->setValue( vl::red );
            m_effect->shader( 0, 1 )->setRenderState( m_effect->shader()->getMaterial() );
            m_effect->shader( 0, 1 )->setRenderState( m_effect->shader()->getLight( 0 ), 0 );


            vl::ref<vl::SceneManagerActorTree> scene_manager = new vl::SceneManagerActorTree;
            scene_manager->setCullingEnabled( false );
            rendering->as<vl::Rendering>()->sceneManagers()->push_back( scene_manager.get() );
            scene_manager->tree()->addActor( m_geom.get(), m_effect.get(), m_transform.get() );
        }


        void Structure::render() {
            vl::mat4 matrix = vl::mat4::getTranslation(
                    m_x + 1,
                    m_y + 1,
                    0.75 + m_z * ::sqrt( 2.0 / 3.0 ) / 2.0 );
            matrix *= vl::mat4::getRotation( m_orientation, 0, 0, 1 );
            m_transform->setLocalMatrix( matrix );
        }

    }
}