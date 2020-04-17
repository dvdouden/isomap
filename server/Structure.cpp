#include "Structure.h"
#include "../common/StructureMessage.h"


namespace isomap {
    namespace server {

        bool Structure::update( Terrain* world ) {
            return false;

        }


        void Structure::processMessage( common::StructureCommandMessage* msg ) {
            switch ( msg->type() ) {
                case common::StructureCommandMessage::Move:
                    break;

                default:
                    break;
            }
        }

        common::StructureServerMessage* Structure::statusMessage() {
            return common::StructureServerMessage::statusMsg( m_x, m_y, m_z );
        }

        common::StructureServerMessage* Structure::createMessage() {
            return common::StructureServerMessage::createMsg( m_x, m_y, m_z );
        }

    }
}
