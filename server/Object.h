#pragma once

#include <set>

#include "../common/types.h"

namespace isomap {
    namespace server {
        class Object {
        public:
            Object( Player* player );

            virtual ~Object() = default;

            Object( const Object& ) = delete;

            const Object& operator=( const Object& ) = delete;


            id_t id() const {
                return m_id;
            }

            Player* player() const {
                return m_player;
            }

            virtual common::PlayerServerMessage* update( Terrain* world ) = 0;

            bool isSubscribed( Player* player ) const;

            void subscribe( Player* player );

            void unsubscribe( Player* player );

        private:
            id_t m_id;
            Player* m_player;
            std::set<id_t> m_subscriptions;

        };
    }
}
