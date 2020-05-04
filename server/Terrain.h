#pragma once

#include <vector>
#include "../common/types.h"

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
                return m_heightMap;
            }

            uint8_t* slopeMap() const {
                return m_slopeMap;
            }

            uint8_t* oreMap() const {
                return m_oreMap;
            }

            void addStructure( Structure* structure );

            void removeStructure( Structure* structure );

            common::TerrainMessage* createMessage() const;

            common::TerrainMessage* updateMessage( const std::vector<uint32_t>& cells ) const;

            common::TerrainMessage* uncoverAll() const;

            Structure* getStructureAt( uint32_t x, uint32_t y );

        private:
            uint32_t m_width;
            uint32_t m_height;

            uint8_t* m_heightMap = nullptr;
            uint8_t* m_slopeMap = nullptr;
            uint8_t* m_oreMap = nullptr;

            uint32_t m_chunkSize = 16;
            std::vector<Structure*>* m_structures;

            std::vector<uint32_t> getChunks( Structure* pStructure );
        };
    }
}
