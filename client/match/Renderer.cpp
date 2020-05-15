
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <vlCore/ResourceDatabase.hpp>

#include "Renderer.h"
#include "../Match.h"
#include "../Player.h"
#include "../player/Renderer.h"
#include "../Terrain.h"

namespace isomap {
    namespace client {
        namespace match {

            Renderer::Renderer( Match* match ) :
                    m_match( match ) {

            }

            Renderer::~Renderer() = default;


            void Renderer::disable() {
                if ( m_match->terrain() != nullptr ) {
                    m_match->terrain()->renderer()->disable();
                }
                for ( auto& player : m_match->players() ) {
                    player.second->renderer()->disable();
                }
            }

            void Renderer::enable() {
                if ( m_match->terrain() != nullptr ) {
                    m_match->terrain()->renderer()->enable();
                }
                for ( auto& player : m_match->players() ) {
                    player.second->renderer()->enable();
                }
            }

            void Renderer::dumpActors() {
                for ( auto& player : m_match->players() ) {
                    player.second->renderer()->dumpActors();
                }
            }

            void Renderer::addPlayer( Player* player ) {
                auto* renderer = new player::Renderer( player );
                player->setRenderer( renderer );
                renderer->init( m_rendering );
            }

            void Renderer::addTerrain( Terrain* terrain ) {
                auto* renderer = new terrain::Renderer( terrain );
                terrain->setRenderer( renderer );
                renderer->init( m_rendering );
            }

            void Renderer::init( vl::RenderingAbstract* rendering ) {
                m_rendering = rendering;
                if ( m_match->terrain() != nullptr ) {
                    m_match->terrain()->renderer()->init( m_rendering );
                }
                for ( auto& player : m_match->players() ) {
                    player.second->renderer()->init( m_rendering );
                }
            }

            void Renderer::render() {
                m_match->terrain()->renderer()->render();
                for ( auto& player : m_match->players() ) {
                    player.second->renderer()->render();
                }
            }

        }
    }
}