#pragma once

#include "types.h"
#include "../util/math.h"


namespace isomap {
    namespace server {

        class TerrainGenerator {
        public:
            TerrainGenerator() = default;

            ~TerrainGenerator() = default;

            Terrain* generate( uint32_t width, uint32_t height );

            uint32_t heightScale() const {
                return m_heightScale;
            }

            void setHeightScale( uint32_t scale ) {
                m_heightScale = scale;
            }

            uint8_t heightNoise() const {
                return m_heightNoise;
            }

            void setHeightNoise( uint8_t noise ) {
                m_heightNoise = noise;
            }

            uint8_t minHeight() const {
                return m_minHeight;
            }

            void setMinHeight( uint8_t height ) {
                m_minHeight = height;
            }

            uint8_t maxHeight() const {
                return m_maxHeight;
            }

            void setMaxHeight( uint8_t height ) {
                m_maxHeight = height;
            }

            uint8_t terrainHeight() const {
                return m_terrainHeight;
            }

            void setTerrainHeight( uint8_t height ) {
                m_terrainHeight = height;
            }

            uint8_t waterDepth() const {
                return m_waterDepth;
            }

            void setWaterDepth( uint8_t waterDepth ) {
                m_waterDepth = waterDepth;
            }

            uint32_t cliffScale() const {
                return m_cliffScale;
            }

            void setCliffScale( uint32_t scale ) {
                m_cliffScale = scale;
            }

            uint8_t cliffNoise() const {
                return m_cliffNoise;
            }

            void setCliffNoise( uint8_t noise ) {
                m_cliffNoise = noise;
            }

            uint8_t cliffThreshold() const {
                return m_cliffThreshold;
            }

            void setCliffThreshold( uint8_t threshold ) {
                m_cliffThreshold = threshold;
            }

            uint8_t oreScale() const {
                return m_oreScale;
            }

            void setOreScale( uint8_t scale ) {
                m_oreScale = scale;
            }

            uint8_t oreNoise() const {
                return m_oreNoise;
            }

            void setOreNoise( uint8_t noise ) {
                m_oreNoise = noise;
            }

            uint8_t oreThreshold() const {
                return m_oreThreshold;
            }

            void setOreThreshold( uint8_t threshold ) {
                m_oreThreshold = threshold;
            }

            uint8_t oreDensity() const {
                return m_oreDensity;
            }

            void setOreDensity( uint8_t oreDensity ) {
                m_oreDensity = oreDensity;
            }

            uint8_t shoreBits() const {
                return m_shoreBits;
            }

            void setShoreBits( uint8_t shoreBits ) {
                m_shoreBits = shoreBits;
            }

            uint32_t seed() const {
                return m_seed;
            }

            void setSeed( uint32_t seed ) {
                m_seed = seed;
            }

        private:
            void generateOreMap( uint8_t* map, uint32_t width, uint32_t height, math::rng& rnd );

            uint32_t m_seed = 0;

            uint32_t m_heightScale = 5;
            uint8_t m_heightNoise = 64;
            uint8_t m_minHeight = 0;
            uint8_t m_maxHeight = 255;

            uint8_t m_terrainHeight = 255;
            uint8_t m_waterDepth = 64;

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
