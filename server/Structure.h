#pragma once

#include "Object.h"

namespace isomap { namespace server {

    class Structure : public Object {
    public:
        Structure( id_t owner ) :
        Object( owner ) {}
        ~Structure() override = default;

        Structure( const Structure& ) = delete;
        const Structure& operator=( const Structure& ) = delete;

        void update( World * world ) override;

        // position
        // health
        // type
        // state
    };
} }


