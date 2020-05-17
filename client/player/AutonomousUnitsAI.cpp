#include "AutonomousUnitsAI.h"
#include "../unit/ConstructorController.h"
#include "../Player.h"

namespace isomap {
    namespace client {
        AutonomousUnitsAI::AutonomousUnitsAI( isomap::client::Player* player )
                : Controller( player ) {
            m_constructionController = std::make_unique<player::ConstructionController>();
        }

        void AutonomousUnitsAI::update() {
            if ( m_constructionController ) {
                m_constructionController->update();
            }
        }

        void AutonomousUnitsAI::onUnitCreated( Unit* unit ) {
            if ( unit->type()->canConstruct() ) {
                unit->setController( new unit::ConstructorController( unit ) );
                if ( m_constructionController ) {
                    m_constructionController->addConstructor( unit );
                }
            }
        }

        void AutonomousUnitsAI::onUnitDestroyed( Unit* unit ) {
            if ( unit->type()->canConstruct() && m_constructionController ) {
                m_constructionController->removeConstructor( unit );
            }
        }

        void AutonomousUnitsAI::onStructureCreated( Structure* structure ) {
            structure->setController( new structure::Controller( structure ) );
            if ( m_constructionController ) {
                m_constructionController->addStructure( structure );
            }
        }

        void AutonomousUnitsAI::onConstructionComplete( Structure* structure ) {
            if ( m_constructionController ) {
                m_constructionController->removeStructure( structure );
            }
        }

        void AutonomousUnitsAI::onStructureDestroyed( Structure* structure ) {
            if ( m_constructionController ) {
                m_constructionController->removeStructure( structure );
            }
        }

        void AutonomousUnitsAI::dump() const {
            printf( "Controller:\n" );
            if ( m_constructionController ) {
                m_constructionController->dump();
            }

        }

        void AutonomousUnitsAI::onUnableToConstruct( Structure* structure, Unit* unit ) {
            if ( structure != nullptr ) {
                if ( m_constructionController ) {
                    m_constructionController->unableToConstruct( structure, unit );
                }
            }
        }

    }
}