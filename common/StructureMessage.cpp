#include "StructureMessage.h"

namespace isomap {
    namespace common {

        StructureServerMessage*
        StructureServerMessage::statusMsg( const StructureData& data ) {
            auto* msg = new StructureServerMessage( Status );
            msg->m_data = data;
            return msg;
        }
    }
}

