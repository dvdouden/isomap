#include "Object.h"
#include "Player.h"

namespace isomap {
    namespace server {

        Object::Object( Player* player ) :
                m_player( player ) {
            // FIXME: will probably want a more robust method for this
            static id_t id = 0;
            m_id = id;
            ++id;
        }

        bool Object::isSubscribed( Player* player ) const {
            return player == m_player || m_subscribers.find( player->id() ) != m_subscribers.cend();
        }

        void Object::subscribe( Player* player ) {
            m_subscribers.insert( player->id() );
        }

        void Object::unsubscribe( Player* player ) {
            m_subscribers.erase( player->id() );
        }
    }
}