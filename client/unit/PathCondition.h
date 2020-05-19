#pragma once

#include "../types.h"

namespace isomap {
    namespace client {
        namespace unit {

            class PathCondition {
            public:
                virtual bool hasReached( uint32_t idx ) const = 0;

            };


        }

    }
}

