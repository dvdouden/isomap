#include "Controller.h"
#include "../player/Controller.h"
#include "../Structure.h"
#include "../Player.h"
#include "../Terrain.h"

namespace isomap {
    namespace client {
        namespace structure {

            Controller::Controller( Structure* structure ) :
                    m_structure( structure ) {

            }

            Controller::~Controller() = default;

            void Controller::dump() {
                printf( "Controller:\n" );
            }

            void Controller::onMessage( common::StructureServerMessage::Type msgType ) {
                switch ( msgType ) {
                    case common::StructureServerMessage::Status:
                        break;
                    case common::StructureServerMessage::Completed:
                        m_structure->player()->controller()->onConstructionComplete( m_structure );
                        break;
                }
            }

            void Controller::update() {
                /*switch ( m_structure->state() ) {
                    default:
                        break;
                }*/
            }

        }
    }
}