#pragma once

#include <memory>
#include <set>
#include <queue>

#include "../types.h"
#include "Controller.h"
#include "ConstructionController.h"

namespace isomap {
    namespace client {
        namespace player {
            class AutonomousUnitsController : public player::Controller {
            public:
                explicit AutonomousUnitsController( Player* player );

                void update() override;

                void onUnitCreated( Unit* unit ) override;

                void onUnitDestroyed( Unit* unit ) override;

                void onStructureCreated( Structure* structure ) override;

                void onConstructionComplete( Structure* structure ) override;

                void onStructureDestroyed( Structure* structure ) override;

                void onUnableToConstruct( Structure* structure, Unit* unit ) override;

                void dump() const override;

            private:
                std::unique_ptr<player::ConstructionController> m_constructionController;
            };
        }
    }
}




