#include "PlayerMessage.h"

namespace isomap {
    namespace common {

        PlayerCommandMessage* PlayerCommandMessage::buildStructureMsg(uint32_t x, uint32_t y, uint32_t z) {
            auto* msg = new PlayerCommandMessage( BuildStructure );
            msg->m_x = x;
            msg->m_y = y;
            msg->m_z = z;
            return msg;
        }

        PlayerCommandMessage* PlayerCommandMessage::buildUnitMsg(uint32_t x, uint32_t y, uint32_t z) {
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
    }
}

