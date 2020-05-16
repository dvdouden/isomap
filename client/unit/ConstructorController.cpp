#include "ConstructorController.h"
#include "../Player.h"
#include "../Structure.h"
#include "../Unit.h"

namespace isomap {
    namespace client {
        namespace unit {

            ConstructorController::ConstructorController( Unit* unit ) :
                    Controller( unit ) {

            }

            ConstructorController::~ConstructorController() = default;

            bool ConstructorController::construct( Structure* structure ) {
                // FIXME: code duplication with Controller::construct!
                if ( structure->player() != unit()->player() ) {
                    printf( "[%d] Construct command given to unit for structure %d of other player!\n",
                            unit()->id(),
                            structure->id() );
                    return false;
                }
                if ( structure->constructionCompleted() ) {
                    printf( "[%d] Construct command given to unit for completed structure %d!\n",
                            unit()->id(),
                            structure->id() );
                    return false;
                }
                m_structureQueue.push( structure->id() );
                return true;
            }

            void ConstructorController::onMessage( common::UnitServerMessage::Type msgType ) {
                if ( m_structureQueue.empty() ) {
                    Controller::onMessage( msgType );
                    return;
                }

                switch ( msgType ) {
                    case common::UnitServerMessage::Construct:
                    case common::UnitServerMessage::Stop:
                    case common::UnitServerMessage::Status:
                    case common::UnitServerMessage::MoveTo:
                        break;

                    case common::UnitServerMessage::Done:
                        onDone();
                        break;

                    case common::UnitServerMessage::Abort:
                        onAbort();
                        break;
                }
            }

            void ConstructorController::onDone() {
                switch ( unit()->lastState() ) {
                    case common::Moving: // reached target
                        m_currentStructure = unit()->player()->getStructure( m_currentStructureId );
                        if ( m_currentStructure != nullptr && !Controller::construct( m_currentStructure ) ) {
                            // unable to construct structure, skip!
                            m_currentStructure = nullptr;
                        }
                        break;

                    case common::Constructing:
                        m_currentStructure = nullptr;
                        break;

                    default:
                        // ignore
                        break;
                }
            }

            void ConstructorController::onAbort() {
                switch ( unit()->lastState() ) {
                    case common::Moving:        // failed to reach target, retry
                        printf( "[%d] Construct command given to unit but unable to reach structure, retry!\n",
                                unit()->id() );
                        m_currentStructure = unit()->player()->getStructure( m_currentStructureId );
                        if ( m_currentStructure != nullptr && !Controller::moveTo( m_currentStructure ) ) {
                            printf( "failed again, abort\n" );
                            m_currentStructure = nullptr;
                        }
                        break;

                    case common::Constructing:  // failed to construct target? Retry
                        printf( "[%d] Construct command given to unit but unable to construct structure, retry!\n",
                                unit()->id() );
                        m_currentStructure = unit()->player()->getStructure( m_currentStructureId );
                        if ( m_currentStructure != nullptr && !Controller::construct( m_currentStructure ) ) {
                            printf( "failed again, abort\n" );
                            m_currentStructure = nullptr;
                        }
                        break;

                    default:
                        // ignore
                        break;
                }
            }

            void ConstructorController::update() {
                if ( m_currentStructure == nullptr && !m_structureQueue.empty() ) {
                    m_currentStructureId = m_structureQueue.front();
                    m_structureQueue.pop();
                    m_currentStructure = unit()->player()->getStructure( m_currentStructureId );
                    if ( m_currentStructure == nullptr ) {
                        printf( "[%d] Construct command given to unit but non-existent structure %d!\n",
                                unit()->id(),
                                m_currentStructureId );
                    } else if ( m_currentStructure->isAdjacentTo( unit()->tileX(), unit()->tileY() ) ) {
                        if ( !Controller::construct( m_currentStructure ) ) {
                            printf( "[%d] Construct command given to unit but unable to construct structure %d!\n",
                                    unit()->id(),
                                    m_currentStructureId );
                            m_currentStructure = nullptr;
                        }
                    } else {
                        if ( !Controller::moveTo( m_currentStructure ) ) {
                            printf( "[%d] Construct command given to unit but unable to reach structure %d!\n",
                                    unit()->id(),
                                    m_currentStructureId );
                            m_currentStructure = nullptr;
                        }
                    }
                }
            }


        }
    }
}