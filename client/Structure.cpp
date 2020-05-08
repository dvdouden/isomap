#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <vlCore/ResourceDatabase.hpp>

#include "Player.h"
#include "Structure.h"
#include "../common/StructureMessage.h"
#include "../common/StructureType.h"
#include "../util/math.h"
#include "ModelCache.h"

namespace isomap {
    namespace client {


        Structure::~Structure() {
            for ( auto* actor : m_actors ) {
                // TODO: make sure this actually destroys the actor
                m_player->sceneManager()->tree()->eraseActor( actor );
            }
        }

        void Structure::processMessage( common::StructureServerMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            switch ( msg->type() ) {
                case common::StructureServerMessage::Status:
                    m_data = msg->data();
                    break;

                default:
                    break;
            }
        }

        void Structure::initRender( vl::RenderingAbstract* rendering, vl::SceneManagerActorTree* sceneManager ) {
            m_transform = new vl::Transform;
            rendering->as<vl::Rendering>()->transform()->addChild( m_transform.get() );


            m_effect = new vl::Effect;
            //m_effect->shader()->gocMaterial()->setColorMaterialEnabled( true );
            m_effect->shader()->setRenderState( new vl::Light, 0 );
            m_effect->shader()->gocMaterial()->setDiffuse( m_player->color() );
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

            vl::ResourceDatabase* resource_db = ModelCache::get( m_type->name() );

            for ( auto& ires : resource_db->resources() ) {
                auto* act = ires->as<vl::Actor>();

                if ( !act )
                    continue;

                auto* geom = act->lod( 0 )->as<vl::Geometry>();

                vl::Actor* actor = sceneManager->tree()->addActor( geom, m_effect.get(), m_transform.get() );
                actor->setObjectName( m_player->name() + " structure " + std::to_string( m_data.id ) + "-" +
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

        void Structure::clearRender( vl::SceneManagerActorTree* sceneManager ) {
            for ( auto* actor : m_actors ) {
                sceneManager->tree()->eraseActor( actor );
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
            matrix *= vl::mat4::getRotation( m_data.orientation * -90.0, 0, 0, 1 );

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