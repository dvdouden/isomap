#include "StructureMessage.h"

namespace isomap {
    namespace common {

        StructureServerMessage* StructureServerMessage::completedMsg( const StructureData& data ) {
            auto* msg = new StructureServerMessage( Completed );
            msg->m_data = data;
            return msg;
        }

        StructureServerMessage* StructureServerMessage::statusMsg( const StructureData& data ) {
            auto* msg = new StructureServerMessage( Status );
            msg->m_data = data;
            return msg;
        }
    }
}

