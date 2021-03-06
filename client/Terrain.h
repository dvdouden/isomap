#pragma once

#include <memory>

#include "../common/FootPrint.h"
#include "../common/TerrainData.h"
#include "types.h"
#include "terrain/Renderer.h"

namespace isomap {
    namespace client {
        class Terrain {
        public:

            Terrain( uint32_t width, uint32_t height );

            ~Terrain() = default;

            Terrain( const Terrain& ) = delete;

            const Terrain& operator=( const Terrain& ) = delete;

            void setRenderer( terrain::Renderer* renderer ) {
                m_renderer.reset( renderer );
            }

            terrain::Renderer* renderer() const {
                return m_renderer.get();
            }

            void processMessage( common::TerrainMessage* msg );

            uint32_t width() const {
                return m_width;
            }

            uint32_t height() const {
                return m_height;
            }

            uint8_t* heightMap() const {
                return m_data.heightMap;
            }

            uint8_t* slopeMap() const {
                return m_data.slopeMap;
            }

            uint8_t* oreMap() const {
                return m_data.oreMap;
            }

            uint8_t ore( uint32_t x, uint32_t y ) const {
                return m_data.oreMap[y * m_data.mapWidth + x];
            }

            uint8_t* occupancyMap() const {
                return m_data.occupancyMap;
            }

            uint8_t occupancy( uint32_t x, uint32_t y ) const {
                return m_data.occupancyMap[y * m_data.mapWidth + x];
            }

            uint8_t* pathMap() const {
                return m_data.pathMap;
            }

            const std::vector<uint8_t>& fogMap() const {
                return m_fogMap;
            }

            void updateFog();

            bool isVisible( uint32_t x, uint32_t y ) const {
                return m_fogMap[y * m_width + x] > 1;
            }

            uint8_t occupied( uint32_t x, uint32_t y ) const {
                return m_data.occupancyMap[y * m_width + x];
            }

            void occupy( uint32_t worldX, uint32_t worldY, const common::FootPrint* footPrint );

            void vacate( uint32_t worldX, uint32_t worldY, const common::FootPrint* footPrint );

            void reserve( uint32_t worldX, uint32_t worldY, const common::FootPrint* footPrint );

            void unreserve( uint32_t worldX, uint32_t worldY, const common::FootPrint* footPrint );

            void addStructure( Structure* structure );

            void removeStructure( Structure* structure );

            Structure* getObstructingStructureAt( uint32_t x, uint32_t y ) const;

            Structure* getConstructedStructureAt( uint32_t x, uint32_t y ) const;

            Structure* getStructureAt( uint32_t x, uint32_t y ) const;

            void addUnit( Unit* unit );

            void removeUnit( Unit* unit );

            void updateUnit( Unit* unit, uint32_t oldX, uint32_t oldY );

            Unit* getUnitAt( uint32_t x, uint32_t y ) const;

            uint8_t safeCorner( int32_t x, int32_t y, uint32_t c ) const {
                return m_data.safeCorner( x, y, c );
            }

            uint8_t corner( uint32_t x, uint32_t y, uint32_t c ) const {
                return m_data.corner( x, y, c );
            }

            common::TerrainData& data() {
                return m_data;
            }

            void dump() const;

        private:


            std::vector<uint32_t> getChunks( Structure* structure ) const;

            uint32_t getChunk( uint32_t x, uint32_t y ) const;

            void removeUnitFromChunk( Unit* unit, uint32_t chunk );

            void addUnitToChunk( Unit* unit, uint32_t chunk );

            uint32_t m_width = 0;
            uint32_t m_height = 0;
            uint32_t m_fogUpdateMapScale = 64;
            uint32_t m_fogUpdateMapWidth = 0;
            uint32_t m_fogUpdateMapHeight = 0;

            bool m_updateFog = false;

            common::TerrainData m_data;

            std::vector<uint8_t> m_fogMap;
            std::vector<uint8_t> m_fogUpdateMap;

            // FIXME: shared code with server!
            uint32_t m_chunkSize = 16;
            std::vector<std::vector<Structure*>> m_structures;
            std::vector<std::vector<Unit*>> m_units;

            std::unique_ptr<terrain::Renderer> m_renderer;
        };
    }
}




