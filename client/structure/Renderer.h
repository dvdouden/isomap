#pragma once

#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/RenderingAbstract.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>

#include "../types.h"
#include "../Unit.h"


namespace isomap {
    namespace client {
        namespace structure {

            class Renderer {
            public:
                explicit Renderer( Structure* structure );

                ~Renderer();

                Renderer( const Renderer& ) = delete;

                Renderer& operator=( const Renderer& ) = delete;

                void update();

                void init( vl::RenderingAbstract* rendering, vl::SceneManagerActorTree* sceneManager );

                void render();

                void setVisible();

                void setInvisible();


            private:
                Structure* m_structure;

                vl::ref<vl::Transform> m_transform;
                vl::ref<vl::Effect> m_effect;
                std::vector<vl::Actor*> m_actors;
            };
        }

    }
}
