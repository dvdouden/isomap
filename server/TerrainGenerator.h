#pragma once

#include "../common/types.h"
#include "../util/math.h"


namespace isomap {
    namespace server {

        class TerrainGenerator {
        public:
            TerrainGenerator() = default;

            ~TerrainGenerator() = default;

            Terrain* generate( uint32_t width, uint32_t height );

            void setHeightScale( uint32_t scale ) {
                m_heightScale = scale;
            }

            void setHeightNoise( uint8_t noise ) {
                m_heightNoise = noise;
            }

            void setMinHeight( uint8_t height ) {
                m_minHeight = height;
            }

            void setMaxHeight( uint8_t height ) {
                m_maxHeight = height;
            }

            void setCliffScale( uint32_t scale ) {
                m_cliffScale = scale;
            }

            void setCliffNoise( uint8_t noise ) {
                m_cliffNoise = noise;
            }

            void setCliffThreshold( uint8_t threshold ) {
                m_cliffThreshold = threshold;
            }

            void setOreScale( uint8_t scale ) {
                m_oreScale = scale;
            }

            void setOreNoise( uint8_t noise ) {
                m_oreNoise = noise;
            }

            void setOreThreshold( uint8_t threshold ) {
                m_oreThreshold = threshold;
            }

            void setOreDensity( uint8_t oreDensity ) {
                m_oreDensity = oreDensity;
            }

            void setShoreBits( uint8_t shoreBits ) {
                m_shoreBits = shoreBits;
            }

        private:
            void generateOreMap( uint8_t* map, uint32_t width, uint32_t height, math::rng& rnd );

            uint32_t m_seed = 0;

            uint32_t m_heightScale = 5;
            uint8_t m_heightNoise = 64;
            uint8_t m_minHeight = 0;
            uint8_t m_maxHeight = 255;

            uint32_t m_cliffScale = 5;
            uint8_t m_cliffNoise = 128;
            uint8_t m_cliffThreshold = 0;

            uint32_t m_oreScale = 5;
            uint8_t m_oreNoise = 32;
            uint8_t m_oreThreshold = 16;
            uint8_t m_oreDensity = 128;

            uint8_t m_shoreBits = 1;


        };

    }
}
