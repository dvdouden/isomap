#include <vlGraphics/Rendering.hpp>

#include "Renderer.h"
#include "../Player.h"
#include "../unit/Renderer.h"

namespace isomap {
    namespace client {
        namespace player {

            Renderer::Renderer( Player* player ) :
                    m_player( player ) {
                id_t id = m_player->id();
                m_color = vl::fvec4(
                        ((id >> 24u) & 0xFFu) / 255.0,
                        ((id >> 16u) & 0xFFu) / 255.0,
                        ((id >> 8u) & 0xFFu) / 255.0,
                        (id & 0xFFu) / 255.0 );
            }


            void Renderer::init( vl::RenderingAbstract* rendering ) {
                m_sceneManager = new vl::SceneManagerActorTree;
                m_sceneManager->setCullingEnabled( false );
                rendering->as<vl::Rendering>()->sceneManagers()->push_back( m_sceneManager.get() );
                m_rendering = rendering;

                for ( auto& it : m_player->structures() ) {
                    if ( it.second->visible() ) {
                        it.second->renderer()->init( m_rendering, m_sceneManager.get() );
                    }
                }
                for ( auto& it : m_player->units() ) {
                    if ( it.second->visible() ) {
                        it.second->renderer()->init( m_rendering, m_sceneManager.get() );
                    }
                }
            }

            void Renderer::disable() {
                m_sceneManager->setEnableMask( 0 );
            }

            void Renderer::enable() {
                m_sceneManager->setEnableMask( 0xFFFFFFFF );
            }

            void Renderer::dumpActors() {
                for ( auto actor : m_sceneManager->tree()->actors()->vector() ) {
                    printf( "%s at %f %f %f\n",
                            actor->objectName().c_str(),
                            actor->transform()->worldMatrix().getT().x(),
                            actor->transform()->worldMatrix().getT().y(),
                            actor->transform()->worldMatrix().getT().z() );
                }
            }

            void Renderer::render() {
                // TODO: This shouldn't be done per player
                for ( auto& it : m_player->structures() ) {
                    if ( it.second->visible() ) {
                        it.second->renderer()->render();
                    }
                }
                for ( auto& it : m_player->units() ) {
                    if ( it.second->visible() ) {
                        it.second->renderer()->render();
                    }
                }
            }

            void Renderer::addUnit( Unit* unit ) {
                auto* unitRenderer = new unit::Renderer( unit );
                unit->setRenderer( unitRenderer );
                unitRenderer->init( m_rendering, m_sceneManager.get() );
            }


            void Renderer::addStructure( Structure* structure ) {
                auto* structureRenderer = new structure::Renderer( structure );
                structure->setRenderer( structureRenderer );
                structureRenderer->init( m_rendering, m_sceneManager.get() );
            }
        }
    }
}