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

        UnitCommandMessage* UnitCommandMessage::harvestMsg() {
            auto* msg = new UnitCommandMessage( Harvest );
            return msg;
        }

        UnitCommandMessage* UnitCommandMessage::unloadMsg() {
            auto* msg = new UnitCommandMessage( Unload );
            return msg;
        }

        UnitCommandMessage* UnitCommandMessage::loadMsg() {
            auto* msg = new UnitCommandMessage( Load );
            return msg;
        }

        const char* UnitCommandMessage::typeName() const {
            switch ( m_type ) {
                case Move:
                    return "Move";
                case Construct:
                    return "Construct";
                case Harvest:
                    return "Harvest";
                case Unload:
                    return "Unload";
                case Load:
                    return "Load";
            }
            return "Invalid";
        }

        UnitServerMessage* UnitServerMessage::constructMsg( const UnitData& data ) {
            auto* msg = new UnitServerMessage( Construct );
            msg->m_data = data;
            return msg;
        }

        UnitServerMessage* UnitServerMessage::harvestMsg( const UnitData& data ) {
            auto* msg = new UnitServerMessage( Harvest );
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

        UnitServerMessage* UnitServerMessage::doneMsg( const UnitData& data ) {
            auto* msg = new UnitServerMessage( Done );
            msg->m_data = data;
            return msg;
        }

        UnitServerMessage* UnitServerMessage::abortMsg( const UnitData& data ) {
            auto* msg = new UnitServerMessage( Abort );
            msg->m_data = data;
            return msg;
        }

        UnitServerMessage* UnitServerMessage::unloadMsg( const UnitData& data ) {
            auto* msg = new UnitServerMessage( Unload );
            msg->m_data = data;
            return msg;
        }

        UnitServerMessage* UnitServerMessage::loadMsg( const UnitData& data ) {
            auto* msg = new UnitServerMessage( Load );
            msg->m_data = data;
            return msg;
        }

        const char* UnitServerMessage::typeName() const {
            switch ( m_type ) {
                case Construct:
                    return "Construct";
                case Harvest:
                    return "Harvest";
                case Status:
                    return "Status";
                case MoveTo:
                    return "MoveTo";
                case Stop:
                    return "Stop";
                case Done:
                    return "Done";
                case Abort:
                    return "Abort";
                case Unload:
                    return "Unload";
                case Load:
                    return "Load";
            }
            return "Invalid";
        }
    }
}

