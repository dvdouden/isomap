#pragma once

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

            // health
            //

        private:
            id_t m_id;
            Player* m_player;

        };
    }
}
