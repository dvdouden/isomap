#pragma once

#include "types.h"

namespace isomap {
    namespace common {

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

