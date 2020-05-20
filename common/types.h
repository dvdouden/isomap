#pragma once

#include <cstdint>

namespace isomap {
    // typedefs
    typedef uint32_t id_t;

    // forward declarations
    namespace common {
        class Area;

        class FootPrint;

        class MatchClientMessage;

        class MatchServerMessage;

        class PlayerCommandMessage;

        class PlayerServerMessage;

        class StructureCommandMessage;

        struct StructureData;

        class StructureServerMessage;

        class StructureType;

        class TerrainMessage;

        struct TerrainData;

        class UnitCommandMessage;

        struct UnitData;

        class UnitServerMessage;

        class UnitType;

        enum UnitState {
            Idle,
            Moving,
            Constructing,
            Harvesting,
            Unloading,
        };

        struct WayPoint {
            int32_t x;
            int32_t y;
        };
    }
}
