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

        class UnitType;

        class StructureType;

        class Player;

        class Match;
    }

    namespace client {
        class Terrain;

        class Unit;
    }

    namespace common {
        class TerrainMessage;

        class UnitCommandMessage;

        class UnitServerMessage;
    }
}
