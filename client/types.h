#pragma once

#include "../common/types.h"

namespace isomap {

    namespace client {
        class Match;

        class Player;

        class Structure;

        class Terrain;

        class Unit;

        namespace match {
            class Renderer;
        }

        namespace player {
            class AutonomousUnitsController;

            class Controller;

            class Renderer;
        }

        namespace structure {
            class Controller;

            class Renderer;
        }

        namespace terrain {
            class Renderer;
        }

        namespace unit {
            class Controller;

            class ConstructorController;

            class Renderer;

            class WorkProvider;
        }
    }
}
