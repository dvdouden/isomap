#pragma once

#include <vlGraphics/RenderingAbstract.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>

#include "../../common/types.h"

namespace isomap {
    namespace client {
        namespace player {
            class Renderer {
            public:
                explicit Renderer( Player* player );

                void init( vl::RenderingAbstract* rendering );

                void disable();

                void enable();

                void render();

                void dumpActors();

                void addUnit( Unit* unit );

                void addStructure( Structure* structure );

                vl::SceneManagerActorTree* sceneManager() {
                    return m_sceneManager.get();
                }

                const vl::fvec4& color() const {
                    return m_color;
                }

            private:
                Player* m_player;
                vl::fvec4 m_color;

                vl::RenderingAbstract* m_rendering = nullptr;
                vl::ref<vl::SceneManagerActorTree> m_sceneManager;

            };
        }
    }
}
