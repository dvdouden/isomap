#pragma once

#include "../../common/types.h"
#include "Controller.h"

namespace isomap {
    namespace client {
        namespace unit {

            class ConstructorController : public Controller {
            public:
                ConstructorController( Unit* unit );

                ~ConstructorController() override;

                ConstructorController( const ConstructorController& ) = delete;

                ConstructorController& operator=( const ConstructorController& ) = delete;

            private:

            };
        }
    }
}
