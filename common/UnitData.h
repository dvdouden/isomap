#pragma once

#include "types.h"

namespace isomap {
    namespace common {

        // data shared between client and server unit objects
        struct UnitData {
            id_t id = 0;
            id_t typeId = 0;
            int32_t x = 0;
            int32_t y = 0;
            int32_t z = 0;
            uint32_t orientation = 0;
            UnitMotionState motionState = UnitMotionState::Stopped;
            WayPoint wayPoint = {0, 0};

            void updateMotion();
        };

    }
}

