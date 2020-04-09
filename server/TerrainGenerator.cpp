#include "TerrainGenerator.h"
#include "Terrain.h"
#include "../util/math.h"

namespace isomap {
    namespace server {

        uint8_t clamp( int a ) {
            return a > 255 ? 255 : (a < 0 ? 0 : a);
        }

        void diamond(
                uint8_t* data,
                uint32_t width,
                uint32_t height,
                uint32_t scale,
                uint32_t depth,
                math::rng& rnd ) {
            // diamond
            uint32_t hs = scale / 2;
            for ( uint32_t y = hs; y < height; y += scale ) {
                for ( uint32_t x = hs; x < width; x += scale ) {
                    uint32_t i0 = data[(y - hs) * width + (x - hs)];
                    uint32_t i1 = data[(y - hs) * width + (x + hs)];
                    uint32_t i2 = data[(y + hs) * width + (x - hs)];
                    uint32_t i3 = data[(y + hs) * width + (x + hs)];
                    int32_t avg = (i0 + i1 + i2 + i3) / 4;
                    data[y * width + x] = clamp( avg + rnd( -int32_t( depth ), int32_t( depth ) ) );
                }
            }
        }


        void square(
                uint8_t* data,
                uint32_t width,
                uint32_t height,
                uint32_t scale,
                uint32_t depth,
                math::rng& rnd ) {
            // diamond
            for ( uint32_t y = 0; y < height; y += scale ) {
                for ( uint32_t x = 0; x < width; x += scale ) {
                    if ( x < width - 1 ) {
                        uint32_t x1 = x + scale / 2;
                        // square the one on the right
                        int32_t avg = data[y * width + x] + data[y * width + x + scale];
                        int32_t cnt = 2;
                        if ( y > 0 ) {
                            avg += data[(y - scale / 2) * width + x1];
                            cnt++;
                        }
                        if ( y < height - 1 ) {
                            avg += data[(y + scale / 2) * width + x1];
                            cnt++;
                        }
                        avg /= cnt;
                        data[y * width + x1] = clamp( avg + rnd( -int32_t( depth ), int32_t( depth ) ) );
                    }
                    if ( y < height - 1 ) {
                        uint32_t y1 = y + scale / 2;
                        // square the one below
                        int32_t avg = data[y * width + x] + data[(y + scale) * width + x];
                        int32_t cnt = 2;
                        if ( x > 0 ) {
                            avg += data[y1 * width + x - scale / 2];
                            cnt++;
                        }
                        if ( x < width - 1 ) {
                            avg += data[y1 * width + x + scale / 2];
                            cnt++;
                        }
                        avg /= cnt;
                        data[y1 * width + x] = clamp( avg + rnd( -int32_t( depth ), int32_t( depth ) ) );
                    }
                }
            }
        }


        uint8_t* squareDiamond( uint32_t width, uint32_t height, uint32_t scale, math::rng& rnd ) {
            auto* tmp = new uint8_t[width * height];
            for ( uint32_t i = 0; i < width * height; ++i ) {
                tmp[i] = 0;
            }

            // initial corner values
            for ( uint32_t y = 0; y < height; y += scale ) {
                for ( uint32_t x = 0; x < width; x += scale ) {
                    tmp[y * width + x] = rnd( 256 );
                }
            }

            uint32_t magnitude = 128;
            while ( scale > 1 ) {
                diamond( tmp, width, height, scale, magnitude, rnd );
                square( tmp, width, height, scale, magnitude, rnd );
                scale >>= 1u;
                magnitude >>= 1u;
            }
            return tmp;
        }


        void generateOreMap(
                uint8_t* map,
                uint32_t width,
                uint32_t height,
                uint32_t scale,
                math::rng& rnd,
                uint8_t oreAmount,
                uint8_t oreDensity ) {

            uint32_t sdWidth = ((width + (scale - 1)) / scale) * scale + 1;
            uint32_t sdHeight = ((height + (scale - 1)) / scale) * scale + 1;

            // create ore map
            uint8_t* ore_map = squareDiamond( sdWidth, sdHeight, scale, rnd );
            uint8_t* scr_o = ore_map;
            for ( uint32_t i = 0; i < sdWidth * sdHeight; ++i ) {
                if ( *scr_o >= oreAmount ) {
                    *scr_o = 0;
                } else {
                    // tiles with 0 have highest concentration
                    // density runs from 0 (0%, all tiles minimum) to 255 (200% half tiles max)
                    uint32_t amount = (oreAmount - *scr_o);
                    // linear interpolation from (oreDensity * 2) to 1
                    amount = ((oreDensity * 2) * amount) / oreAmount;
                    if ( amount > 255 ) {
                        // clamp at 255
                        amount = 255;
                    } else if ( amount == 0 ) {
                        // ensure a minimum of 1
                        amount = 1;
                    }
                    *scr_o = amount;
                }
                ++scr_o;
            }

            // copy data to actual oremap
            for ( int y = 0; y < height; ++y ) {
                memcpy( map + (y * width), ore_map + (y * sdWidth), width );
            }

            delete[] ore_map;
        }


        Terrain* TerrainGenerator::generate( uint32_t width, uint32_t height ) {
            auto* terrain = new Terrain( width, height );

            math::rng rnd( m_seed );

            // use diamond-square algorithm
            // scale up to a multiple of 2^depth + 1
            uint32_t scale = 1u << m_depth;
            uint32_t sdWidth = ((width + (scale - 1)) / scale) * scale + 1;
            uint32_t sdHeight = ((height + (scale - 1)) / scale) * scale + 1;
            uint8_t* heightMap = squareDiamond( sdWidth, sdHeight, scale, rnd );

            // apply cliffs
            uint8_t* cliffMap = squareDiamond( sdWidth, sdHeight, scale / 2, rnd );
            uint8_t* scratchHeightMap = heightMap;
            uint8_t* scratchCliffMap = cliffMap;
            for ( int i = 0; i < sdWidth * sdHeight; ++i ) {
                if ( *scratchCliffMap >= m_cliffAmount ) {
                    // reduce height to 0-3
                    *scratchHeightMap >>= 6u;
                } else {
                    // clamp height to 0 or 3
                    if ( *scratchHeightMap >= 128 ) {
                        *scratchHeightMap = 3;
                    } else {
                        *scratchHeightMap = 0;
                    }
                }
                ++scratchHeightMap;
                ++scratchCliffMap;
            }
            delete[] cliffMap;

            // copy data to actual map
            for ( int y = 0; y < height; ++y ) {
                memcpy( terrain->heightMap() + (y * width), heightMap + (y * sdWidth), width );
            }
            delete[] heightMap;

            generateOreMap( terrain->oreMap(), width, height, scale, rnd, m_oreAmount, m_oreDensity );

            return terrain;
        }

    }
}
