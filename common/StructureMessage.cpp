#include "StructureMessage.h"

namespace isomap {
    namespace common {

        /*StructureCommandMessage* StructureCommandMessage::moveMsg(  ) {
            auto* msg = new StructureCommandMessage( Move );
            return msg;
        }*/

        StructureServerMessage* StructureServerMessage::createMsg( int32_t x, int32_t y, int32_t z ) {
            auto* msg = new StructureServerMessage( Create );
            msg->m_x = x;
            msg->m_y = y;
            msg->m_z = z;
            return msg;
        }

        StructureServerMessage* StructureServerMessage::statusMsg( int32_t x, int32_t y, int32_t z ) {
            auto* msg = new StructureServerMessage( Status );
            msg->m_x = x;
            msg->m_y = y;
            msg->m_z = z;
            return msg;
        }
    }
}

