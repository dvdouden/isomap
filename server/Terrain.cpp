#include "Structure.h"
#include "Terrain.h"
#include "Unit.h"
#include "../common/TerrainMessage.h"

namespace isomap {
    namespace server {

        Terrain::Terrain( uint32_t width, uint32_t height ) :
                m_width( width ),
                m_height( height ),
                m_data( width, height ) {
            m_structures = new std::vector<Structure*>[(m_width / m_chunkSize) * (m_height / m_chunkSize)];
            m_units = new std::vector<Unit*>[(m_width / m_chunkSize) * (m_height / m_chunkSize)];
        }

        Terrain::~Terrain() {
            delete[] m_structures;
            delete[] m_units;
        }

        void Terrain::init() {
            m_data.updatePathMap();
        }

        common::TerrainMessage* Terrain::updateMessage( const std::vector<uint32_t>& ids ) const {
            std::vector<common::TerrainMessage::Cell> cells;
            cells.reserve( ids.size() );
            for ( auto id : ids ) {
                cells.push_back( {id, m_data.heightMap[id], m_data.slopeMap[id], m_data.oreMap[id]} );
            }
            return common::TerrainMessage::updateMsg( cells );
        }

        common::TerrainMessage* Terrain::uncoverAll() const {
            std::vector<common::TerrainMessage::Cell> cells;
            cells.reserve( m_width * m_height );
            for ( uint32_t id = 0; id < m_width * m_height; ++id ) {
                cells.push_back( {id, m_data.heightMap[id], m_data.slopeMap[id], m_data.oreMap[id]} );
            }
            return common::TerrainMessage::updateMsg( cells );
        }

        void Terrain::addStructure( Structure* structure ) {
            for ( uint32_t chunk : getChunks( structure ) ) {
                m_structures[chunk].push_back( structure );
            }
            occupy( structure->x(), structure->y(), structure->footPrint() );
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
            vacate( structure->x(), structure->y(), structure->footPrint() );
        }

        void Terrain::addUnit( Unit* unit ) {
            m_units[getChunk( unit->tileX(), unit->tileY() )].push_back( unit );
        }

        void Terrain::removeUnit( Unit* unit ) {
            uint32_t chunk = getChunk( unit->tileX(), unit->tileY() );
            for ( auto it = m_units[chunk].begin(); it != m_units[chunk].end(); ++it ) {
                if ( (*it) == unit ) {
                    m_units[chunk].erase( it );
                    break;
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

        uint32_t Terrain::getChunk( uint32_t x, uint32_t y ) {
            uint32_t chunkX = x / m_chunkSize;
            uint32_t chunkY = y / m_chunkSize;
            return chunkY * (m_width / m_chunkSize) + chunkX;
        }

        Structure* Terrain::getStructureAt( uint32_t x, uint32_t y ) {
            if ( (m_data.occupancyMap[y * m_width + x] & 0b0000'0001u) == 0 ) {
                return nullptr;
            }
            uint32_t chunk = getChunk( x, y );
            for ( Structure* structure : m_structures[chunk] ) {
                if ( structure->occupies( x, y ) ) {
                    return structure;
                }
            }
            return nullptr;
        }

        Unit* Terrain::getUnitAt( uint32_t x, uint32_t y ) {
            uint32_t chunk = getChunk( x, y );
            for ( Unit* unit : m_units[chunk] ) {
                if ( unit->tileX() == x && unit->tileY() == y ) {
                    return unit;
                }
            }
            return nullptr;
        }

        void Terrain::occupy( uint32_t worldX, uint32_t worldY, const common::FootPrint* footPrint ) {
            // occupy area
            for ( uint32_t y = 0; y < footPrint->height(); ++y ) {
                for ( uint32_t x = 0; x < footPrint->width(); ++x ) {
                    if ( footPrint->get( x, y ) != 0 ) {
                        m_data.occupancyMap[(y + worldY) * m_width + (x + worldX)] |= 0b0000'0001u;
                    }
                }
            }
            m_data.updatePathMap( worldX, worldY, footPrint->width(), footPrint->height() );
        }

        void Terrain::vacate( uint32_t worldX, uint32_t worldY, const common::FootPrint* footPrint ) {
            // vacate area
            for ( uint32_t y = 0; y < footPrint->height(); ++y ) {
                for ( uint32_t x = 0; x < footPrint->width(); ++x ) {
                    if ( footPrint->get( x, y ) != 0 ) {
                        m_data.occupancyMap[(y + worldY) * m_width + (x + worldX)] &= ~0b0000'0001u;
                    }
                }
            }
            m_data.updatePathMap( worldX, worldY, footPrint->width(), footPrint->height() );
        }

    }
}
