#pragma once

#include "Object.h"

namespace isomap {
    namespace server {

        class Structure : public Object {
        public:
            Structure( Player* owner ) :
                    Object( owner ) { }

            ~Structure() override = default;

            Structure( const Structure& ) = delete;

            const Structure& operator=( const Structure& ) = delete;

            bool update( Terrain* world ) override;

            // position
            // health
            // type
            // state
        };
    }
}


