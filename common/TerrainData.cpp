#include <list>
#include "FootPrint.h"
#include "TerrainData.h"

namespace isomap {
    namespace common {


        // use the slope constants to set bias for moving up and down hills
        //static const int8_t s_slopeConstants[8] = { 0, 1, 2, 1, 0, -2, -4, -2 };
        //static const int8_t s_slopeConstants[8] = { -6, -6, -6, -6, -6, -6, -6, -6 };
        static const int8_t s_slopeConstants[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        static const uint8_t s_slopeOffsets[16] = {8, 3, 1, 2, 7, 8, 0, 1, 5, 4, 8, 3, 6, 5, 7, 8};
        //static const uint8_t s_slopeOffsets[16] = { 8, 3, 1, 2, 7, 0, 0, 1, 5, 4, 0, 3, 6, 5, 7, 8 };

        int8_t slope( uint8_t slopeBits, uint8_t orientation ) {
            return 0;
            /*if ( s_slopeOffsets[orientation] == 8 ) {
                return 0;
            }
            slopeBits += s_slopeOffsets[orientation];
            return -s_slopeConstants[slopeBits % 16u];*/
        }

        TerrainData::TerrainData( uint32_t width, uint32_t height ) :
                mapWidth( width ),
                mapHeight( height ),
                heightMap( new uint8_t[mapWidth * mapHeight]() ),
                slopeMap( new uint8_t[mapWidth * mapHeight]() ),
                oreMap( new uint8_t[mapWidth * mapHeight]() ),
                occupancyMap( new uint8_t[mapWidth * mapHeight]() ),
                pathMap( new uint8_t[mapWidth * mapHeight]() ) {
        }

        TerrainData::~TerrainData() {
            delete[] heightMap;
            delete[] slopeMap;
            delete[] oreMap;
            delete[] occupancyMap;
            delete[] pathMap;
        }

        void TerrainData::occupy( uint32_t worldX, uint32_t worldY, const common::FootPrint* footPrint ) {
            // occupy area
            for ( uint32_t y = 0; y < footPrint->height(); ++y ) {
                for ( uint32_t x = 0; x < footPrint->width(); ++x ) {
                    uint8_t footPrintBits = footPrint->get( x, y );
                    if ( footPrintBits != 0 ) {
                        occupancyMap[(y + worldY) * mapWidth + (x + worldX)] |=
                                uint8_t( footPrintBits & occupancy::maskStructureBits );
                    }
                }
            }
            updatePathMap( worldX, worldY, footPrint->width(), footPrint->height() );
        }

        void TerrainData::vacate( uint32_t worldX, uint32_t worldY, const common::FootPrint* footPrint ) {
            // vacate area
            for ( uint32_t y = 0; y < footPrint->height(); ++y ) {
                for ( uint32_t x = 0; x < footPrint->width(); ++x ) {
                    if ( footPrint->get( x, y ) != 0 ) {
                        occupancyMap[(y + worldY) * mapWidth + (x + worldX)] &= uint8_t(
                                ~occupancy::maskStructureBits );
                    }
                }
            }
            updatePathMap( worldX, worldY, footPrint->width(), footPrint->height() );
        }

        void TerrainData::reserve( uint32_t worldX, uint32_t worldY, const common::FootPrint* footPrint ) {
            // reserve area
            for ( uint32_t y = 0; y < footPrint->height(); ++y ) {
                for ( uint32_t x = 0; x < footPrint->width(); ++x ) {
                    if ( footPrint->get( x, y ) != 0 ) {
                        occupancyMap[(y + worldY) * mapWidth + (x + worldX)] |= occupancy::bitReserved;
                    }
                }
            }
        }

        void TerrainData::unreserve( uint32_t worldX, uint32_t worldY, const common::FootPrint* footPrint ) {
            // unreserve area
            for ( uint32_t y = 0; y < footPrint->height(); ++y ) {
                for ( uint32_t x = 0; x < footPrint->width(); ++x ) {
                    if ( footPrint->get( x, y ) != 0 ) {
                        occupancyMap[(y + worldY) * mapWidth + (x + worldX)] &= ~occupancy::bitReserved;
                    }
                }
            }
        }

        void TerrainData::updatePathMap() {
            updatePathMap( 0, 0, mapWidth, mapHeight );
        }

        void TerrainData::updatePathMap( uint32_t x, uint32_t y, uint32_t width, uint32_t height ) {
            // update (part of) path map
            // we actually also need to update the tiles touching the given area

            // so first we need to calculate the actual area that needs to be updated
            uint32_t startX = x > 0 ? x - 1 : x;
            uint32_t startY = y > 0 ? y - 1 : y;
            uint32_t endX = std::min( x + width + 1, mapWidth );
            uint32_t endY = std::min( y + height + 1, mapHeight );
            // for every tile, we store whether any of the eight surrounding tiles can be reached
            // height difference between two tiles may not be more than 1
            // for diagonal movement, both the horizontal and vertical movement must be possible
            int32_t stride = mapWidth; // NOTE: needs to be signed, will get "funny" results when negating when unsigned
            for ( uint32_t tileY = startY; tileY < endY; ++tileY ) {
                auto* tmpPathMap = pathMap + tileY * stride + startX;
                auto* tmpHeightMap = heightMap + tileY * stride + startX;
                auto* tmpOccupancyMap = occupancyMap + tileY * stride + startX;
                for ( uint32_t tileX = startX; tileX < endX; ++tileX ) {
                    uint8_t pathBits = 0;
                    uint8_t h = tmpHeightMap[0];

                    // bit 4, down, x, y - 1
                    if ( tileY > 0 ) {
                        uint8_t hDown = tmpHeightMap[-stride];
                        uint8_t oDown = tmpOccupancyMap[-stride];
                        if ( (hDown == h || hDown - h == 1 || h - hDown == 1) &&
                             (oDown & occupancy::bitObstructed) == 0 && hDown >= 4 ) {
                            pathBits |= path::bitDown;
                        }
                    }

                    // bit 2, right, x + 1, y
                    if ( tileX < mapWidth - 1 ) {
                        uint8_t hRight = tmpHeightMap[1];
                        uint8_t oRight = tmpOccupancyMap[1];
                        if ( (hRight == h || hRight - h == 1 || h - hRight == 1) &&
                             (oRight & occupancy::bitObstructed) == 0 && hRight >= 4 ) {
                            pathBits |= path::bitRight;
                        }
                    }

                    // bit 0, up, x, y + 1
                    if ( tileY < mapHeight - 1 ) {
                        uint8_t hUp = tmpHeightMap[stride];
                        uint8_t oUp = tmpOccupancyMap[stride];
                        if ( (hUp == h || hUp - h == 1 || h - hUp == 1) &&
                             (oUp & occupancy::bitObstructed) == 0 && hUp >= 4 ) {
                            pathBits |= path::bitUp;
                        }
                    }

                    // bit 6, left, x - 1, y
                    if ( tileX > 0 ) {
                        uint8_t hLeft = tmpHeightMap[-1];
                        uint8_t oLeft = tmpOccupancyMap[-1];
                        if ( (hLeft == h || hLeft - h == 1 || h - hLeft == 1) &&
                             (oLeft & occupancy::bitObstructed) == 0 && hLeft >= 4 ) {
                            pathBits |= path::bitLeft;
                        }
                    }

                    // bit 5, down left, x - 1, y - 1
                    if ( tileX > 0 && tileY > 0 && (pathBits & path::bitDownOrLeft) == path::bitDownOrLeft ) {
                        uint8_t hDownLeft = tmpHeightMap[-stride - 1];
                        uint8_t oDownLeft = tmpOccupancyMap[-stride - 1];
                        if ( (hDownLeft == h || hDownLeft - h == 1 || h - hDownLeft == 1) &&
                             (oDownLeft & occupancy::bitObstructed) == 0 && hDownLeft >= 4 ) {
                            pathBits |= path::bitDownLeft;
                        }
                    }

                    // bit 3, down right, x + 1, y - 1
                    if ( tileX < mapWidth - 1 && tileY > 0 &&
                         (pathBits & path::bitDownOrRight) == path::bitDownOrRight ) {
                        uint8_t hDownRight = tmpHeightMap[-stride + 1];
                        uint8_t oDownRight = tmpOccupancyMap[-stride + 1];
                        if ( (hDownRight == h || hDownRight - h == 1 || h - hDownRight == 1) &&
                             (oDownRight & occupancy::bitObstructed) == 0 && hDownRight >= 4 ) {
                            pathBits |= path::bitDownRight;
                        }
                    }

                    // bit 1, up right, x + 1, y + 1
                    if ( tileX < mapWidth - 1 && tileY < mapHeight - 1 &&
                         (pathBits & path::bitUpOrRight) == path::bitUpOrRight ) {
                        uint8_t hUpRight = tmpHeightMap[stride + 1];
                        uint8_t oUpRight = tmpOccupancyMap[stride + 1];
                        if ( (hUpRight == h || hUpRight - h == 1 || h - hUpRight == 1) &&
                             (oUpRight & occupancy::bitObstructed) == 0 && hUpRight >= 4 ) {
                            pathBits |= path::bitUpRight;
                        }
                    }

                    // bit 7, up left, x -1, y + 1
                    if ( tileX > 0 && tileY < mapHeight - 1 && (pathBits & path::bitUpOrLeft) == path::bitUpOrLeft ) {
                        uint8_t hUpLeft = tmpHeightMap[stride - 1];
                        uint8_t oUpLeft = tmpOccupancyMap[stride - 1];
                        if ( (hUpLeft == h || hUpLeft - h == 1 || h - hUpLeft == 1) &&
                             (oUpLeft & occupancy::bitObstructed) == 0 && hUpLeft >= 4 ) {
                            pathBits |= path::bitUpLeft;
                        }
                    }

                    *tmpPathMap = pathBits;
                    ++tmpPathMap;
                    ++tmpHeightMap;
                    ++tmpOccupancyMap;
                }
            }
        }

        void TerrainData::flatten( uint32_t x, uint32_t y ) {
            uint32_t idx = y * mapWidth + x;
            uint8_t slope = slopeMap[idx];
            if ( (slope & 0b0000'1111u) != 0 ) {
                slopeMap[idx] = 0;
                updateCliffs( x, y, 1 , 1);
            }
        }

        uint8_t TerrainData::corner( int32_t x, int32_t y, uint32_t i ) const {
            if ( x < 0 ) {
                x = 0;
                if ( i == 1 ) {
                    i = 0;
                } else if ( i == 2 ) {
                    i = 3;
                }
            } else if ( x >= mapWidth ) {
                x = (int)mapWidth - 1;
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
            } else if ( y >= mapHeight ) {
                y = (int)mapHeight - 1;
                if ( i == 0 ) {
                    i = 3;
                } else if ( i == 1 ) {
                    i = 2;
                }
            }
            uint32_t idx = y * mapWidth + x;
            return heightMap[idx] - (uint8_t( slopeMap[idx] >> uint32_t( i ) ) & 0b0000'0001u);
        }

        uint8_t TerrainData::calcCliffBits( uint32_t x, uint32_t y ) const {
            auto c0 = corner( x, y, 0 );
            auto c1 = corner( x, y, 1 );
            auto c2 = corner( x, y, 2 );
            auto c3 = corner( x, y, 3 );

            // get adjacent corners
            auto c03 = corner( x, y - 1, 3 );
            auto c02 = corner( x, y - 1, 2 );

            auto c10 = corner( x + 1, y, 0 );
            auto c13 = corner( x + 1, y, 3 );

            auto c21 = corner( x, y + 1, 1 );
            auto c20 = corner( x, y + 1, 0 );

            auto c32 = corner( x - 1, y, 2 );
            auto c31 = corner( x - 1, y, 1 );

            uint8_t cliffBits = 0;

            if ( c0 > c03 || c1 > c02 ) {
                cliffBits |= 0b0001'0000u;
            }
            if ( c1 > c10 || c2 > c13 ) {
                cliffBits |= 0b0010'0000u;
            }
            if ( c2 > c21 || c3 > c20 ) {
                cliffBits |= 0b0100'0000u;
            }
            if ( c3 > c32 || c0 > c31 ) {
                cliffBits |= 0b1000'0000u;
            }
            return cliffBits;
        }


        void TerrainData::updateCliffs() {
            updateCliffs( 0, 0, mapWidth, mapHeight );
        }

        void TerrainData::updateCliffs( uint32_t x, uint32_t y, uint32_t width, uint32_t height ) {
            uint32_t startX = x > 0 ? x - 1 : x;
            uint32_t startY = y > 0 ? y - 1 : y;
            uint32_t endX = std::min( x + width + 1, mapWidth );
            uint32_t endY = std::min( y + height + 1, mapHeight );

            for ( int tileY = startY; tileY < endY; ++tileY ) {
                for ( int tileX = startX; tileX < endX; ++tileX ) {
                    uint32_t idx = tileY * mapWidth + tileX;
                    slopeMap[idx] = (slopeMap[idx] & 0b0000'1111u) | calcCliffBits( tileX, tileY );
                }
            }
        }


    }
}
