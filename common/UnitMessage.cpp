#include "UnitMessage.h"

namespace isomap {
    namespace common {

        UnitCommandMessage* UnitCommandMessage::moveMsg( const std::vector<WayPoint>& wayPoints ) {
            auto* msg = new UnitCommandMessage( Move );
            msg->m_wayPoints = wayPoints;
            return msg;
        }

        UnitCommandMessage* UnitCommandMessage::constructMsg( id_t structureId ) {
            auto* msg = new UnitCommandMessage( Construct );
            msg->m_id = structureId;
            return msg;
        }

        const char* UnitCommandMessage::typeName() const {
            switch ( m_type ) {
                case Move:
                    return "Move";
                case Construct:
                    return "Construct";
            }
            return "Invalid";
        }

        UnitServerMessage* UnitServerMessage::constructMsg( const UnitData& data ) {
            auto* msg = new UnitServerMessage( Construct );
            msg->m_data = data;
            return msg;
        }

        UnitServerMessage* UnitServerMessage::statusMsg( const UnitData& data ) {
            auto* msg = new UnitServerMessage( Status );
            msg->m_data = data;
            return msg;
        }

        UnitServerMessage* UnitServerMessage::moveToMsg( const UnitData& data ) {
            auto* msg = new UnitServerMessage( MoveTo );
            msg->m_data = data;
            return msg;
        }

        UnitServerMessage* UnitServerMessage::stopMsg( const UnitData& data ) {
            auto* msg = new UnitServerMessage( Stop );
            msg->m_data = data;
            return msg;
        }

        const char* UnitServerMessage::typeName() const {
            switch ( m_type ) {
                case Construct:
                    return "Construct";
                case Status:
                    return "Status";
                case MoveTo:
                    return "MoveTo";
                case Stop:
                    return "Stop";
            }
            return "Invalid";
        }
    }
}

