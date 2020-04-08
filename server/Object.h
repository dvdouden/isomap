#pragma once

#include "types.h"

namespace isomap { namespace server {
    class Object {
    public:
        Object( id_t owner );
        virtual ~Object() = default;

        Object( const Object& ) = delete;
        const Object& operator=( const Object& ) = delete;


        id_t id() const {
            return m_id;
        }

        id_t owner() const {
            return m_owner;
        }

        virtual void update( World* world ) = 0;

        // health
        //

    private:
        id_t m_id;
        id_t m_owner;

    };
} }
