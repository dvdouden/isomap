#pragma once

#include "../../common/types.h"
#include "Controller.h"

namespace isomap {
    namespace client {
        namespace unit {

            class ConstructorController : public Controller {
            public:
                ConstructorController( Unit* unit, player::Controller* playerAI );

                ~ConstructorController() override;

                ConstructorController( const ConstructorController& ) = delete;

                ConstructorController& operator=( const ConstructorController& ) = delete;

                void update() override;

                void onIdle() override;

                void onActive() override;

                void onStuck() override;

            private:

            };
        }
    }
}
