#include <map>

#include "StructureType.h"
#include "TerrainData.h"
#include "UnitType.h"

namespace isomap {
    namespace common {
        typedef std::map<id_t, StructureType*> StructureTypes;

        static StructureTypes s_structureTypes;

        StructureType* StructureType::get( id_t id ) {
            auto search = s_structureTypes.find( id );
            if ( search == s_structureTypes.end() ) {
                return nullptr;
            }
            return search->second;
        }

        void StructureType::load() {
            s_structureTypes[1] = new StructureType( 1,
                                                     "constructionyard",
                                                     new FootPrint( 3, 3,
                                                                    {3, 3, 3,
                                                                     3, 3, 3,
                                                                     2, 2, 2} ) );

            s_structureTypes[2] = new StructureType( 2,
                                                     "powerplant",
                                                     new FootPrint( 2, 3,
                                                                    {3, 0,
                                                                     3, 3,
                                                                     2, 2} ) );

            s_structureTypes[3] = new StructureType( 3,
                                                     "barracks",
                                                     new FootPrint( 2, 3,
                                                                    {3, 3,
                                                                     3, 3,
                                                                     2, 2} ) );

            s_structureTypes[4] = new StructureType( 4,
                                                     "refinery",
                                                     new FootPrint( 3, 4,
                                                                    {0, 3, 0,
                                                                     3, 3, 3,
                                                                     3, 3, 3,
                                                                     2, occupancy::bitSpawnPoint | 2u, 2} ),
                                                     2 );

            s_structureTypes[5] = new StructureType( 5,
                                                     "turret",
                                                     new FootPrint( 1, 1,
                                                                    {3} ) );

        }

        void StructureType::clear() {
            for ( auto structureType : s_structureTypes ) {
                delete structureType.second;
            }
            s_structureTypes.clear();
        }

        StructureType::StructureType( isomap::id_t id, std::string name, FootPrint* footPrint, id_t spawnUnitId ) :
                m_id( id ),
                m_name( std::move( name ) ),
                m_spawnUnitId( spawnUnitId ) {
            m_footPrint[0] = footPrint;
            m_footPrint[1] = m_footPrint[0]->rotate();
            m_footPrint[2] = m_footPrint[1]->rotate();
            m_footPrint[3] = m_footPrint[2]->rotate();
        }

        StructureType::~StructureType() {
            delete m_footPrint[0];
            delete m_footPrint[1];
            delete m_footPrint[2];
            delete m_footPrint[3];
        }

        UnitType* StructureType::includedUnitType() const {
            return UnitType::get( m_spawnUnitId );
        }
    }
}