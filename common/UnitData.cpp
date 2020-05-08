#include "UnitData.h"
#include "../util/math.h"

namespace isomap {
    namespace common {
        void UnitData::updateMotion() {
            if ( motionState == Stopped ) {
                return;
            }

            if ( x != wayPoint.x * math::fix::precision ) {
                if ( x < wayPoint.x * math::fix::precision ) {
                    x += math::fix::precision / 16;
                } else {
                    x -= math::fix::precision / 16;
                }
            }
            if ( y != wayPoint.y * math::fix::precision ) {
                if ( y < wayPoint.y * math::fix::precision ) {
                    y += math::fix::precision / 16;
                } else {
                    y -= math::fix::precision / 16;
                }
            }
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
                motionState = common::Stopped;
            }
        }
    }
}
