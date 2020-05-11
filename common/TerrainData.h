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

        namespace occupancy {
            const uint8_t bitObstructed = 0b0000'0001u;
            const uint8_t bitConstructed = 0b0000'0010u;
            const uint8_t bitReserved = 0b0000'0100u;
        }

        // common data structure and methods
        struct TerrainData {
            uint32_t mapWidth = 0;
            uint32_t mapHeight = 0;

            uint8_t* heightMap = nullptr;
            uint8_t* slopeMap = nullptr;
            uint8_t* oreMap = nullptr;
            uint8_t* occupancyMap = nullptr;
            uint8_t* pathMap = nullptr;

            TerrainData( uint32_t width, uint32_t heigth );

            TerrainData( const TerrainData& ) = delete;

            ~TerrainData();

            const TerrainData& operator=( const TerrainData& ) = delete;


            void occupy( uint32_t x, uint32_t y, const common::FootPrint* footPrint );

            void vacate( uint32_t x, uint32_t y, const common::FootPrint* footPrint );

            void reserve( uint32_t x, uint32_t y, const common::FootPrint* footPrint );

            void unreserve( uint32_t x, uint32_t y, const common::FootPrint* footPrint );

            void updatePathMap();

            void updatePathMap( uint32_t x, uint32_t y, uint32_t width, uint32_t height );
        };

    }
}

