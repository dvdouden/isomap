#include "UnitMessage.h"

namespace isomap {
    namespace common {

        UnitCommandMessage* UnitCommandMessage::moveMsg( std::vector<WayPoint>& wayPoints ) {
            auto* msg = new UnitCommandMessage( Move );
            msg->m_wayPoints = std::move( wayPoints );
            return msg;
        }

        UnitServerMessage* UnitServerMessage::statusMsg( const UnitData& data ) {
            auto* msg = new UnitServerMessage( Status );
            msg->m_data = data;
            return msg;
        }
    }
}

