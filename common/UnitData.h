#pragma once

#include "types.h"

namespace isomap {
    namespace common {

        // data shared between client and server unit objects
        struct UnitData {
            id_t id = 0;
            id_t typeId = 0;
            id_t structureId = 0;
            int32_t x = 0;
            int32_t y = 0;
            int32_t z = 0;
            uint32_t orientation = 0;
            UnitState state = UnitState::Idle;
            UnitState lastState = UnitState::Idle;
            WayPoint wayPoint = {0, 0};

            void setState( UnitState newState ) {
                lastState = state;
                state = newState;
            }

            uint32_t getOrientation( int32_t dX, int32_t dY );

            void getMotion( int32_t& dX, int32_t& dY, uint32_t orientation );

            void updateMotion();

            const char* stateName() const;

            const char* lastStateName() const;
        };

    }
}

