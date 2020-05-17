#include "UnitData.h"
#include "../util/math.h"

namespace isomap {
    namespace common {
        void UnitData::updateMotion() {
            if ( state == Idle ) {
                return;
            }
            int32_t dX = wayPoint.x * math::fix::precision - x;
            int32_t dY = wayPoint.y * math::fix::precision - y;
            orientation = getOrientation( dX, dY );
            getMotion( dX, dY, orientation );
            x += dX * math::fix::precision / 16;
            y += dY * math::fix::precision / 16;

            // FIXME: bounds checking!
            /*
            if ( x < 0 ) {
                x = 0;
            } else if ( x >= terrain->width()  * math::fix::precision) {
                x = (terrain->width() - 1)  * math::fix::precision;
            }
            if ( y < 0 ) {
                y = 0;
            } else if ( y >= terrain->height()  * math::fix::precision) {
                y = (terrain->height() - 1) * math::fix::precision;
            }*/
            // FIXME: height!
            //m_data.z = terrain->heightMap()[(m_data.y /  math::fix::precision) * terrain->width() + (m_data.x /  math::fix::precision)] * math::fix::precision;
            // FIXME: fog!
            //player()->unFog( m_data.x/  math::fix::precision, m_data.y/  math::fix::precision, 20 );
            if ( x == wayPoint.x * math::fix::precision && y == wayPoint.y * math::fix::precision ) {
                lastState = state;
                state = common::Idle;
            }
        }

        uint32_t UnitData::getOrientation( int32_t dX, int32_t dY ) {
            if ( dY > 0 ) {
                if ( dX > 0 ) {
                    return 1;
                } else if ( dX == 0 ) {
                    return 0;
                } else {
                    return 7;
                }
            } else if ( dY == 0 ) {
                if ( dX > 0 ) {
                    return 2;
                } else if ( dX == 0 ) {
                    return 8; // yeah, I don't know either...
                } else {
                    return 6;
                }
            } else {
                if ( dX > 0 ) {
                    return 3;
                } else if ( dX == 0 ) {
                    return 4;
                } else {
                    return 5;
                }
            }
        }

        void UnitData::getMotion( int32_t& dX, int32_t& dY, uint32_t orientation ) {
            switch ( orientation ) {
                case 0:
                    dX = 0;
                    dY = 1;
                    break;
                case 1:
                    dX = 1;
                    dY = 1;
                    break;
                case 2:
                    dX = 1;
                    dY = 0;
                    break;
                case 3:
                    dX = 1;
                    dY = -1;
                    break;
                case 4:
                    dX = 0;
                    dY = -1;
                    break;
                case 5:
                    dX = -1;
                    dY = -1;
                    break;
                case 6:
                    dX = -1;
                    dY = 0;
                    break;
                case 7:
                    dX = -1;
                    dY = 1;
                    break;
                default:
                    dX = 0;
                    dY = 0;
                    break;
            }
        }

        const char* stateToString( UnitState state ) {
            switch ( state ) {
                case isomap::common::Idle:
                    return "idle";
                case isomap::common::Moving:
                    return "moving";
                case isomap::common::Constructing:
                    return "constructing";
                case isomap::common::Harvesting:
                    return "harvesting";
            }
            return "unknown";
        }

        const char* UnitData::stateName() const {
            return stateToString( state );
        }

        const char* UnitData::lastStateName() const {
            return stateToString( lastState );
        }
    }
}
