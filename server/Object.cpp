#include "Object.h"

namespace isomap { namespace server {

    Object::Object( Player* player ) :
            m_player ( player ) {
        // FIXME: will probably want a more robust method for this
        static id_t id = 0;
        m_id = id;
        ++id;
    }
} }