#include <map>
#include "UnitType.h"

namespace isomap {
    namespace common {
        typedef std::map<id_t, UnitType*> UnitTypes;

        static UnitTypes s_unitTypes;

        UnitType* UnitType::get( id_t id ) {
            auto search = s_unitTypes.find( id );
            if ( search == s_unitTypes.end() ) {
                return nullptr;
            }
            return search->second;
        }

        void UnitType::load() {
            s_unitTypes[1] = new UnitType( 1, "harvester" );
        }

        UnitType::UnitType( isomap::id_t id, std::string name ) :
                m_id( id ),
                m_name( std::move( name ) ) {

        }
    }
}