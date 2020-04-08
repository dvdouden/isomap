#include "Object.h"

namespace isomap { namespace server {

    Object::Object( id_t owner ) :
    m_owner ( owner ) {
        // FIXME: will probably want a more robust method for this
        static id_t id = 0;
        m_id = id;
        ++id;
    }
} }