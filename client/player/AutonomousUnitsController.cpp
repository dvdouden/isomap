#include "AutonomousUnitsController.h"
#include "../unit/ConstructorController.h"
#include "../Player.h"

namespace isomap {
    namespace client {
        namespace player {
            AutonomousUnitsController::AutonomousUnitsController( isomap::client::Player* player )
                    : Controller( player ) {
                m_constructionController = std::make_unique<player::ConstructionController>();
            }

            void AutonomousUnitsController::update() {
                if ( m_constructionController ) {
                    m_constructionController->update();
                }
            }

            void AutonomousUnitsController::onUnitCreated( Unit* unit ) {
                if ( unit->type()->canConstruct() ) {
                    unit->setController( new unit::ConstructorController( unit ) );
                    if ( m_constructionController ) {
                        m_constructionController->addConstructor( unit );
                    }
                } else {
                    unit->setController( new unit::Controller( unit ) );
                }
            }

            void AutonomousUnitsController::onUnitDestroyed( Unit* unit ) {
                if ( unit->type()->canConstruct() && m_constructionController ) {
                    m_constructionController->removeConstructor( unit );
                }
            }

            void AutonomousUnitsController::onStructureCreated( Structure* structure ) {
                structure->setController( new structure::Controller( structure ) );
                if ( m_constructionController ) {
                    m_constructionController->addStructure( structure );
                }
            }

            void AutonomousUnitsController::onConstructionComplete( Structure* structure ) {
                if ( m_constructionController ) {
                    m_constructionController->removeStructure( structure );
                }
            }

            void AutonomousUnitsController::onStructureDestroyed( Structure* structure ) {
                if ( m_constructionController ) {
                    m_constructionController->removeStructure( structure );
                }
            }

            void AutonomousUnitsController::dump() const {
                printf( "Controller:\n" );
                if ( m_constructionController ) {
                    m_constructionController->dump();
                }

            }

            void AutonomousUnitsController::onUnableToConstruct( Structure* structure, Unit* unit ) {
                if ( structure != nullptr ) {
                    if ( m_constructionController ) {
                        m_constructionController->unableToConstruct( structure, unit );
                    }
                }
            }

        } // namespace end
    }
}