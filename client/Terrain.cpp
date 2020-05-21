#include <cstring>

#include "Structure.h"
#include "Terrain.h"
#include "Unit.h"
#include "../common/TerrainMessage.h"

namespace isomap {
    namespace client {


        Terrain::Terrain( uint32_t width, uint32_t height ) :
                m_width( width ),
                m_height( height ),
                m_data( width, height ) {
            m_fogMap.resize( m_width * m_height, 0 );
            m_fogUpdateMapWidth = (m_width + (m_fogUpdateMapScale - 1)) / m_fogUpdateMapScale;
            m_fogUpdateMapHeight = (m_height + (m_fogUpdateMapScale - 1)) / m_fogUpdateMapScale;
            m_fogUpdateMap.resize( m_fogUpdateMapWidth * m_fogUpdateMapHeight, 0 );
            m_structures.resize( (m_width / m_chunkSize) * (m_height / m_chunkSize) );
            m_units.resize( (m_width / m_chunkSize) * (m_height / m_chunkSize) );
        }

        void Terrain::processMessage( isomap::common::TerrainMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            switch ( msg->type() ) {
                case common::TerrainMessage::Update:
                    for ( const auto& cell : msg->cells() ) {
                        m_data.heightMap[cell.id] = cell.height;
                        m_data.oreMap[cell.id] = cell.ore;
                        m_data.slopeMap[cell.id] = cell.slope;
                        m_fogMap[cell.id] = 255;
                        uint32_t x = cell.id % m_width;
                        uint32_t y = cell.id / m_height;
                        uint32_t fogX = x / m_fogUpdateMapScale;
                        uint32_t fogY = y / m_fogUpdateMapScale;
                        m_fogUpdateMap[fogY * m_fogUpdateMapWidth + fogX] = 1;
                        m_data.updatePathMap( x, y, 1, 1 );
                    }
                    break;

                default:
                    break;
            }
        }

        void Terrain::updateFog() {
            if ( !m_updateFog ) {
                return;
            }
            uint32_t checks = 0;
            for ( uint32_t fogY = 0; fogY < m_fogUpdateMapHeight; ++fogY ) {
                for ( uint32_t fogX = 0; fogX < m_fogUpdateMapWidth; ++fogX ) {
                    if ( m_fogUpdateMap[fogY * m_fogUpdateMapWidth + fogX] == 0 ) {
                        continue;
                    }
                    ++checks;
                    uint32_t cnt = 0;
                    // could probably make this a _lot_ faster by splitting the actual map into chunks
                    for ( uint32_t y = fogY * m_fogUpdateMapScale;
                          y < ((fogY + 1) * m_fogUpdateMapScale) && y < m_height; ++y ) {
                        for ( uint32_t x = fogX * m_fogUpdateMapScale;
                              x < ((fogX + 1) * m_fogUpdateMapScale) && x < m_width; ++x ) {
                            if ( m_fogMap[y * m_width + x] > 1 ) {
                                m_fogMap[y * m_width + x]--;
                                ++cnt;
                            }
                        }
                    }
                    if ( cnt == 0 ) {
                        m_fogUpdateMap[fogY * m_fogUpdateMapWidth + fogX] = 0;
                    }
                }
            }
        }

        void Terrain::occupy( uint32_t worldX, uint32_t worldY, const common::FootPrint* footPrint ) {
            m_data.occupy( worldX, worldY, footPrint );
        }

        void Terrain::vacate( uint32_t worldX, uint32_t worldY, const common::FootPrint* footPrint ) {
            m_data.vacate( worldX, worldY, footPrint );
        }

        void Terrain::reserve( uint32_t worldX, uint32_t worldY, const common::FootPrint* footPrint ) {
            m_data.reserve( worldX, worldY, footPrint );
        }

