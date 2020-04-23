#include "UnitMessage.h"

namespace isomap {
    namespace common {

        UnitCommandMessage* UnitCommandMessage::moveMsg( std::vector<WayPoint>& wayPoints ) {
            auto* msg = new UnitCommandMessage( Move );
            msg->m_wayPoints = std::move( wayPoints );
            return msg;
        }

        UnitServerMessage* UnitServerMessage::statusMsg( id_t id, int32_t x, int32_t y, int32_t z ) {
            auto* msg = new UnitServerMessage( Status );
            msg->m_id = id;
            msg->m_x = x;
            msg->m_y = y;
            msg->m_z = z;
            return msg;
        }
    }
}

