#include "AutonomousUnitsAI.h"
#include "Player.h"
#include "../common/PlayerMessage.h"

namespace isomap {
    namespace client {
        AutonomousUnitsAI::AutonomousUnitsAI( isomap::client::Player* player )
                : m_player( player ) {

        }

        void AutonomousUnitsAI::update() {
            while ( !m_constructionQueue.empty() && !m_idleConstructionUnits.empty() ) {
                auto* structure = m_player->getStructure( m_constructionQueue.front() );
                if ( structure == nullptr ) {
                    // structure no longer exists, could be removed or destroyed
                    m_constructionQueue.pop();
                    continue;
                }

                // FIXME: needs a bit more intelligence (get unit closest to structure)
                for ( id_t unitId : m_idleConstructionUnits ) {
                    auto* unit = m_player->getUnit( unitId );
                    if ( unit == nullptr ) {
                        // shouldn't happen, but who knows...
                        m_idleConstructionUnits.erase( unitId );
                        // also, this invalidates the iterator, so exit the loop and retry
                        break;
                    }
                    unit->construct( structure );
                    m_constructionQueue.pop();
                    break;
                }
            }
            if ( !m_stuckUnitsQueue.empty() ) {
                Unit* unit = m_player->getUnit( m_stuckUnitsQueue.front() );
                m_stuckUnitsQueue.pop();
                if ( unit != nullptr ) {
                    if ( unit->idle() ) {
                        onUnitIdle( unit );
                    } else {
                        onUnitActive( unit );
                    }
                }
            }
        }

        void AutonomousUnitsAI::onBuildStructureAccepted( id_t structureId ) {
            m_constructionQueue.push( structureId );
        }

        void AutonomousUnitsAI::onUnitIdle( Unit* unit ) {
            printf( "Unit %d became idle\n", unit->id() );
            if ( unit->type()->canConstruct() ) {
                m_idleConstructionUnits.insert( unit->id() );
            }
            if ( unit->type()->canHarvest() ) {
                m_idleHarvesters.insert( unit->id() );
            }
        }

        void AutonomousUnitsAI::onUnitActive( Unit* unit ) {
            printf( "Unit %d became active\n", unit->id() );
            if ( unit->type()->canConstruct() ) {
                m_idleConstructionUnits.erase( unit->id() );
            }
            if ( unit->type()->canHarvest() ) {
                m_idleHarvesters.erase( unit->id() );
            }
        }

        void AutonomousUnitsAI::onUnitStuck( Unit* unit ) {
            m_stuckUnitsQueue.push( unit->id() );
        }
    }
}