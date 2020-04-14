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
                uint32_t noise,
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
                    data[y * width + x] = clamp( avg + rnd( -int32_t( noise ), int32_t( noise ) ) );
                }
            }
        }


        void square(
                uint8_t* data,
                uint32_t width,
                uint32_t height,
                uint32_t scale,
                uint32_t noise,
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
                        data[y * width + x1] = clamp( avg + rnd( -int32_t( noise ), int32_t( noise ) ) );
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
                        data[y1 * width + x] = clamp( avg + rnd( -int32_t( noise ), int32_t( noise ) ) );
                    }
                }
            }
        }


        uint8_t* squareDiamond( uint32_t width, uint32_t height, uint32_t scale, uint32_t noise, math::rng& rnd ) {
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

            uint32_t tempScale = scale;
            while ( tempScale > 1 ) {
                uint32_t magnitude = (noise * tempScale) / scale;
                diamond( tmp, width, height, tempScale, magnitude, rnd );
                square( tmp, width, height, tempScale, magnitude, rnd );
                tempScale >>= 1u;
            }
            return tmp;
        }


        void TerrainGenerator::generateOreMap(
                uint8_t* map,
                uint32_t width,
                uint32_t height,
                math::rng& rnd ) {
            uint32_t scale = 1u << m_oreScale;
            uint32_t sdWidth = ((width + (scale - 1)) / scale) * scale + 1;
            uint32_t sdHeight = ((height + (scale - 1)) / scale) * scale + 1;

            // create ore map
            uint8_t* ore_map = squareDiamond( sdWidth, sdHeight, scale, m_oreNoise, rnd );
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

            // copy data to actual oremap
            for ( int y = 0; y < height; ++y ) {
                memcpy( map + (y * width), ore_map + (y * sdWidth), width );
            }

            delete[] ore_map;
        }


        uint8_t safe_height( uint8_t* heightMap, uint32_t width, uint32_t height, int x, int y ) {
            if ( y < 0 ) y = 0;
            if ( y >= height ) y = height - 1;
            if ( x < 0 ) x = 0;
            if ( x >= width ) x = width - 1;
            return heightMap[y * width + x];
        }


        uint8_t
        getCorner( const uint8_t* heightMap, const uint8_t* slopes, uint32_t width, uint32_t height, int x, int y,
                   int i ) {
            if ( x < 0 ) {
                x = 0;
                if ( i == 1 ) {
                    i = 0;
                } else if ( i == 2 ) {
                    i = 3;
                }
            } else if ( x >= width ) {
                x = (int)width - 1;
                if ( i == 0 ) {
                    i = 1;
                } else if ( i == 3 ) {
                    i = 2;
                }
            }
            if ( y < 0 ) {
                y = 0;
                if ( i == 2 ) {
                    i = 1;
                } else if ( i == 3 ) {
                    i = 0;
                }
            } else if ( y >= height ) {
                y = (int)height - 1;
                if ( i == 0 ) {
                    i = 3;
                } else if ( i == 1 ) {
                    i = 2;
                }
            }
            return heightMap[y * width + x] - (uint8_t( slopes[y * width + x] >> uint32_t( i ) ) & 0b0000'0001u);
        }


        Terrain* TerrainGenerator::generate( uint32_t width, uint32_t height ) {
            auto* terrain = new Terrain( width, height );

            math::rng rnd( m_seed );

            // use diamond-square algorithm
            // scale up to a multiple of 2^depth + 1
            uint32_t scale = 1u << m_heightScale;
            uint32_t sdWidth = ((width + (scale - 1)) / scale) * scale + 1;
            uint32_t sdHeight = ((height + (scale - 1)) / scale) * scale + 1;
            uint8_t* tmpHeightMap = squareDiamond( sdWidth, sdHeight, scale, m_heightNoise, rnd );

            // apply cliffs
            uint8_t* tmpCliffMap = squareDiamond( sdWidth, sdHeight, 1 << m_cliffScale, m_cliffNoise, rnd );
            uint8_t* scratchHeightMap = tmpHeightMap;
            uint8_t* scratchCliffMap = tmpCliffMap;
            for ( int i = 0; i < sdWidth * sdHeight; ++i ) {
                *scratchHeightMap >>= 4u;
                if ( *scratchCliffMap < m_cliffThreshold ) {
                    // clamp to nearest plateau
                    *scratchHeightMap = (*scratchHeightMap + 2u) & 0b1111'1100u;
                    if ( *scratchHeightMap > 0b0000'1111u ) {
                        *scratchHeightMap = 0b0000'1111u;
                    }
                }
                ++scratchHeightMap;
                ++scratchCliffMap;
            }
            delete[] tmpCliffMap;

            // copy data to actual map
            uint8_t* heightMap = terrain->heightMap();
            for ( int y = 0; y < height; ++y ) {
                memcpy( heightMap + (y * width), tmpHeightMap + (y * sdWidth), width );
            }
            delete[] tmpHeightMap;

            // smooth out the map, get rid of peaks and holes
            for ( int p = 0; p < 4; ++p ) {
                for ( int y = 0; y < height; ++y ) {
                    for ( int x = 0; x < width; ++x ) {
                        auto h = heightMap[y * width + x];
                        auto h2 = safe_height( heightMap, width, height, x, y - 1 );
                        auto h4 = safe_height( heightMap, width, height, x + 1, y );
                        auto h6 = safe_height( heightMap, width, height, x, y + 1 );
                        auto h8 = safe_height( heightMap, width, height, x - 1, y );

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
                    auto h1 = safe_height( heightMap, width, height, x - 1, y - 1 );
                    auto h2 = safe_height( heightMap, width, height, x, y - 1 );
                    auto h3 = safe_height( heightMap, width, height, x + 1, y - 1 );
                    auto h4 = safe_height( heightMap, width, height, x + 1, y );
                    auto h5 = safe_height( heightMap, width, height, x + 1, y + 1 );
                    auto h6 = safe_height( heightMap, width, height, x, y + 1 );
                    auto h7 = safe_height( heightMap, width, height, x - 1, y + 1 );
                    auto h8 = safe_height( heightMap, width, height, x - 1, y );

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
            uint8_t* scratchSlope = terrain->slopeMap();
            for ( int y = 0; y < height; ++y ) {
                for ( int x = 0; x < width; ++x ) {
                    auto h = heightMap[y * width + x];
                    auto h1 = safe_height( heightMap, width, height, x - 1, y - 1 );
                    auto h2 = safe_height( heightMap, width, height, x, y - 1 );
                    auto h3 = safe_height( heightMap, width, height, x + 1, y - 1 );
                    auto h4 = safe_height( heightMap, width, height, x + 1, y );
                    auto h5 = safe_height( heightMap, width, height, x + 1, y + 1 );
                    auto h6 = safe_height( heightMap, width, height, x, y + 1 );
                    auto h7 = safe_height( heightMap, width, height, x - 1, y + 1 );
                    auto h8 = safe_height( heightMap, width, height, x - 1, y );

                    uint8_t slope = 0;
                    if ( (h8 < h || h1 < h || h2 < h) && !(h8 < h - 1 || h1 < h - 1 || h2 < h - 1) ) {
                        slope |= 0b0000'0001u;
                    }

                    if ( (h2 < h || h3 < h || h4 < h) && !(h2 < h - 1 || h3 < h - 1 || h4 < h - 1) ) {
                        slope |= 0b0000'0010u;
                    }

                    if ( (h4 < h || h5 < h || h6 < h) && !(h4 < h - 1 || h5 < h - 1 || h6 < h - 1) ) {
                        slope |= 0b0000'0100u;
                    }

                    if ( (h6 < h || h7 < h || h8 < h) && !(h6 < h - 1 || h7 < h - 1 || h8 < h - 1) ) {
                        slope |= 0b0000'1000u;
                    }
                    *scratchSlope = slope;
                    ++scratchSlope;
                }
            }

            // calculate if the tile has a cliff side
            uint8_t* slopeMap = terrain->slopeMap();
            for ( int y = 0; y < height; ++y ) {
                for ( int x = 0; x < width; ++x ) {
                    // get tile corners
                    auto c0 = getCorner( heightMap, slopeMap, width, height, x, y, 0 );
                    auto c1 = getCorner( heightMap, slopeMap, width, height, x, y, 1 );
                    auto c2 = getCorner( heightMap, slopeMap, width, height, x, y, 2 );
                    auto c3 = getCorner( heightMap, slopeMap, width, height, x, y, 3 );

                    // get adjacent corners
                    auto c03 = getCorner( heightMap, slopeMap, width, height, x, y - 1, 3 );
                    auto c02 = getCorner( heightMap, slopeMap, width, height, x, y - 1, 2 );

                    auto c10 = getCorner( heightMap, slopeMap, width, height, x + 1, y, 0 );
                    auto c13 = getCorner( heightMap, slopeMap, width, height, x + 1, y, 3 );

                    auto c21 = getCorner( heightMap, slopeMap, width, height, x, y + 1, 1 );
                    auto c20 = getCorner( heightMap, slopeMap, width, height, x, y + 1, 0 );

                    auto c32 = getCorner( heightMap, slopeMap, width, height, x - 1, y, 2 );
                    auto c31 = getCorner( heightMap, slopeMap, width, height, x - 1, y, 1 );

                    if ( c0 > c03 || c1 > c02 ) {
                        slopeMap[y * width + x] |= 0b0001'0000u;
                    }
                    if ( c1 > c10 || c2 > c13 ) {
                        slopeMap[y * width + x] |= 0b0010'0000u;
                    }
                    if ( c2 > c21 || c3 > c20 ) {
                        slopeMap[y * width + x] |= 0b0100'0000u;
                    }
                    if ( c3 > c32 || c0 > c31 ) {
                        slopeMap[y * width + x] |= 0b1000'0000u;
                    }
                }
            }


            generateOreMap( terrain->oreMap(), width, height, rnd );

            return terrain;
        }

    }
}
