#pragma once

#include <cstdint>

namespace isomap {
    // typedefs
    typedef uint32_t id_t;

    // forward declarations
    namespace server {
        class Terrain;

        class TerrainGenerator;

        class Object;

        class Unit;

        class Structure;

        class Player;

        class Match;
    }

    namespace client {
        class AutonomousUnitsAI;

        class Match;

        class Player;

        class Structure;

        class Terrain;

        class Unit;

        namespace match {
            class Renderer;
        }

        namespace player {
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
        }
    }

    namespace common {
        class Area;

        class FootPrint;

        class MatchClientMessage;

        class MatchServerMessage;

        class PlayerCommandMessage;

        class PlayerServerMessage;

        class UnitCommandMessage;

        class UnitServerMessage;

        class UnitType;

        class StructureCommandMessage;

        struct StructureData;

        class StructureServerMessage;

        class StructureType;

        class TerrainMessage;

        struct UnitData;

        enum UnitState {
            Idle,
            Moving,
            Constructing,
            Harvesting
        };

        struct WayPoint {
            int32_t x;
            int32_t y;
        };
    }
}
