#include "UnitMessage.h"

namespace isomap {
    namespace common {

        UnitCommandMessage* UnitCommandMessage::moveMsg( std::vector<WayPoint>& wayPoints ) {
            auto* msg = new UnitCommandMessage( Move );
            // probably should be using the move operator here
            std::swap( msg->m_wayPoints, wayPoints );
            return msg;
        }
    }
}