        void Terrain::unreserve( uint32_t worldX, uint32_t worldY, const common::FootPrint* footPrint ) {
            m_data.unreserve( worldX, worldY, footPrint );
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

        Structure* Terrain::getObstructingStructureAt( uint32_t x, uint32_t y ) const {
            if ( (m_data.occupancyMap[y * m_width + x] & common::occupancy::bitObstructed) == 0 ) {
                return nullptr;
            }
            return getStructureAt( x, y );
        }

        Structure* Terrain::getConstructedStructureAt( uint32_t x, uint32_t y ) const {
            if ( (m_data.occupancyMap[y * m_width + x] & common::occupancy::bitConstructed) == 0 ) {
                return nullptr;
            }
            return getStructureAt( x, y );
        }

        Structure* Terrain::getStructureAt( uint32_t x, uint32_t y ) const {
            uint32_t chunk = getChunk( x, y );
            for ( Structure* structure : m_structures[chunk] ) {
                if ( structure->occupies( x, y ) ) {
                    return structure;
                }
            }
            return nullptr;
        }

        void Terrain::addUnit( Unit* unit ) {
            //printf( "Add unit %d\n", unit->id() );
            addUnitToChunk( unit, getChunk( unit->tileX(), unit->tileY() ) );
        }

        void Terrain::removeUnit( Unit* unit ) {
            //printf( "Remove unit %d\n", unit->id() );
            removeUnitFromChunk( unit, getChunk( unit->tileX(), unit->tileY() ) );
        }

        void Terrain::updateUnit( Unit* unit, uint32_t oldX, uint32_t oldY ) {
            //printf( "Update unit %d (old %d, %d)\n", unit->id(), oldX, oldY );
            uint32_t oldChunk = getChunk( oldX, oldY );
            uint32_t chunk = getChunk( unit->tileX(), unit->tileY() );
            if ( oldChunk != chunk ) {
                removeUnitFromChunk( unit, oldChunk );
                addUnitToChunk( unit, chunk );
            }
        }

        Unit* Terrain::getUnitAt( uint32_t x, uint32_t y ) const {
            uint32_t chunk = getChunk( x, y );
            for ( Unit* unit : m_units[chunk] ) {
                if ( unit->tileX() == x && unit->tileY() == y ) {
                    return unit;
                }
            }
            return nullptr;
        }

        std::vector<uint32_t> Terrain::getChunks( Structure* structure ) const {
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

        uint32_t Terrain::getChunk( uint32_t x, uint32_t y ) const {
            uint32_t chunkX = x / m_chunkSize;
            uint32_t chunkY = y / m_chunkSize;
            return chunkY * (m_width / m_chunkSize) + chunkX;
        }

        void Terrain::removeUnitFromChunk( Unit* unit, uint32_t chunk ) {
            //printf( "Remove unit %d from chunk %d\n", unit->id(), chunk );
            for ( auto it = m_units[chunk].begin(); it != m_units[chunk].end(); ++it ) {
                if ( (*it) == unit ) {
                    //printf( "Removed %d\n", (*it)->id() );
                    m_units[chunk].erase( it );
                    break;
                }
            }
        }

        void Terrain::addUnitToChunk( Unit* unit, uint32_t chunk ) {
            //printf( "Add unit %d to chunk %d\n", unit->id(), chunk );
            m_units[chunk].push_back( unit );
        }

        void Terrain::dump() const {
            printf( "Terrain:\n" );
            printf( "\tWidth: %d\n", m_width );
            printf( "\tHeight: %d\n", m_height );
            for ( int i = 0; i < (m_width / m_chunkSize) * (m_height / m_chunkSize); ++i ) {
                if ( !m_structures[i].empty() || !m_units[i].empty() ) {
                    printf( "\tChunk %d (%d, %d):\n", i, (i % (m_width / m_chunkSize)) * m_chunkSize,
                            (i / (m_width / m_chunkSize)) * m_chunkSize );
                    if ( !m_structures[i].empty() ) {
                        printf( "\t\tStructures (%lu):\n", m_structures[i].size() );
                        for ( auto* s : m_structures[i] ) {
                            printf( "\t\t\t%d (%s): %d, %d\n", s->id(), s->type()->name().c_str(), s->x(), s->y() );
                        }
                    }
                    if ( !m_units[i].empty() ) {
                        printf( "\t\tUnits (%lu):\n", m_units[i].size() );
                        for ( auto* u : m_units[i] ) {
                            printf( "\t\t\t%d (%s): %d, %d\n", u->id(), u->type()->name().c_str(), u->tileX(),
                                    u->tileY() );
                        }
                    }
                }
            }

        }
    }

}