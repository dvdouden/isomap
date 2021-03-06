#include <cstring>
#include "TerrainGenerator.h"
#include "Terrain.h"
#include "../util/math.h"

namespace isomap {
    namespace server {

        uint8_t clamp( int a, uint8_t max = 255 ) {
            return a > max ? max : (a < 0 ? 0 : a);
        }

        void diamond(
                uint8_t* data,
                uint32_t width,
                uint32_t height,
                uint32_t scale,
                uint32_t noise,
                math::rng& rnd,
                uint8_t maxHeight ) {
            uint32_t hs = scale / 2;
            for ( uint32_t y = hs; y < height; y += scale ) {
                for ( uint32_t x = hs; x < width; x += scale ) {
                    uint32_t i0 = data[(y - hs) * width + (x - hs)];
                    uint32_t i1 = data[(y - hs) * width + (x + hs)];
                    uint32_t i2 = data[(y + hs) * width + (x - hs)];
                    uint32_t i3 = data[(y + hs) * width + (x + hs)];
                    int32_t avg = (i0 + i1 + i2 + i3) / 4;
                    data[y * width + x] = clamp( avg + rnd( -int32_t( noise ), int32_t( noise ) ), maxHeight );
                }
            }
        }


        void square(
                uint8_t* data,
                uint32_t width,
                uint32_t height,
                uint32_t scale,
                uint32_t noise,
                math::rng& rnd,
                uint8_t maxHeight,
                uint8_t shores ) {
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
                        data[y * width + x1] = clamp( avg + rnd( -int32_t( noise ), int32_t( noise ) ), maxHeight );

                        if ( y == 0 && (shores & 0b0000'0001u) != 0 ) {
                            data[y * width + x1] = 0;
                        }
                        if ( (y + scale) >= height && (shores & 0b0000'0100u) != 0 ) {
                            data[y * width + x1] = 0;
                        }
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
                        data[y1 * width + x] = clamp( avg + rnd( -int32_t( noise ), int32_t( noise ) ), maxHeight );
                        if ( x == 0 && (shores & 0b0000'0010u) != 0 ) {
                            data[y1 * width + x] = 0;
                        }
                        if ( (x + scale) >= width && (shores & 0b0000'1000u) != 0 ) {
                            data[y1 * width + x] = 0;
                        }
                    }
                }
            }
        }

        uint8_t* diamondSquare( uint32_t width, uint32_t height, uint32_t scale, uint32_t noise, math::rng& rnd,
                                uint8_t minHeight = 0, uint8_t maxHeight = 255, uint8_t shores = 0 ) {
            auto* tmp = new uint8_t[width * height];
            for ( uint32_t i = 0; i < width * height; ++i ) {
                tmp[i] = 0;
            }

            // initial corner values
            for ( uint32_t y = 0; y < height; y += scale ) {
                for ( uint32_t x = 0; x < width; x += scale ) {
                    tmp[y * width + x] = rnd( minHeight, maxHeight );
                }
            }

            if ( shores & 0b0000'0001u ) {
                for ( uint32_t x = 0; x < width; x += scale ) {
                    tmp[x] = 0;
                }
            }
            if ( shores & 0b0000'0010u ) {
                for ( uint32_t y = 0; y < height; y += scale ) {
                    tmp[y * width] = 0;
                }
            }
            if ( shores & 0b0000'0100u ) {
                for ( uint32_t x = 0; x < width; x += scale ) {
                    tmp[(height - 1) * width + x] = 0;
                }
            }
            if ( shores & 0b0000'1000u ) {
                for ( uint32_t y = 0; y < height; y += scale ) {
                    tmp[y * width + (width - 1)] = 0;
                }
            }

            uint32_t tempScale = scale;
            while ( tempScale > 1 ) {
                uint32_t magnitude = (noise * tempScale) / scale;
                diamond( tmp, width, height, tempScale, magnitude, rnd, maxHeight );
                square( tmp, width, height, tempScale, magnitude, rnd, maxHeight, shores );
                tempScale >>= 1u;
            }
            return tmp;
        }

        Terrain* TerrainGenerator::generate( uint32_t width, uint32_t height ) {
            auto* terrain = new Terrain( width, height );
            auto& data = terrain->data();

            math::rng rnd( m_seed );

            static const uint8_t grayCode[] = {
                    0b0000u,
                    0b0001u,
                    0b0011u,
                    0b0010u,
                    0b0110u,
                    0b0111u,
                    0b0101u,
                    0b0100u,
                    0b1100u,
                    0b1101u,
                    0b1111u,
                    0b1110u,
                    0b1010u,
                    0b1011u,
                    0b1001u,
                    0b1000u};

            // use diamond-square algorithm
            // scale up to a multiple of 2^depth + 1
            uint32_t scale = 1u << m_heightScale;
            uint32_t sdWidth = ((width + (scale - 1)) / scale) * scale + 1;
            uint32_t sdHeight = ((height + (scale - 1)) / scale) * scale + 1;
            uint8_t* tmpHeightMap = diamondSquare( sdWidth, sdHeight, scale, m_heightNoise, rnd, m_minHeight,
                                                   m_maxHeight, grayCode[m_shoreBits] );

            // apply cliffs
            uint8_t* tmpCliffMap = diamondSquare( sdWidth, sdHeight, 1u << m_cliffScale, m_cliffNoise, rnd );
            uint8_t* scratchHeightMap = tmpHeightMap;
            uint8_t* scratchCliffMap = tmpCliffMap;
            for ( int i = 0; i < sdWidth * sdHeight; ++i ) {
                auto& h = *scratchHeightMap;
                if ( h < 64 ) {
                    uint32_t tmp = 64 - h;
                    tmp *= m_waterDepth;
                    tmp += 511;
                    tmp /= 0x0400;
                    h = 4 - tmp;
                } else {
                    uint32_t tmp = h - 64;
                    tmp *= m_terrainHeight;
                    tmp /= 0x0FFF;
                    h = tmp + 4;
                }
                if ( *scratchCliffMap < m_cliffThreshold ) {
                    // clamp to nearest plateau
                    h = (h + 2u) & 0b1111'1100u;
                    if ( h > 0b0000'1111u ) {
                        h = 0b0000'1111u;
                    }
                }
                ++scratchHeightMap;
                ++scratchCliffMap;
            }
            delete[] tmpCliffMap;

            // copy data to actual map
            uint8_t* heightMap = data.heightMap;
            for ( int y = 0; y < height; ++y ) {
                memcpy( heightMap + (y * width), tmpHeightMap + (y * sdWidth), width );
            }
            delete[] tmpHeightMap;

            // smooth out the map, get rid of peaks and holes
            for ( int p = 0; p < 4; ++p ) {
                for ( int y = 0; y < height; ++y ) {
                    for ( int x = 0; x < width; ++x ) {
                        auto h = heightMap[y * width + x];
                        auto h2 = data.safeHeight( x, y - 1 );
                        auto h4 = data.safeHeight( x + 1, y );
                        auto h6 = data.safeHeight( x, y + 1 );
                        auto h8 = data.safeHeight( x - 1, y );

                        if ( h2 == h4 && h4 == h6 && h6 == h8 && h != h2 ) {
                            heightMap[y * width + x] = h2;
                        }
                    }
                }
            }

            // smooth out the map, get rid of bumps that would always end up flat anyway
            for ( int y = 0; y < height; ++y ) {
                for ( int x = 0; x < width; ++x ) {
                    auto h = heightMap[y * width + x];
                    auto h1 = data.safeHeight( x - 1, y - 1 );
                    auto h2 = data.safeHeight( x, y - 1 );
                    auto h3 = data.safeHeight( x + 1, y - 1 );
                    auto h4 = data.safeHeight( x + 1, y );
                    auto h5 = data.safeHeight( x + 1, y + 1 );
                    auto h6 = data.safeHeight( x, y + 1 );
                    auto h7 = data.safeHeight( x - 1, y + 1 );
                    auto h8 = data.safeHeight( x - 1, y );

                    auto ha = h;
                    if ( (h8 < h || h1 < h || h2 < h) && !(h8 < h - 1 || h1 < h - 1 || h2 < h - 1) ) {
                        ha = h - 1;
                    }

                    auto hb = h;
                    if ( (h2 < h || h3 < h || h4 < h) && !(h2 < h - 1 || h3 < h - 1 || h4 < h - 1) ) {
                        hb = h - 1;
                    }

                    auto hc = h;
                    if ( (h4 < h || h5 < h || h6 < h) && !(h4 < h - 1 || h5 < h - 1 || h6 < h - 1) ) {
                        hc = h - 1;
                    }

                    auto hd = h;
                    if ( (h6 < h || h7 < h || h8 < h) && !(h6 < h - 1 || h7 < h - 1 || h8 < h - 1) ) {
                        hd = h - 1;
                    }

                    if ( ha == hb && hc == hd && ha == hc ) {
                        heightMap[y * width + x] = ha;
                    }
                }
            }

            // now calculate the corners of each tile
            data.updateSlopes();

            // calculate if the tile has a cliff side
            data.updateCliffs();

            generateOreMap( data, rnd );

            return terrain;
        }

        void TerrainGenerator::generateOreMap(
                common::TerrainData& data,
                math::rng& rnd ) {
            uint32_t scale = 1u << m_oreScale;
            uint32_t sdWidth = ((data.mapWidth + (scale - 1)) / scale) * scale + 1;
            uint32_t sdHeight = ((data.mapHeight + (scale - 1)) / scale) * scale + 1;

            // create ore map
            uint8_t* ore_map = diamondSquare( sdWidth, sdHeight, scale, m_oreNoise, rnd );
            uint8_t* scr_o = ore_map;
            for ( uint32_t i = 0; i < sdWidth * sdHeight; ++i ) {
                if ( *scr_o >= m_oreThreshold ) {
                    *scr_o = 0;
                } else {
                    // tiles with 0 have highest concentration
                    // density runs from 0 (0%, all tiles minimum) to 255 (200% half tiles max)
                    uint32_t amount = (m_oreThreshold - *scr_o);
                    // linear interpolation from (oreDensity * 2) to 1
                    amount = ((m_oreDensity * 2) * amount) / m_oreThreshold;
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

            // copy data to actual ore map
            for ( int y = 0; y < data.mapHeight; ++y ) {
                memcpy( data.oreMap + (y * data.mapWidth), ore_map + (y * sdWidth), data.mapWidth );
            }

            delete[] ore_map;
        }

    }
}
