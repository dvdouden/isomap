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
        }

        Terrain::~Terrain() {
            delete[] m_heightMap;
            delete[] m_slopeMap;
            delete[] m_oreMap;
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
    }
}
