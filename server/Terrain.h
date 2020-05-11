#pragma once

#include <vector>
#include "../common/types.h"
#include "../common/TerrainData.h"

namespace isomap {
    namespace server {

        class Terrain {
        public:
            Terrain( uint32_t width, uint32_t height );

            ~Terrain();

            Terrain( const Terrain& ) = delete;

            const Terrain& operator=( const Terrain& ) = delete;

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

            uint8_t* occupancyMap() const {
                return m_data.occupancyMap;
            }

            uint8_t* pathMap() const {
                return m_data.pathMap;
            }

            void init();

            void addStructure( Structure* structure );

            void removeStructure( Structure* structure );

            void addUnit( Unit* unit );

            void removeUnit( Unit* unit );

            void updateUnit( Unit* unit, uint32_t oldX, uint32_t oldY );

            common::TerrainMessage* updateMessage( const std::vector<uint32_t>& cells ) const;

            common::TerrainMessage* uncoverAll() const;

            Structure* getStructureAt( uint32_t x, uint32_t y );

            Unit* getUnitAt( uint32_t x, uint32_t y );

            void occupy( uint32_t x, uint32_t y, const common::FootPrint* footPrint );

            void vacate( uint32_t x, uint32_t y, const common::FootPrint* footPrint );

        private:
            void removeUnitFromChunk( Unit* unit, uint32_t chunk );

            uint32_t m_width;
            uint32_t m_height;

            common::TerrainData m_data;

            uint32_t m_chunkSize = 16;
            std::vector<Structure*>* m_structures;
            std::vector<Unit*>* m_units;

            std::vector<uint32_t> getChunks( Structure* pStructure );

            uint32_t getChunk( uint32_t x, uint32_t y );
        };
    }
}
