#include "Controller.h"
#include "../player/Controller.h"

namespace isomap {
    namespace client {
        namespace unit {

            Controller::Controller( Unit* unit, player::Controller* playerAI ) :
                    m_unit( unit ),
                    m_playerController( playerAI ) {

            }

            Controller::~Controller() = default;
        }
    }
}