#include "ConstructorController.h"
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
                m_structure = nullptr;
                m_state = common::UnitState::Idle;

                if ( structure->isAdjacentTo( unit()->tileX(), unit()->tileY() ) ) {
                    if ( Controller::construct( structure ) ) {
                        m_structure = structure;
                        return true;
                    }
                    return false;
                }

                if ( Controller::moveTo( structure ) ) {
                    m_structure = structure;
                    return true;
                }
                printf( "[%d] Construct command given to unit but unable to reach structure!\n",
                        unit()->id() );
                return false;
            }

            void ConstructorController::onMessage( common::UnitServerMessage::Type msgType ) {
                if ( m_structure == nullptr ) {
                    Controller::onMessage( msgType );
                    return;
                }

                switch ( msgType ) {
                    case common::UnitServerMessage::Construct:
                        m_state = common::UnitState::Constructing;
                        break;

                    case common::UnitServerMessage::Stop:
                    case common::UnitServerMessage::Status:
                        break;

                    case common::UnitServerMessage::MoveTo:
                        m_state = common::UnitState::Moving;
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
                switch ( m_state ) {
                    case common::Moving: // reached target
                        construct( m_structure );
                        break;

                    case common::Constructing:
                        m_structure = nullptr;
                        m_state = common::Idle;
                        break;

                    default:
                        // ignore
                        break;
                }
            }

            void ConstructorController::onAbort() {
                switch ( m_state ) {
                    case common::Moving:        // failed to reach target, retry
                    case common::Constructing:  // failed to construct target? Retry
                        construct( m_structure );
                        break;

                    default:
                        // ignore
                        break;
                }
            }


        }
    }
}