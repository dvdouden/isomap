#include <list>
#include "FootPrint.h"
#include "TerrainData.h"

namespace isomap {
    namespace common {

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
                        occupancyMap[(y + worldY) * mapWidth + (x + worldX)] |= footPrintBits & 0b0000'0011u;
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
                        occupancyMap[(y + worldY) * mapWidth + (x + worldX)] &= ~0b0000'0011u;
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
                        occupancyMap[(y + worldY) * mapWidth + (x + worldX)] |= 0b0000'0100u;
                    }
                }
            }
        }

        void TerrainData::unreserve( uint32_t worldX, uint32_t worldY, const common::FootPrint* footPrint ) {
            // unreserve area
            for ( uint32_t y = 0; y < footPrint->height(); ++y ) {
                for ( uint32_t x = 0; x < footPrint->width(); ++x ) {
                    if ( footPrint->get( x, y ) != 0 ) {
                        occupancyMap[(y + worldY) * mapWidth + (x + worldX)] &= ~0b0000'0100u;
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
            // so first we need to calculate the actual arrea that needs to be updated
            uint32_t startX = x > 0 ? x - 1 : x;
            uint32_t startY = y > 0 ? y - 1 : y;
            uint32_t endX = std::min( x + width, mapWidth );
            uint32_t endY = std::min( y + height, mapHeight );
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

                    // bit 1, up, x, y - 1
                    if ( tileY > 0 ) {
                        uint8_t hUp = tmpHeightMap[-stride];
                        uint8_t oUp = tmpOccupancyMap[-stride];
                        if ( (hUp == h || hUp - h == 1 || h - hUp == 1) && (oUp & 0b0000'0001u) == 0 ) {
                            pathBits |= 0b0000'0010u;
                        }
                    }

                    // bit 3, right, x + 1, y
                    if ( tileX < mapWidth - 1 ) {
                        uint8_t hRight = tmpHeightMap[1];
                        uint8_t oRight = tmpOccupancyMap[1];
                        if ( (hRight == h || hRight - h == 1 || h - hRight == 1) && (oRight & 0b0000'0001u) == 0 ) {
                            pathBits |= 0b0000'1000u;
                        }
                    }

                    // bit 5, down, x, y + 1
                    if ( tileY < mapHeight - 1 ) {
                        uint8_t hDown = tmpHeightMap[stride];
                        uint8_t oDown = tmpOccupancyMap[stride];
                        if ( (hDown == h || hDown - h == 1 || h - 54 == 1) && (oDown & 0b0000'0001u) == 0 ) {
                            pathBits |= 0b0010'0000u;
                        }
                    }

                    // bit 7, left, x - 1, y
                    if ( tileX > 0 ) {
                        uint8_t hLeft = tmpHeightMap[-1];
                        uint8_t oLeft = tmpOccupancyMap[-1];
                        if ( (hLeft == h || hLeft - h == 1 || h - hLeft == 1) && (oLeft & 0b0000'0001u) == 0 ) {
                            pathBits |= 0b1000'0000u;
                        }
                    }

                    // bit 0, up left, x - 1, y - 1
                    if ( tileX > 0 && tileY > 0 && (pathBits & 0b1000'0010u) == 0b1000'0010u ) {
                        uint8_t hUpLeft = tmpHeightMap[-stride - 1];
                        uint8_t oUpLeft = tmpOccupancyMap[-stride - 1];
                        if ( (hUpLeft == h || hUpLeft - h == 1 || h - hUpLeft == 1) && (oUpLeft & 0b0000'0001u) == 0 ) {
                            pathBits |= 0b0000'0001u;
                        }
                    }

                    // bit 2, up right, x + 1, y - 1
                    if ( tileX < mapWidth - 1 && tileY > 0 && (pathBits & 0b0000'1010u) == 0b0000'1010u ) {
                        uint8_t hUpRight = tmpHeightMap[-stride + 1];
                        uint8_t oUpRight = tmpOccupancyMap[-stride + 1];
                        if ( (hUpRight == h || hUpRight - h == 1 || h - hUpRight == 1) &&
                             (oUpRight & 0b0000'0001u) == 0 ) {
                            pathBits |= 0b0000'0100u;
                        }
                    }

                    // bit 4, down right, x + 1, y + 1
                    if ( tileX < mapWidth - 1 && tileY < mapHeight - 1 && (pathBits & 0b0010'1000u) == 0b0010'1000u ) {
                        uint8_t hDownRight = tmpHeightMap[stride + 1];
                        uint8_t oDownRight = tmpOccupancyMap[stride + 1];
                        if ( (hDownRight == h || hDownRight - h == 1 || h - hDownRight == 1) &&
                             (oDownRight & 0b0000'0001u) == 0 ) {
                            pathBits |= 0b0001'0000u;
                        }
                    }

                    // bit 6, down left, x -1, y + 1
                    if ( tileX > 0 && tileY < mapHeight - 1 && (pathBits & 0b1010'0000u) == 0b1010'0000u ) {
                        uint8_t hDownLeft = tmpHeightMap[stride - 1];
                        uint8_t oDownLeft = tmpOccupancyMap[stride - 1];
                        if ( (hDownLeft == h || hDownLeft - h == 1 || h - hDownLeft == 1) &&
                             (oDownLeft & 0b0000'0001u) == 0 ) {
                            pathBits |= 0b0100'0000u;
                        }
                    }

                    *tmpPathMap = pathBits;
                    ++tmpPathMap;
                    ++tmpHeightMap;
                    ++tmpOccupancyMap;
                }
            }
        }

    }
}
