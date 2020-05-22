
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <vlCore/ResourceDatabase.hpp>

#include "Renderer.h"
#include "../ModelCache.h"
#include "../Player.h"

namespace isomap {
    namespace client {
        namespace unit {

            Renderer::Renderer( Unit* unit ) :
                    m_unit( unit ) {

            }

            Renderer::~Renderer() {
                for ( auto* actor : m_actors ) {
                    // this deletes the actor (reduces refcount and *poof*)
                    m_unit->player()->renderer()->sceneManager()->tree()->eraseActor( actor );
                }
            }

            void Renderer::update() {

            }


            void Renderer::init( vl::RenderingAbstract* rendering, vl::SceneManagerActorTree* sceneManager ) {
                m_transform = new vl::Transform;
                rendering->as<vl::Rendering>()->transform()->addChild( m_transform.get() );

                m_effect = new vl::Effect;
                m_effect->shader()->setRenderState( new vl::Light, 0 );
                m_effect->shader()->gocMaterial()->setDiffuse( m_unit->player()->renderer()->color() );
                m_effect->shader()->enable( vl::EN_DEPTH_TEST );
                m_effect->shader()->enable( vl::EN_LIGHTING );
                m_effect->lod( 0 )->push_back( new vl::Shader );
                m_effect->shader( 0, 1 )->enable( vl::EN_BLEND );
                m_effect->shader( 0, 1 )->enable( vl::EN_DEPTH_TEST );
                m_effect->shader( 0, 1 )->setRenderState( m_effect->shader()->getMaterial() );
                m_effect->shader( 0, 1 )->setRenderState( m_effect->shader()->getLight( 0 ), 0 );

                vl::ResourceDatabase* resource_db = ModelCache::get( m_unit->type()->name() );

                for ( auto& ires : resource_db->resources() ) {
                    auto* act = ires->as<vl::Actor>();

                    if ( !act )
                        continue;

                    auto* geom = act->lod( 0 )->as<vl::Geometry>();

                    vl::Actor* actor = sceneManager->tree()->addActor( geom, m_effect.get(), m_transform.get() );
                    actor->setObjectName( m_unit->player()->name() + " unit " + std::to_string( m_unit->id() ) + "-" +
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
                // remember, operations are done in reverse order here

                // 4. translate to actual position
                vl::mat4 matrix = vl::mat4::getTranslation(
                        vl::real( m_unit->x() ) / math::fix::fPrecision,
                        vl::real( m_unit->y() ) / math::fix::fPrecision,
                        vl::real( m_unit->z() ) / math::fix::fPrecision * ::sqrt( 2.0 / 3.0 ) / 2.0 );
                // 3. move model back to 0,0 make sure to use new orientation
                /*matrix *= vl::mat4::getTranslation( 1.0 / 2.0,
                                                    1.0 / 2.0, 0 );*/
                // 2. rotate to correct orientation
                matrix *= vl::mat4::getRotation( m_unit->orientation() * -45.0, 0, 0, 1 );

                // 1. move model to center of model, use default orientation
                matrix *= vl::mat4::getTranslation( 1.0 / -2.0,
                                                    1.0 / -2.0, 0 );
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