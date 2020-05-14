#pragma once

#include "../common/types.h"

namespace isomap {
    namespace client {
        class AutonomousUnitsAI {
        public:
            explicit AutonomousUnitsAI( Player* player );

            void update();

            void onBuildStructureAccepted( common::PlayerServerMessage* msg );

        private:
            Player* m_player;
        };
    }
}




