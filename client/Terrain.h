#pragma once

#include <vlGraphics/RenderingAbstract.hpp>
#include "../common/types.h"

namespace isomap {
    namespace client {
        class Terrain {
        public:
            Terrain() = default;

            ~Terrain() = default;

            Terrain( const Terrain& ) = delete;

            const Terrain& operator=( const Terrain& ) = delete;

            void processMessage( common::TerrainMessage* msg );

            uint32_t width() const {
                return m_width;
            }

            uint32_t height() const {
                return m_height;
            }

            uint8_t* heightMap() const {
                return m_heightMap;
            }

            uint8_t* slopeMap() const {
                return m_slopeMap;
            }

            uint8_t* oreMap() const {
                return m_oreMap;
            }

            void initRender( vl::RenderingAbstract* rendering );

            void render();

            void updateFog();

        private:
            uint8_t getCorner( int x, int y, int c ) const;

            uint32_t m_width = 0;
            uint32_t m_height = 0;
            uint32_t m_fogUpdateMapScale = 64;
            uint32_t m_fogUpdateMapWidth = 0;
            uint32_t m_fogUpdateMapHeight = 0;

            uint8_t* m_heightMap = nullptr;
            uint8_t* m_slopeMap = nullptr;
            uint8_t* m_oreMap = nullptr;
            uint8_t* m_fogMap = nullptr;
            uint8_t* m_fogUpdateMap = nullptr;


            vl::ref<vl::SceneManagerActorTree> m_sceneManager;
        };
    }
}




