#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <vlCore/ResourceDatabase.hpp>

#include "Structure.h"
#include "../common/StructureMessage.h"
#include "../common/StructureType.h"
#include "../util/math.h"

namespace isomap {
    namespace client {

        void Structure::processMessage( common::StructureServerMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            switch ( msg->type() ) {
                case common::StructureServerMessage::Status:
                    m_data.x = msg->x();
                    m_data.y = msg->y();
                    m_data.z = msg->z();
                    m_data.constructionProgress = msg->constructionProgress();
                    break;

                default:
                    break;
            }
        }

        void Structure::initRender( vl::RenderingAbstract* rendering ) {
            m_transform = new vl::Transform;
            rendering->as<vl::Rendering>()->transform()->addChild( m_transform.get() );


            m_effect = new vl::Effect;
            //m_effect->shader()->gocMaterial()->setColorMaterialEnabled( true );
            m_effect->shader()->setRenderState( new vl::Light, 0 );
            m_effect->shader()->gocMaterial()->setDiffuse( vl::crimson );
            m_effect->shader()->enable( vl::EN_DEPTH_TEST );
            m_effect->shader()->enable( vl::EN_LIGHTING );
            m_effect->lod( 0 )->push_back( new vl::Shader );
            m_effect->shader( 0, 1 )->enable( vl::EN_BLEND );
            //m_effect->shader(0,1)->enable(vl::EN_LINE_SMOOTH);
            m_effect->shader( 0, 1 )->enable( vl::EN_DEPTH_TEST );
            //m_effect->shader( 0, 1 )->enable( vl::EN_POLYGON_OFFSET_LINE );
            //m_effect->shader( 0, 1 )->gocPolygonOffset()->set( -1.0f, -1.0f );
            //m_effect->shader( 0, 1 )->gocPolygonMode()->set( vl::PM_LINE, vl::PM_LINE );
            //m_effect->shader( 0, 1 )->gocColor()->setValue( vl::red );
            m_effect->shader( 0, 1 )->setRenderState( m_effect->shader()->getMaterial() );
            m_effect->shader( 0, 1 )->setRenderState( m_effect->shader()->getLight( 0 ), 0 );

            vl::ref<vl::SceneManagerActorTree> scene_manager = new vl::SceneManagerActorTree;
            scene_manager->setCullingEnabled( false );
            rendering->as<vl::Rendering>()->sceneManagers()->push_back( scene_manager.get() );

            std::string path = "models/";
            path += m_type->name();
            path += ".obj";
            vl::ref<vl::ResourceDatabase> resource_db = vl::loadResource( path.c_str(), false );
            if ( !resource_db || resource_db->count<vl::Actor>() == 0 ) {
                VL_LOG_ERROR << "No data found.\n";
                return;
            }
            for ( size_t ires = 0; ires < resource_db->resources().size(); ++ires ) {
                vl::Actor* act = resource_db->resources()[ires]->as<vl::Actor>();

                if ( !act )
                    continue;

                //if ( act->effect() == NULL ) {
                //printf( "Set default effect!\n" );
                act->setEffect( m_effect.get() );
                /*} else {
                    vl::Effect* fx = act->effect();
                    fx->shader()->enable(vl::EN_DEPTH_TEST);
                    fx->shader()->enable(vl::EN_LIGHTING);
                    fx->shader()->setRenderState( m_effect->shader()->renderState( vl::RS_Light, 0 ), 0 );
                    fx->shader()->gocLightModel()->setTwoSide(true);
                }*/

                vl::Geometry* geom = act->lod( 0 )->as<vl::Geometry>();
                geom->computeNormals();

                scene_manager->tree()->addActor( act );

                if ( geom && geom->normalArray() ) {
                    act->effect()->shader()->enable( vl::EN_LIGHTING );
                    act->effect()->shader()->gocLightModel()->setTwoSide( true );
                }

                if ( geom && !geom->normalArray() ) {
                    act->effect()->shader()->disable( vl::EN_LIGHTING );
                }

                VL_CHECK( act );
                VL_CHECK( act->effect() );
                act->setTransform( m_transform.get() );

                //mEffects.insert( act->effect() );
            }
        }


        void Structure::render() {
            vl::real z = (m_data.constructionProgress * 1.5) / 100.0;
            z = z - 1.5;
            // remember, operations are done in reverse order here

            // 4. translate to actual position
            vl::mat4 matrix = vl::mat4::getTranslation(
                    m_data.x,
                    m_data.y,
                    z + m_data.z * ::sqrt( 2.0 / 3.0 ) / 2.0 );
            // 3. move model back to 0,0 make sure to use new orientation
            matrix *= vl::mat4::getTranslation( m_type->footPrint( m_data.orientation )->width() / 2.0,
                                                m_type->footPrint( m_data.orientation )->height() / 2.0, 0 );
            // 2. rotate to correct orientation
            matrix *= vl::mat4::getRotation( -m_data.orientation * 90, 0, 0, 1 );
            // 1. move model to center of model, use default orientation
            matrix *= vl::mat4::getTranslation( m_type->footPrint( 0 )->width() / -2.0,
                                                m_type->footPrint( 0 )->height() / -2.0, 0 );
            m_transform->setLocalMatrix( matrix );
        }


        bool Structure::occupies( uint32_t x, uint32_t y, uint32_t width, uint32_t height ) const {
            if ( x >= m_data.x + m_type->width( m_data.orientation ) ) {
                return false; // structure right of area
            }
            if ( m_data.x >= x + width ) {
                return false; // structure left of area
            }
            if ( y >= m_data.y + m_type->height( m_data.orientation ) ) {
                return false; // structure below area
            }
            if ( m_data.y >= y + height ) {
                return false; // structure above area
            }
            return true; // structure and area overlap
        }

    }
}