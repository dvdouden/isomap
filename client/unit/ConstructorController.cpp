#include "ConstructorController.h"
#include "../Player.h"
#include "../Structure.h"
#include "../Unit.h"
#include "WorkProvider.h"

namespace isomap {
    namespace client {
        namespace unit {

            ConstructorController::ConstructorController( Unit* unit ) :
                    Controller( unit ) {

            }

            ConstructorController::~ConstructorController() = default;

            bool ConstructorController::construct( Structure* structure ) {
                if ( unit()->isAdjacentTo( structure ) ) {
                    if ( Controller::construct( structure ) ) {
                        setStructure( structure );
                        return true;
                    }
                } else {
                    if ( Controller::moveTo( structure ) ) {
                        setStructure( structure );
                        return true;
                    }
                }
                return false;
            }

            void ConstructorController::onMessage( common::UnitServerMessage::Type msgType ) {
                if ( m_currentStructure == nullptr ) {
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
                        //printf( "Reached target, start constructing\n" );
                        construct();
                        break;

                    case common::Constructing:
                        //printf( "Construction complete\n" );
                        m_currentStructure = nullptr;
                        if ( workProvider() != nullptr ) {
                            workProvider()->unitAvailable( unit() );
                        }
                        break;

                    default:
                        // ignore
                        break;
                }
            }

            void ConstructorController::onAbort() {
                switch ( unit()->lastState() ) {
                    case common::Moving:
                        // failed to reach target, retry
                        printf( "[%d] Construct command given to unit but unable to reach structure, retry!\n",
                                unit()->id() );
                        moveTo();
                        break;

                    case common::Constructing:
                        // failed to construct target? Retry
                        printf( "[%d] Construct command given to unit but unable to construct structure, retry!\n",
                                unit()->id() );
                        construct();
                        break;

                    default:
                        // ignore
                        break;
                }
            }

            void ConstructorController::update() {
                if ( m_currentStructure == nullptr ) {
                    Controller::update();
                }
            }

            void ConstructorController::moveTo() {
                m_currentStructure = unit()->player()->getStructure( m_currentStructureId );
                if ( m_currentStructure == nullptr || !Controller::moveTo( m_currentStructure ) ) {
                    printf( "[%d] Construct command given to unit but unable to reach structure %d!\n",
                            unit()->id(),
                            m_currentStructureId );
                    fail();
                }
            }

            void ConstructorController::construct() {
                m_currentStructure = unit()->player()->getStructure( m_currentStructureId );
                if ( m_currentStructure == nullptr || !Controller::construct( m_currentStructure ) ) {
                    printf( "[%d] Construct command given to unit but unable to construct structure %d!\n",
                            unit()->id(),
                            m_currentStructureId );
                    fail();
                }
            }

            void ConstructorController::fail() {
                m_currentStructure = nullptr;
                // notify player that we're unable to construct structure
                unit()->player()->controller()->onUnableToConstruct(
                        unit()->player()->getStructure( m_currentStructureId ),
                        unit() );
                if ( workProvider() ) {
                    workProvider()->unitAvailable( unit() );
                }
            }

            void ConstructorController::setStructure( Structure* structure ) {
                if ( m_currentStructure != nullptr ) {
                    unit()->player()->controller()->onUnableToConstruct(
                            unit()->player()->getStructure( m_currentStructureId ),
                            unit() );
                }
                m_currentStructure = structure;
                m_currentStructureId = m_currentStructure->id();
                if ( workProvider() != nullptr ) {
                    workProvider()->unitUnavailable( unit() );
                }
            }

            void ConstructorController::dump() {
                Controller::dump();
                printf( "Current structure id: %d\n", m_currentStructureId );
                printf( "Current structure: %p\n", m_currentStructure );
            }

        }
    }
}