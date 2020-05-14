#include "AutonomousUnitsAI.h"
#include "Player.h"
#include "../common/PlayerMessage.h"

namespace isomap {
    namespace client {
        AutonomousUnitsAI::AutonomousUnitsAI( isomap::client::Player* player )
                : m_player( player ) {

        }

        void AutonomousUnitsAI::update() {

        }

        void AutonomousUnitsAI::onBuildStructureAccepted( common::PlayerServerMessage* msg ) {
            auto* str = m_player->getStructure( msg->structureData()->id );
            // FIXME: use something a bit more intelligent
            for ( auto& unit : m_player->units() ) {
                if ( unit.second->type()->canConstruct() ) {
                    unit.second->construct( str );
                    break;
                }
            }
        }
    }
}