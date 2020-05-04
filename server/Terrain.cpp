#include "Structure.h"
#include "Terrain.h"
#include "../common/TerrainMessage.h"

namespace isomap {
    namespace server {

        Terrain::Terrain( uint32_t width, uint32_t height ) :
                m_width( width ),
                m_height( height ) {
            m_heightMap = new uint8_t[m_width * m_height];
            m_slopeMap = new uint8_t[m_width * m_height];
            m_oreMap = new uint8_t[m_width * m_height];
            m_structures = new std::vector<Structure*>[(m_width / m_chunkSize) * (m_height / m_chunkSize)];
        }

        Terrain::~Terrain() {
            delete[] m_heightMap;
            delete[] m_slopeMap;
            delete[] m_oreMap;
            delete[] m_structures;
        }

        common::TerrainMessage* Terrain::createMessage() const {
            return common::TerrainMessage::createMsg( m_width, m_height );
        }

        common::TerrainMessage* Terrain::updateMessage( const std::vector<uint32_t>& ids ) const {
            std::vector<common::TerrainMessage::Cell> cells;
            cells.reserve( ids.size() );
            for ( auto id : ids ) {
                cells.push_back( {id, m_heightMap[id], m_slopeMap[id], m_oreMap[id]} );
            }
            return common::TerrainMessage::updateMsg( cells );
        }

        common::TerrainMessage* Terrain::uncoverAll() const {
            std::vector<common::TerrainMessage::Cell> cells;
            cells.reserve( m_width * m_height );
            for ( uint32_t id = 0; id < m_width * m_height; ++id ) {
                cells.push_back( {id, m_heightMap[id], m_slopeMap[id], m_oreMap[id]} );
            }
            return common::TerrainMessage::updateMsg( cells );
        }

        void Terrain::addStructure( Structure* structure ) {
            for ( uint32_t chunk : getChunks( structure ) ) {
                m_structures[chunk].push_back( structure );
            }
        }

        void Terrain::removeStructure( Structure* structure ) {
            for ( uint32_t chunk : getChunks( structure ) ) {
                for ( auto it = m_structures[chunk].begin(); it != m_structures[chunk].end(); ++it ) {
                    if ( (*it) == structure ) {
                        m_structures[chunk].erase( it );
                        break;
                    }
                }
            }
        }

        std::vector<uint32_t> Terrain::getChunks( Structure* structure ) {
            std::vector<uint32_t> chunks;
            uint32_t x1 = structure->x() / m_chunkSize;
            uint32_t y1 = structure->y() / m_chunkSize;
            uint32_t x2 = (structure->x() + structure->footPrint()->width() - 1) / m_chunkSize;
            uint32_t y2 = (structure->y() + structure->footPrint()->height() - 1) / m_chunkSize;
            for ( uint32_t y = y1; y <= y2; ++y ) {
                for ( uint32_t x = x1; x <= x2; ++x ) {
                    chunks.push_back( y * (m_width / m_chunkSize) + x );
                }
            }
            return std::move( chunks );
        }

        Structure* Terrain::getStructureAt( uint32_t x, uint32_t y ) {
            uint32_t chunkX = x / m_chunkSize;
            uint32_t chunkY = y / m_chunkSize;
            uint32_t chunk = chunkY * (m_width / m_chunkSize) + chunkX;
            for ( Structure* structure : m_structures[chunk] ) {
                if ( structure->occupies( x, y ) ) {
                    return structure;
                }
            }
            return nullptr;
        }
    }
}
