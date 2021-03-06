#pragma once

#include "types.h"

namespace isomap {
    namespace common {
        namespace path {

            // define some bit masks, just to make things a bit more readable
            const uint8_t bitUp = 0b0000'0001u;
            const uint8_t bitUpRight = 0b0000'0010u;
            const uint8_t bitRight = 0b0000'0100u;
            const uint8_t bitDownRight = 0b0000'1000u;
            const uint8_t bitDown = 0b0001'0000u;
            const uint8_t bitDownLeft = 0b0010'0000u;
            const uint8_t bitLeft = 0b0100'0000u;
            const uint8_t bitUpLeft = 0b1000'0000u;

            const uint8_t bitUpOrLeft = bitUp | bitLeft;
            const uint8_t bitUpOrRight = bitUp | bitRight;
            const uint8_t bitDownOrLeft = bitDown | bitLeft;
            const uint8_t bitDownOrRight = bitDown | bitRight;
        }

        namespace slope {
            const uint8_t bitSlopeDownLeft = 0b0000'0001u;
            const uint8_t bitSlopeDownRight = 0b0000'0010u;
            const uint8_t bitSlopeUpRight = 0b0000'0100u;
            const uint8_t bitSlopeUpLeft = 0b0000'1000u;
            const uint8_t slopeMask = 0b0000'1111u;

            const uint8_t bitCliffDown = 0b0001'0000u;
            const uint8_t bitCliffRight = 0b0010'0000u;
            const uint8_t bitCliffUp = 0b0100'0000u;
            const uint8_t bitCliffLeft = 0b1000'0000u;
            const uint8_t cliffMask = 0b1111'0000u;
        }

        namespace occupancy {
            const uint8_t bitObstructed = 0b0000'0001u;
            const uint8_t bitConstructed = 0b0000'0010u;
            const uint8_t bitReserved = 0b0000'0100u;
            const uint8_t bitSpawnPoint = 0b0000'1000u;
            const uint8_t bitDockingPoint = 0b0001'0000u;
            const uint8_t bitUnit = 0b0010'0000u;
            const uint8_t bitDockAndSpawn = bitSpawnPoint | bitDockingPoint;
            const uint8_t bitsPassable = uint8_t( bitObstructed | bitReserved ) | bitUnit;
            const uint8_t maskStructureBits = uint8_t(
                    uint32_t( bitObstructed ) | bitConstructed | bitSpawnPoint | bitDockingPoint );
        }

        int8_t slopeAngle( uint8_t slopeBits, uint8_t orientation );


        // common data structure and methods
        struct TerrainData {
            uint32_t mapWidth = 0;
            uint32_t mapHeight = 0;

            uint8_t* heightMap = nullptr;
            uint8_t* slopeMap = nullptr;
            uint8_t* oreMap = nullptr;
            uint8_t* occupancyMap = nullptr;
            uint8_t* pathMap = nullptr;

            bool useSlopes = true;

            TerrainData( uint32_t width, uint32_t heigth );

            TerrainData( const TerrainData& ) = delete;

            ~TerrainData();

            const TerrainData& operator=( const TerrainData& ) = delete;


            void occupy( uint32_t x, uint32_t y, const common::FootPrint* footPrint );

            void vacate( uint32_t x, uint32_t y, const common::FootPrint* footPrint );

            void reserve( uint32_t x, uint32_t y, const common::FootPrint* footPrint );

            void unreserve( uint32_t x, uint32_t y, const common::FootPrint* footPrint );

            void reserveUnit( uint32_t x, uint32_t y );

            void unreserveUnit( uint32_t x, uint32_t y );

            bool reservedByUnit( uint32_t x, uint32_t y ) const {
                return (occupancy( x, y ) & occupancy::bitUnit) != 0;
            }

            void updatePathMap();

            void updatePathMap( uint32_t x, uint32_t y, uint32_t width, uint32_t height );

            void flatten( uint32_t x, uint32_t y );

            void flatten( uint32_t x, uint32_t y, uint8_t height );

            void raise( uint32_t x, uint32_t y );

            void lower( uint32_t x, uint32_t y );

            uint8_t safeCorner( int32_t x, int32_t y, uint32_t c ) const;

            uint8_t corner( uint32_t x, uint32_t y, uint32_t c ) const;

            uint8_t safeHeight( int32_t x, int32_t y ) const;

            void toggleSlopes();

            void incSlope( uint32_t x, uint32_t y );

            void updateCliffs();

            void updateCliffs( uint32_t x, uint32_t y, uint32_t width, uint32_t height );

            uint8_t calcCliffBits( uint32_t x, uint32_t y ) const;

            void updateSlopes();

            void updateSlopes( uint32_t x, uint32_t y, uint32_t width, uint32_t height );

            uint8_t calcSlopeBits( uint32_t x, uint32_t y ) const;

            void splode( uint32_t x, uint32_t y, uint32_t radius );

            int32_t heightAt( uint32_t fixX, uint32_t fixY ) const;

            uint8_t occupancy( uint32_t x, uint32_t y ) const {
                return occupancyMap[y * mapWidth + x];
            }

            bool impassable( uint32_t x, uint32_t y ) const {
                return (occupancy( x, y ) & occupancy::bitsPassable) != 0;
            }

            uint8_t path( uint32_t x, uint32_t y ) const {
                return pathMap[y * mapWidth + x];
            }

            bool hasPath( uint32_t x, uint32_t y, uint32_t orientation ) const {
                return (path( x, y ) & (1u << orientation)) != 0;
            }
        };

    }
}

