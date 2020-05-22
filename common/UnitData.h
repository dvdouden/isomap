#pragma once

#include "types.h"
#include "../util/math.h"

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
            uint32_t payload = 0;
            WayPoint wayPoint = {0, 0};

            void setState( UnitState newState ) {
                lastState = state;
                state = newState;
            }


            int32_t tileX() const {
                return x >> math::fix::precisionBits;
            }

            int32_t tileY() const {
                return y >> math::fix::precisionBits;
            }

            int32_t tileZ() const {
                return z >> math::fix::precisionBits;
            }

            int32_t subTileX() const {
                return x & math::fix::precisionMask;
            }

            int32_t subTileY() const {
                return y & math::fix::precisionMask;
            }

            int32_t subTileZ() const {
                return z & math::fix::precisionMask;
            }

            bool onCenterOfTile() const {
                return subTileX() == math::fix::halfPrecision && subTileY() == math::fix::halfPrecision;
            }

            static uint32_t getOrientation( int32_t dX, int32_t dY );

            static void getMotion( int32_t& dX, int32_t& dY, uint32_t orientation );

            void updateMotion( const TerrainData& terrain );

            const char* stateName() const;

            const char* lastStateName() const;
        };

    }
}

