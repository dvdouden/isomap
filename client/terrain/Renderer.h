#pragma once

#include <map>

#include <vlGraphics/RenderingAbstract.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>

#include "../../common/FootPrint.h"
#include "../types.h"
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


                void addCursor( uint32_t x, uint32_t y, const vl::fvec4& color );

                void clearCursor() {
                    m_cursor.clear();
                }

                void addHighlight( uint32_t x, uint32_t y, const vl::fvec4& color );

                void clearHighlight() {
                    m_highlights.clear();
                }

                void addDebug( uint32_t x, uint32_t y, const vl::fvec4& color );

                void clearDebug() {
                    m_debug.clear();
                }


            private:
                Terrain* m_terrain;

                bool m_renderFog = false;
                bool m_renderOccupancy = false;

                std::map<uint32_t, vl::fvec4> m_cursor;
                std::map<uint32_t, vl::fvec4> m_highlights;
                std::map<uint32_t, vl::fvec4> m_debug;

                vl::ref<vl::SceneManagerActorTree> m_sceneManager;
            };
        }

    }
}
