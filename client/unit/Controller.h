#pragma once

#include "../../common/types.h"
#include "../Unit.h"
#include "../player/Controller.h"

namespace isomap {
    namespace client {
        namespace unit {

            class Controller {
            public:
                Controller( Unit* unit, player::Controller* playerController );

                virtual ~Controller();

                Controller( const Controller& ) = delete;

                Controller& operator=( const Controller& ) = delete;

                virtual void update() = 0;

                virtual void onIdle() = 0;

                virtual void onActive() = 0;

                virtual void onStuck() = 0;

            private:
                Unit* m_unit;
                player::Controller* m_playerController;
            };
        }

    }
}
