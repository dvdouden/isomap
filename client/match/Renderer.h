#pragma once

#include <vlGraphics/RenderingAbstract.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>

#include "../types.h"

namespace isomap {
    namespace client {
        namespace match {
            class Renderer {
            public:
                explicit Renderer( Match* match );

                ~Renderer();

                void init( vl::RenderingAbstract* rendering );

                void disable();

                void enable();

                void render();

                void dumpActors();

                void addPlayer( Player* player );

                void addTerrain( Terrain* terrain );

                vl::SceneManagerActorTree* sceneManager() {
                    return m_sceneManager.get();
                }

            private:
                Match* m_match;

                vl::RenderingAbstract* m_rendering = nullptr;
                vl::ref<vl::SceneManagerActorTree> m_sceneManager;

            };
        }
    }
}
