#include "PlayerMessage.h"

namespace isomap {
    namespace common {

        PlayerCommandMessage*
        PlayerCommandMessage::buildStructureMsg( uint32_t x, uint32_t y, uint32_t z, id_t id, uint32_t rotation ) {
            auto* msg = new PlayerCommandMessage( BuildStructure );
            msg->m_id = id;
            msg->m_x = x;
            msg->m_y = y;
            msg->m_z = z;
            msg->m_rotation = rotation;
            return msg;
        }

        PlayerCommandMessage* PlayerCommandMessage::buildUnitMsg( uint32_t x, uint32_t y, uint32_t z ) {
            auto* msg = new PlayerCommandMessage( BuildUnit );
            msg->m_x = x;
            msg->m_y = y;
            msg->m_z = z;
            return msg;
        }


        PlayerServerMessage* PlayerServerMessage::statusMsg() {
            auto* msg = new PlayerServerMessage( Status );
            return msg;
        }

        PlayerServerMessage*
        PlayerServerMessage::buildStructureAcceptedMsg( uint32_t x, uint32_t y, uint32_t z, id_t id, id_t typeId,
                                                        uint32_t rotation ) {
            auto* msg = new PlayerServerMessage( BuildStructureAccepted );
            msg->m_x = x;
            msg->m_y = y;
            msg->m_z = z;
            msg->m_id = id;
            msg->m_typeId = typeId;
            msg->m_rotation = rotation;
            return msg;
        }

        PlayerServerMessage*
        PlayerServerMessage::buildStructureRejectedMsg( uint32_t x, uint32_t y, uint32_t z, id_t typeId,
                                                        uint32_t rotation ) {
            auto* msg = new PlayerServerMessage( BuildStructureRejected );
            msg->m_x = x;
            msg->m_y = y;
            msg->m_z = z;
            msg->m_typeId = typeId;
            msg->m_rotation = rotation;
            return msg;
        }

        PlayerServerMessage* PlayerServerMessage::unitCreatedMsg( uint32_t x, uint32_t y, uint32_t z, id_t id ) {
            auto* msg = new PlayerServerMessage( UnitCreated );
            msg->m_x = x;
            msg->m_y = y;
            msg->m_z = z;
            msg->m_id = id;
            return msg;
        }


        PlayerServerMessage* PlayerServerMessage::structureMsg( StructureServerMessage* msg ) {
            auto* ret = new PlayerServerMessage( StructureMessage );
            ret->m_structureMessage = msg;
            return ret;
        }

        PlayerServerMessage* PlayerServerMessage::unitMsg( UnitServerMessage* msg ) {
            auto* ret = new PlayerServerMessage( UnitMessage );
            ret->m_unitMessage = msg;
            return ret;
        }
    }
}

