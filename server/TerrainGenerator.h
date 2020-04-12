#pragma once

#include "../common/types.h"


namespace isomap {
    namespace server {

        class TerrainGenerator {
        public:
            TerrainGenerator() = default;

            ~TerrainGenerator() = default;

            Terrain* generate( uint32_t width, uint32_t height );

            void setVariation( uint32_t variation ) {
                m_variation = variation;
            }

            void setCliffAmount( uint8_t cliffAmount ) {
                m_cliffAmount = cliffAmount;
            }

            void setOreAmount( uint8_t oreAmount ) {
                m_oreAmount = oreAmount;
            }

            void setOreDensity( uint8_t oreDensity ) {
                m_oreDensity = oreDensity;
            }

            void setDepth( uint32_t depth ) {
                m_depth = depth;
            }

            void setCliffVariation( uint32_t cliffVariation ) {
                m_cliffVariation = cliffVariation;
            }

        private:
            uint32_t m_seed = 0;
            uint32_t m_depth = 5;
            uint32_t m_variation = 64;
            uint8_t m_cliffAmount = 0;
            uint32_t m_cliffVariation = 128;
            uint8_t m_oreAmount = 16;
            uint8_t m_oreDensity = 128;
        };

    }
}
