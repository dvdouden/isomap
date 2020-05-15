
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <vlCore/ResourceDatabase.hpp>

#include "Renderer.h"
#include "../ModelCache.h"
#include "../Player.h"
#include "../player/Renderer.h"

namespace isomap {
    namespace client {
        namespace structure {

            Renderer::Renderer( Structure* structure ) :
                    m_structure( structure ) {

            }

            Renderer::~Renderer() {
                for ( auto* actor : m_actors ) {
                    // this deletes the actor (reduces refcount and *poof*)
                    m_structure->player()->renderer()->sceneManager()->tree()->eraseActor( actor );
                }
            }

            void Renderer::update() {

            }


            void Renderer::init( vl::RenderingAbstract* rendering, vl::SceneManagerActorTree* sceneManager ) {
                m_transform = new vl::Transform;
                rendering->as<vl::Rendering>()->transform()->addChild( m_transform.get() );

                m_effect = new vl::Effect;
                m_effect->shader()->setRenderState( new vl::Light, 0 );
                m_effect->shader()->gocMaterial()->setDiffuse( m_structure->player()->renderer()->color() );
                m_effect->shader()->enable( vl::EN_DEPTH_TEST );
                m_effect->shader()->enable( vl::EN_LIGHTING );
                m_effect->lod( 0 )->push_back( new vl::Shader );
                m_effect->shader( 0, 1 )->enable( vl::EN_BLEND );
                m_effect->shader( 0, 1 )->enable( vl::EN_DEPTH_TEST );
                m_effect->shader( 0, 1 )->setRenderState( m_effect->shader()->getMaterial() );
                m_effect->shader( 0, 1 )->setRenderState( m_effect->shader()->getLight( 0 ), 0 );

                vl::ResourceDatabase* resource_db = ModelCache::get( m_structure->type()->name() );

                for ( auto& ires : resource_db->resources() ) {
                    auto* act = ires->as<vl::Actor>();

                    if ( !act )
                        continue;

                    auto* geom = act->lod( 0 )->as<vl::Geometry>();

                    vl::Actor* actor = sceneManager->tree()->addActor( geom, m_effect.get(), m_transform.get() );
                    actor->setObjectName(
                            m_structure->player()->name() + " structure " + std::to_string( m_structure->id() ) + "-" +
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


            void Renderer::render() {
                vl::real z = (m_structure->data().constructionProgress * 1.5) / 100.0;
                z = z - 1.5;
                // remember, operations are done in reverse order here

                // 4. translate to actual position
                vl::mat4 matrix = vl::mat4::getTranslation(
                        m_structure->data().x,
                        m_structure->data().y,
                        z + m_structure->data().z * ::sqrt( 2.0 / 3.0 ) / 2.0 );
                // 3. move model back to 0,0 make sure to use new orientation
                matrix *= vl::mat4::getTranslation( m_structure->footPrint()->width() / 2.0,
                                                    m_structure->footPrint()->height() / 2.0, 0 );
                // 2. rotate to correct orientation
                matrix *= vl::mat4::getRotation( m_structure->data().orientation * -90.0, 0, 0, 1 );

                // 1. move model to center of model, use default orientation
                matrix *= vl::mat4::getTranslation( m_structure->type()->footPrint( 0 )->width() / -2.0,
                                                    m_structure->type()->footPrint( 0 )->height() / -2.0, 0 );
                m_transform->setLocalMatrix( matrix );
            }

            void Renderer::setVisible() {
                for ( auto* actor : m_actors ) {
                    actor->setEnabled( true );
                }
            }

            void Renderer::setInvisible() {
                for ( auto* actor : m_actors ) {
                    actor->setEnabled( false );
                }
            }
        }
    }
}