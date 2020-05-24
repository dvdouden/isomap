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
            s_unitTypes[1]->setPayload( 500, 500 );
            s_unitTypes[1]->setDockStructureType( 1 );
            s_unitTypes[2] = new UnitType( 2, "harvester", false, true );
            s_unitTypes[2]->setPayload( 0, 700 );
            s_unitTypes[2]->setDockStructureType( 4 );
            s_unitTypes[3] = new UnitType( 3, "scout", false, false );
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

        void UnitType::setDockStructureType( id_t dockStructureTypeId ) {
            m_dockStructureId = dockStructureTypeId;
        }
    }
}