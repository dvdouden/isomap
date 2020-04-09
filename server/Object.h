#pragma once

#include "../common/types.h"

namespace isomap { namespace server {
    class Object {
    public:
        Object( Player* owner );
        virtual ~Object() = default;

        Object( const Object& ) = delete;
        const Object& operator=( const Object& ) = delete;


        id_t id() const {
            return m_id;
        }

        Player* owner() const {
            return m_owner;
        }

        virtual bool update(Terrain* world ) = 0;

        // health
        //

    private:
        id_t m_id;
        Player* m_owner;

    };
} }
