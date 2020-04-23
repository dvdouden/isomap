#include "StructureMessage.h"

namespace isomap {
    namespace common {

        /*StructureCommandMessage* StructureCommandMessage::moveMsg(  ) {
            auto* msg = new StructureCommandMessage( Move );
            return msg;
        }*/

        StructureServerMessage* StructureServerMessage::createMsg( id_t id, int32_t x, int32_t y, int32_t z ) {
            auto* msg = new StructureServerMessage( Create );
            msg->m_id = id;
            msg->m_x = x;
            msg->m_y = y;
            msg->m_z = z;
            return msg;
        }

        StructureServerMessage*
        StructureServerMessage::statusMsg( id_t id, int32_t x, int32_t y, int32_t z, int32_t constructionProgress ) {
            auto* msg = new StructureServerMessage( Status );
            msg->m_id = id;
            msg->m_x = x;
            msg->m_y = y;
            msg->m_z = z;
            msg->m_constructionProgress = constructionProgress;
            return msg;
        }
    }
}

