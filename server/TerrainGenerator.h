#pragma once

#include "../common/types.h"


namespace isomap {
    namespace server {

        class TerrainGenerator {
        public:
            TerrainGenerator() = default;

            ~TerrainGenerator() = default;

            Terrain* generate( uint32_t width, uint32_t height );

        private:
            uint32_t m_seed = 0;
            uint32_t m_depth = 5;
            uint8_t m_cliffAmount = 128;
            uint8_t m_oreAmount = 16;
            uint8_t m_oreDensity = 128;
        };

    }
}
