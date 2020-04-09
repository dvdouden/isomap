#include "Unit.h"
#include "../common/UnitMessage.h"

namespace isomap {
    namespace client {

        common::UnitCommandMessage* Unit::moveTo(int32_t x, int32_t y) {
            // convert into waypoints
            int32_t stepX = m_x;
            int32_t stepY = m_y;
            std::vector<common::UnitCommandMessage::WayPoint> wayPoints;
            do {
                if ( stepX < x ) {
                    ++stepX;
                } else if ( stepX > x ) {
                    --stepX;
                }
                if ( stepY < y ) {
                    ++stepY;
                } else if ( stepY > y ) {
                    --stepY;
                }
                wayPoints.push_back( {stepX, stepY} );
            }
            while( stepX != x || stepY != y );
            return common::UnitCommandMessage::moveMsg( wayPoints );
        }

    }
}