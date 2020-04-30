#include "Structure.h"
#include "../common/PlayerMessage.h"
#include "../common/StructureMessage.h"


namespace isomap {
    namespace server {

        common::PlayerServerMessage* Structure::update( Terrain* world ) {
            if ( m_data.constructionProgress < 100 ) {
                m_data.constructionProgress++;
                return common::PlayerServerMessage::structureMsg( statusMessage() );
            }
            return nullptr;
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
            return common::StructureServerMessage::statusMsg( id(), m_data.x, m_data.y, m_data.z,
                                                              m_data.constructionProgress );
        }

        common::StructureServerMessage* Structure::createMessage() {
            return common::StructureServerMessage::createMsg( id(), m_data.x, m_data.y, m_data.z );
        }

    }
}
