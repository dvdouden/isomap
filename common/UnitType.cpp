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
            s_unitTypes[1] = new UnitType( 1, "constructor", true, false );
            s_unitTypes[2] = new UnitType( 2, "harvester", false, true );
        }

        void UnitType::clear() {
            for ( auto unitType : s_unitTypes ) {
                delete unitType.second;
            }
            s_unitTypes.clear();
        }

        UnitType::UnitType( isomap::id_t id, std::string name, bool canConstruct, bool canHarvest ) :
                m_id( id ),
                m_name( std::move( name ) ),
                m_canConstruct( canConstruct ),
                m_canHarvest( canHarvest ) {

        }
    }
}