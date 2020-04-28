#include <map>
#include <cstring>
#include "StructureType.h"

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
            s_structureTypes[1] = new StructureType( 1, 3, 3,
                                                     {2, 2, 2,
                                                      2, 2, 2,
                                                      1, 1, 1} );

            s_structureTypes[2] = new StructureType( 2, 2, 3,
                                                     {2, 0,
                                                      2, 2,
                                                      1, 1} );


            s_structureTypes[3] = new StructureType( 3, 2, 3,
                                                     {2, 2,
                                                      2, 2,
                                                      1, 1} );


            s_structureTypes[4] = new StructureType( 4, 3, 4,
                                                     {0, 2, 0,
                                                      2, 2, 2,
                                                      2, 2, 2,
                                                      1, 1, 1} );

        }

        StructureType::StructureType( isomap::id_t id, uint32_t width, uint32_t height,
                                      std::initializer_list<uint8_t> footPrint ) :
                m_id( id ),
                m_width( width ),
                m_height( height ) {
            m_footPrint = new uint8_t[m_width * m_height];
            std::copy( footPrint.begin(), footPrint.end(), m_footPrint );
        }
    }
}