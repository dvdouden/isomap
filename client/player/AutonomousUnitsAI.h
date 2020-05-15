#pragma once

#include <set>
#include <queue>

#include "../../common/types.h"
#include "Controller.h"

namespace isomap {
    namespace client {
        class AutonomousUnitsAI : public player::Controller {
        public:
            explicit AutonomousUnitsAI( Player* player );

            void update() override;

            void onUnitCreated( Unit* unit ) override;

            void onStructureCreated( Structure* structure ) override;

            void onUnitIdle( Unit* unit );

            void onUnitActive( Unit* unit );

            void onUnitStuck( Unit* unit );

        private:
            std::queue<id_t> m_constructionQueue;
            std::set<id_t> m_idleConstructionUnits;
            std::set<id_t> m_idleHarvesters;
            std::queue<id_t> m_stuckUnitsQueue;
        };
    }
}




