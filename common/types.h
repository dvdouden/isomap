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
        class Player;

        class Structure;

        class Terrain;

        class Unit;

    }

    namespace common {
        class FootPrint;

        class TerrainMessage;

        class PlayerCommandMessage;

        class PlayerServerMessage;

        class UnitCommandMessage;

        class UnitServerMessage;

        class UnitType;

        class StructureCommandMessage;

        class StructureServerMessage;

        class StructureType;
    }
}
