#pragma once

#include <vlGraphics/RenderingAbstract.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>

#include "../../common/FootPrint.h"
#include "../../common/types.h"
#include "../Terrain.h"


namespace isomap {
    namespace client {
        namespace terrain {

            class Renderer {
            public:
                explicit Renderer( Terrain* terrain );

                ~Renderer();

                Renderer( const Renderer& ) = delete;

                Renderer& operator=( const Renderer& ) = delete;

                void init( vl::RenderingAbstract* rendering );

                void render();

                void enable();

                void disable();


                void toggleRenderFog() {
                    m_renderFog = !m_renderFog;
                }

                void toggleRenderOccupancy() {
                    m_renderOccupancy = !m_renderOccupancy;
                }


                void highLight( const common::Area& area, const vl::fvec4& color ) {
                    m_highlightArea = area;
                    m_highlightColor = color;
                    m_renderHighlight = true;
                }

                void addHighlight( const common::Area& area, const vl::fvec4& color ) {
                    m_highLightAreas.emplace_back( area, color );
                }

                void clearHighlight() {
                    m_renderHighlight = false;
                    m_highLightAreas.clear();
                }


            private:
                Terrain* m_terrain;

                bool m_renderFog = false;
                bool m_renderHighlight = false;
                bool m_renderOccupancy = false;

                common::Area m_highlightArea;
                vl::fvec4 m_highlightColor;

                std::vector<std::pair<common::Area, vl::fvec4>> m_highLightAreas;

                vl::ref<vl::SceneManagerActorTree> m_sceneManager;
            };
        }

    }
}
