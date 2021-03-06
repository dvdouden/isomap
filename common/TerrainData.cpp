#include <list>
#include <cstring>
#include <vector>
#include "FootPrint.h"
#include "TerrainData.h"
#include "../util/math.h"

namespace isomap {
    namespace common {


        // use the slope constants to set bias for moving up and down hills
        //static const int8_t s_slopeConstants[8] = { 0, 1, 2, 1, 0, -2, -4, -2 };
        //static const int8_t s_slopeConstants[8] = { -6, -6, -6, -6, -6, -6, -6, -6 };
        static const int8_t s_slopeConstants[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        static const uint8_t s_slopeOffsets[16] = {8, 3, 1, 2, 7, 8, 0, 1, 5, 4, 8, 3, 6, 5, 7, 8};
        //static const uint8_t s_slopeOffsets[16] = { 8, 3, 1, 2, 7, 0, 0, 1, 5, 4, 0, 3, 6, 5, 7, 8 };

        int8_t slopeAngle( uint8_t slopeBits, uint8_t orientation ) {
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
                        occupancyMap[(y + worldY) * mapWidth + (x + worldX)] &= uint8_t( ~occupancy::bitReserved );
                    }
                }
            }
        }

        void TerrainData::reserveUnit( uint32_t worldX, uint32_t worldY ) {
            occupancyMap[worldY * mapWidth + worldX] |= occupancy::bitUnit;
        }

        void TerrainData::unreserveUnit( uint32_t worldX, uint32_t worldY ) {
            occupancyMap[worldY * mapWidth + worldX] &= uint8_t( ~occupancy::bitUnit );
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

            // we've already calculated whether there's a height difference between two adjacent tiles
            // so all we need to do is check the cliff bits
            // for diagonal movement, both the horizontal and vertical movement must be possible
            int32_t stride = mapWidth; // NOTE: needs to be signed, will get "funny" results when negating when unsigned
            for ( uint32_t tileY = startY; tileY < endY; ++tileY ) {
                auto* tmpPathMap = pathMap + tileY * stride + startX;
                auto* tmpSlopeMap = slopeMap + tileY * stride + startX;
                auto* tmpHeightMap = heightMap + tileY * stride + startX;
                auto* tmpOccupancyMap = occupancyMap + tileY * stride + startX;
                for ( uint32_t tileX = startX; tileX < endX; ++tileX ) {
                    uint8_t pathBits = 0;
                    uint8_t slope = tmpSlopeMap[0];

                    // bit 4, down, x, y - 1
                    if ( tileY > 0 && (slope & slope::bitCliffDown) == 0 ) {
                        uint8_t slopeDown = tmpSlopeMap[-stride] & slope::bitCliffUp;
                        uint8_t heightDown = tmpHeightMap[-stride];
                        uint8_t occupancyDown = tmpOccupancyMap[-stride];
                        if ( slopeDown == 0 &&
                             (occupancyDown & occupancy::bitObstructed) == 0 && heightDown >= 4 ) {
                            pathBits |= path::bitDown;
                        }
                    }

                    // bit 2, right, x + 1, y
                    if ( tileX < mapWidth - 1 && (slope & slope::bitCliffRight) == 0 ) {
                        uint8_t slopeRight = tmpSlopeMap[1] & slope::bitCliffLeft;
                        uint8_t heightRight = tmpHeightMap[1];
                        uint8_t occupancyRight = tmpOccupancyMap[1];
                        if ( slopeRight == 0 &&
                             (occupancyRight & occupancy::bitObstructed) == 0 && heightRight >= 4 ) {
                            pathBits |= path::bitRight;
                        }
                    }

                    // bit 0, up, x, y + 1
                    if ( tileY < mapHeight - 1 && (slope & slope::bitCliffUp) == 0 ) {
                        uint8_t slopeUp = tmpSlopeMap[stride] & slope::bitCliffDown;
                        uint8_t heightUp = tmpHeightMap[stride];
                        uint8_t occupancyUp = tmpOccupancyMap[stride];
                        if ( slopeUp == 0 &&
                             (occupancyUp & occupancy::bitObstructed) == 0 && heightUp >= 4 ) {
                            pathBits |= path::bitUp;
                        }
                    }

                    // bit 6, left, x - 1, y
                    if ( tileX > 0 && (slope & slope::bitCliffLeft) == 0 ) {
                        uint8_t slopeLeft = tmpSlopeMap[-1] & slope::bitCliffRight;
                        uint8_t heightLeft = tmpHeightMap[-1];
                        uint8_t occupancyLeft = tmpOccupancyMap[-1];
                        if ( slopeLeft == 0 &&
                             (occupancyLeft & occupancy::bitObstructed) == 0 && heightLeft >= 4 ) {
                            pathBits |= path::bitLeft;
                        }
                    }

                    // bit 5, down left, x - 1, y - 1
                    if ( tileX > 0 && tileY > 0 && (pathBits & path::bitDownOrLeft) == path::bitDownOrLeft ) {
                        uint8_t slopeDownLeft =
                                tmpSlopeMap[-stride - 1] & uint8_t( slope::bitCliffUp | slope::bitCliffRight );
                        uint8_t hDownLeft = tmpHeightMap[-stride - 1];
                        uint8_t oDownLeft = tmpOccupancyMap[-stride - 1];
                        if ( slopeDownLeft == 0 &&
                             (oDownLeft & occupancy::bitObstructed) == 0 && hDownLeft >= 4 ) {
                            pathBits |= path::bitDownLeft;
                        }
                    }

                    // bit 3, down right, x + 1, y - 1
                    if ( tileX < mapWidth - 1 && tileY > 0 &&
                         (pathBits & path::bitDownOrRight) == path::bitDownOrRight ) {
                        uint8_t slopeDownRight =
                                tmpSlopeMap[-stride + 1] & uint8_t( slope::bitCliffUp | slope::bitCliffLeft );
                        uint8_t hDownRight = tmpHeightMap[-stride + 1];
                        uint8_t oDownRight = tmpOccupancyMap[-stride + 1];
                        if ( slopeDownRight == 0 &&
                             (oDownRight & occupancy::bitObstructed) == 0 && hDownRight >= 4 ) {
                            pathBits |= path::bitDownRight;
                        }
                    }

                    // bit 1, up right, x + 1, y + 1
                    if ( tileX < mapWidth - 1 && tileY < mapHeight - 1 &&
                         (pathBits & path::bitUpOrRight) == path::bitUpOrRight ) {
                        uint8_t slopeUpRight =
                                tmpSlopeMap[stride + 1] & uint8_t( slope::bitCliffDown | slope::bitCliffLeft );
                        uint8_t hUpRight = tmpHeightMap[stride + 1];
                        uint8_t oUpRight = tmpOccupancyMap[stride + 1];
                        if ( slopeUpRight == 0 &&
                             (oUpRight & occupancy::bitObstructed) == 0 && hUpRight >= 4 ) {
                            pathBits |= path::bitUpRight;
                        }
                    }

                    // bit 7, up left, x -1, y + 1
                    if ( tileX > 0 && tileY < mapHeight - 1 && (pathBits & path::bitUpOrLeft) == path::bitUpOrLeft ) {
                        uint8_t slopeUpLeft =
                                tmpSlopeMap[stride - 1] & uint8_t( slope::bitCliffDown | slope::bitCliffRight );
                        uint8_t hUpLeft = tmpHeightMap[stride - 1];
                        uint8_t oUpLeft = tmpOccupancyMap[stride - 1];
                        if ( slopeUpLeft == 0 &&
                             (oUpLeft & occupancy::bitObstructed) == 0 && hUpLeft >= 4 ) {
                            pathBits |= path::bitUpLeft;
                        }
                    }

                    *tmpPathMap = pathBits;
                    ++tmpPathMap;
                    ++tmpSlopeMap;
                    ++tmpHeightMap;
                    ++tmpOccupancyMap;
                }
            }
        }

        void TerrainData::flatten( uint32_t x, uint32_t y ) {
            uint32_t idx = y * mapWidth + x;
            uint8_t h = heightMap[idx];

            uint8_t slope = slopeMap[idx];
            if ( (slope & slope::slopeMask) != 0 ) {
                heightMap[idx] = h - 1;
                if ( useSlopes ) {
                    updateSlopes( x, y, 1, 1 );
                }
                updateCliffs( x, y, 1, 1 );
                updatePathMap( x, y, 1, 1 );
            }
        }

        void TerrainData::flatten( uint32_t x, uint32_t y, uint8_t height ) {
            uint32_t idx = y * mapWidth + x;
            heightMap[idx] = height;
/*
            if ( x > 0 ) {
                if ( heightMap[idx - 1] == height - 1 ) {
                    heightMap[idx - 1] = height;
                }
            }
            if ( x < mapWidth - 1 ) {
                if ( heightMap[idx + 1] == height - 1 ) {
                    heightMap[idx + 1] = height;
                }
            }

            if ( y > 0 ) {
                if ( heightMap[idx - mapWidth] == height - 1 ) {
                    heightMap[idx - mapWidth] = height;
                }
            }
            if ( y < mapHeight - 1 ) {
                if ( heightMap[idx + mapWidth] == height - 1 ) {
                    heightMap[idx + mapWidth] = height;
                }
            }
            */
            if ( useSlopes ) {
                updateSlopes( x - 1, y - 1, 3, 3 );
            }
            updateCliffs( x - 1, y - 1, 3, 3 );
            updatePathMap( x - 1, y - 1, 3, 3 );
        }

        uint8_t TerrainData::safeCorner( int32_t x, int32_t y, uint32_t c ) const {
            if ( x < 0 ) {
                x = 0;
                if ( c == 1 ) {
                    c = 0;
                } else if ( c == 2 ) {
                    c = 3;
                }
            } else if ( x >= mapWidth ) {
                x = mapWidth - 1;
                if ( c == 0 ) {
                    c = 1;
                } else if ( c == 3 ) {
                    c = 2;
                }
            }
            if ( y < 0 ) {
                y = 0;
                if ( c == 2 ) {
                    c = 1;
                } else if ( c == 3 ) {
                    c = 0;
                }
            } else if ( y >= mapHeight ) {
                y = mapHeight - 1;
                if ( c == 0 ) {
                    c = 3;
                } else if ( c == 1 ) {
                    c = 2;
                }
            }
            uint32_t idx = y * mapWidth + x;
            return heightMap[idx] + (uint8_t( slopeMap[idx] >> uint32_t( c ) ) & 0b0000'0001u);
        }

        uint8_t TerrainData::corner( uint32_t x, uint32_t y, uint32_t c ) const {
            uint32_t idx = y * mapWidth + x;
            return heightMap[idx] + (uint8_t( slopeMap[idx] >> uint32_t( c ) ) & 0b0000'0001u);
        }

        uint8_t TerrainData::safeHeight( int32_t x, int32_t y ) const {
            if ( y < 0 ) {
                y = 0;
            }
            if ( y >= mapHeight ) {
                y = mapHeight - 1;
            }
            if ( x < 0 ) {
                x = 0;
            }
            if ( x >= mapWidth ) {
                x = mapWidth - 1;
            }
            return heightMap[y * mapWidth + x];
        }

        uint8_t TerrainData::calcCliffBits( uint32_t x, uint32_t y ) const {
            auto c0 = safeCorner( x, y, 0 );
            auto c1 = safeCorner( x, y, 1 );
            auto c2 = safeCorner( x, y, 2 );
            auto c3 = safeCorner( x, y, 3 );

            // get adjacent corners
            auto c03 = safeCorner( x, y - 1, 3 );
            auto c02 = safeCorner( x, y - 1, 2 );

            auto c10 = safeCorner( x + 1, y, 0 );
            auto c13 = safeCorner( x + 1, y, 3 );

            auto c21 = safeCorner( x, y + 1, 1 );
            auto c20 = safeCorner( x, y + 1, 0 );

            auto c32 = safeCorner( x - 1, y, 2 );
            auto c31 = safeCorner( x - 1, y, 1 );

            uint8_t cliffBits = 0;

            if ( c0 != c03 || c1 != c02 ) {
                cliffBits |= slope::bitCliffDown;
            }
            if ( c1 != c10 || c2 != c13 ) {
                cliffBits |= slope::bitCliffRight;
            }
            if ( c2 != c21 || c3 != c20 ) {
                cliffBits |= slope::bitCliffUp;
            }
            if ( c3 != c32 || c0 != c31 ) {
                cliffBits |= slope::bitCliffLeft;
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
                    slopeMap[idx] = uint8_t( slopeMap[idx] & slope::slopeMask ) | calcCliffBits( tileX, tileY );
                }
            }
        }

        void TerrainData::updateSlopes() {
            updateSlopes( 0, 0, mapWidth, mapHeight );
        }

        void TerrainData::updateSlopes( uint32_t x, uint32_t y, uint32_t width, uint32_t height ) {
            uint32_t startX = x > 0 ? x - 1 : x;
            uint32_t startY = y > 0 ? y - 1 : y;
            uint32_t endX = std::min( x + width + 1, mapWidth );
            uint32_t endY = std::min( y + height + 1, mapHeight );

            for ( int tileY = startY; tileY < endY; ++tileY ) {
                for ( int tileX = startX; tileX < endX; ++tileX ) {
                    uint32_t idx = tileY * mapWidth + tileX;
                    slopeMap[idx] = calcSlopeBits( tileX, tileY );
                    if ( slopeMap[idx] == 0b0000'1111u ) {
                        slopeMap[idx] = 0;
                        heightMap[idx]++;
                    }
                }
            }
        }

        uint8_t TerrainData::calcSlopeBits( uint32_t tileX, uint32_t tileY ) const {
            int32_t x = tileX;
            int32_t y = tileY;
            auto h = heightMap[y * mapWidth + x];
            auto h1 = safeHeight( x - 1, y - 1 );
            auto h2 = safeHeight( x, y - 1 );
            auto h3 = safeHeight( x + 1, y - 1 );
            auto h4 = safeHeight( x + 1, y );
            auto h5 = safeHeight( x + 1, y + 1 );
            auto h6 = safeHeight( x, y + 1 );
            auto h7 = safeHeight( x - 1, y + 1 );
            auto h8 = safeHeight( x - 1, y );
            /*if ( h == h2 && h == h4 && h == h6 && h == h8 ) {
                return 0;
            }*/

            uint8_t slope = 0;
            if ( (h8 > h || h1 > h || h2 > h) && !(h8 > h + 1 || h1 > h + 1 || h2 > h + 1) ) {
                slope |= slope::bitSlopeDownLeft;
            }

            if ( (h2 > h || h3 > h || h4 > h) && !(h2 > h + 1 || h3 > h + 1 || h4 > h + 1) ) {
                slope |= slope::bitSlopeDownRight;
            }

            if ( (h4 > h || h5 > h || h6 > h) && !(h4 > h + 1 || h5 > h + 1 || h6 > h + 1) ) {
                slope |= slope::bitSlopeUpRight;
            }

            if ( (h6 > h || h7 > h || h8 > h) && !(h6 > h + 1 || h7 > h + 1 || h8 > h + 1) ) {
                slope |= slope::bitSlopeUpLeft;
            }
/*
            if ( h == h2 ) {
                slope &= 0b0000'1100u;
            }
            if ( h == h4 ) {
                slope &= 0b0000'1001u;
            }
            if ( h == h6 ) {
                slope &= 0b0000'0011u;
            }
            if ( h == h8 ) {
                slope &= 0b0000'0110u;
            }
*/
            return slope;
        }

        void TerrainData::toggleSlopes() {
            useSlopes = !useSlopes;
            if ( !useSlopes ) {
                memset( slopeMap, 0, mapWidth * mapHeight );
            } else {
                updateSlopes();
            }
            updateCliffs();
            updatePathMap();
        }

        void TerrainData::raise( uint32_t x, uint32_t y ) {
            uint32_t idx = y * mapWidth + x;
            heightMap[idx]++;
            if ( useSlopes ) {
                updateSlopes( x, y, 1, 1 );
            }
            updateCliffs( x, y, 1, 1 );
            updatePathMap( x, y, 1, 1 );
        }

        void TerrainData::lower( uint32_t x, uint32_t y ) {
            uint32_t idx = y * mapWidth + x;
            heightMap[idx]--;
            if ( useSlopes ) {
                updateSlopes( x, y, 1, 1 );
            }
            updateCliffs( x, y, 1, 1 );
            updatePathMap( x, y, 1, 1 );
        }

        void TerrainData::incSlope( uint32_t x, uint32_t y ) {
            uint32_t idx = y * mapWidth + x;
            uint8_t slope = slopeMap[idx];
            slopeMap[idx] = (slope + 1u) % 16u;
            updateCliffs( x, y, 1, 1 );
            updatePathMap( x, y, 1, 1 );
        }

        void TerrainData::splode( uint32_t origX, uint32_t origY, uint32_t radius ) {

            std::vector<std::pair<uint32_t, int8_t>> radii;

            // calculate crater shape
            for ( int32_t r = 0; r < radius; ++r ) {
                radii.emplace_back( r * r, -(radius - r) / 2 );
                //printf( "crater %d (%d) = %d\n", r, radii.back().first, radii.back().second );
            }

            uint32_t lipWidth = radius / 2;
            // calculate lip shape
            for ( int32_t r = 0; r <= lipWidth; ++r ) {
                if ( r <= lipWidth / 2 ) {
                    radii.emplace_back( (radius + r) * (radius + r), r / 2 );
                } else {
                    radii.emplace_back( (radius + r) * (radius + r), (lipWidth - r) / 2 );
                }
                //printf( "lip %d (%d) = %d\n", r, radii.back().first, radii.back().second );
            }

            int8_t level = heightMap[origY * mapWidth + origX];

            radius += lipWidth;
            uint32_t startY = radius > origY ? 0 : origY - radius;
            uint32_t endY = radius + origY > mapHeight - 1 ? mapHeight - 1 : origY + radius;
            uint32_t startX = radius > origX ? 0 : origX - radius;
            uint32_t endX = radius + origX > mapWidth - 1 ? mapWidth - 1 : origX + radius;

            for ( int32_t y = startY; y <= endY; ++y ) {
                if ( y < 0 || y >= mapHeight ) {
                    continue;
                }

                int32_t deltaY = (y - int32_t( origY )) * (y - int32_t( origY ));
                for ( int32_t x = startX; x <= endX; ++x ) {
                    if ( x < 0 || x >= mapWidth ) {
                        continue;
                    }
                    int32_t deltaX = (x - int32_t( origX )) * (x - int32_t( origX ));

                    for ( auto& r : radii ) {
                        if ( deltaX + deltaY <= r.first ) {
                            int8_t newLevel = level + r.second;
                            //printf( "%d, %d in range %d, (%d) new level: %d\n", x, y, r.first, r.second, newLevel );
                            if ( newLevel < 0 ) {
                                newLevel = 0;
                            }
                            uint32_t idx = y * mapWidth + x;
                            uint8_t oldLevel = heightMap[idx];

                            int8_t newOtherLevel = oldLevel + r.second;
                            if ( newOtherLevel < 0 ) {
                                newOtherLevel = 0;
                            }
                            newLevel = (newLevel + newOtherLevel) / 2;
                            heightMap[idx] = uint8_t( newLevel );
                            break;
                        }
                    }
                }
            }
            if ( useSlopes ) {
                updateSlopes( startX, startY, endX - startX, endY - startY );
            }
            updateCliffs( startX, startY, endX - startX, endY - startY );
        }

        int32_t TerrainData::heightAt( uint32_t fixX, uint32_t fixY ) const {
            uint32_t tileX = fixX / math::fix::precision;
            uint32_t tileY = fixY / math::fix::precision;
            uint32_t subX = fixX % math::fix::precision;
            uint32_t subY = fixY % math::fix::precision;

            int32_t height = heightMap[tileY * mapWidth + tileX] * math::fix::precision;

            uint8_t slopes = slopeMap[tileY * mapWidth + tileX] & slope::slopeMask;

            switch ( slopes ) {
                default:
                case 0:
                    return height;

                case 0b0011u:   // south edge raised
                    return height + (math::fix::precision - subY);
                case 0b0110u:   // east edge raised
                    return height + subX;
                case 0b1100u:   // north edge raised
                    return height + subY;
                case 0b1001u:   // west edge raised
                    return height + (math::fix::precision - subX);

                case 0b0001u:   // southwest corner raised
                    if ( subX + subY > math::fix::precision ) {
                        return height + subX - subY;
                    } else {
                        return height + math::fix::precision - (subX + subY);
                    }
                case 0b0010u:   // southeast corner raised
                    if ( subX > subY ) { // southeast half
                        return height + subX - subY;
                    } else { // northwest half
                        return height;
                    }
                case 0b0100u:   // northeast corner raised
                    if ( subX + subY > math::fix::precision ) {
                        return height + subX + subY - math::fix::precision;
                    } else {
                        return height;
                    }
                case 0b1000u:   // northwest corner raised
                    if ( subX > subY ) { // southeast half
                        return height;
                    } else { // northwest half
                        return height + subY - subX;
                    }

                case 0b0101u:   // southwest and northeast corners raised
                    if ( subX > subY ) { // southeast half
                        return height + math::fix::precision - (subX - subY);
                    } else { // northwest half
                        return height + math::fix::precision - (subY - subX);
                    }
                case 0b1010u:   // southeast and northwest corners raised
                    if ( subX + subY > math::fix::precision ) {
                        return height + math::fix::precision - (subX + subY - math::fix::precision);
                    } else {
                        return height + subX + subY;
                    }

                case 0b0111u:   // all but northwest corner raised
                    if ( subX > subY ) { // southeast half
                        return height + math::fix::precision;
                    } else { // northwest half
                        return height + math::fix::precision - (subY - subX);
                    }
                case 0b1011u:   // all but northeast corner raised
                    if ( subX + subY > math::fix::precision ) {
                        return height + math::fix::precision - (subX + subY - math::fix::precision);
                    } else {
                        return height + math::fix::precision;
                    }
                case 0b1101u:   // all but southeast corner raised
                    if ( subX > subY ) { // southeast half
                        return height + math::fix::precision - (subX - subY);
                    } else { // northwest half
                        return height + math::fix::precision;
                    }
                case 0b1110u:   // all but southwest corner raised
                    if ( subX + subY > math::fix::precision ) {
                        return height + math::fix::precision;
                    } else {
                        return height + subX + subY;
                    }

                case 0b1111u:
                    return height + math::fix::precision;
            }
        }


    }
}
