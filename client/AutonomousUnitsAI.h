#pragma once

#include <set>
#include <queue>

#include "../common/types.h"

namespace isomap {
    namespace client {
        class AutonomousUnitsAI {
        public:
            explicit AutonomousUnitsAI( Player* player );

            void update();

            void onBuildStructureAccepted( common::PlayerServerMessage* msg );

            void onUnitIdle( Unit* unit );

            void onUnitActive( Unit* unit );

        private:
            Player* m_player;

            std::queue<id_t> m_constructionQueue;
            std::set<id_t> m_idleConstructionUnits;
            std::set<id_t> m_idleHarvesters;
        };
    }
}




