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
        class Match;

        class Player;

        class Structure;

        class Terrain;

        class Unit;

    }

    namespace common {
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


        enum UnitMotionState {
            Stopped,
            Moving,
        };

        struct WayPoint {
            int32_t x;
            int32_t y;
        };
    }
}
