#pragma once

#include "../../common/types.h"
#include "Controller.h"

namespace isomap {
    namespace client {
        namespace unit {

            class ConstructorController : public Controller {
            public:
                explicit ConstructorController( Unit* unit );

                ~ConstructorController() override;

                ConstructorController( const ConstructorController& ) = delete;

                ConstructorController& operator=( const ConstructorController& ) = delete;

                bool construct( Structure* structure ) override;

                void onMessage( common::UnitServerMessage::Type msgType ) override;

            private:
                void onDone();

                void onAbort();

                Structure* m_structure = nullptr;
                common::UnitState m_state = common::UnitState::Idle;
            };
        }
    }
}
