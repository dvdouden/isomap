#pragma once

#include <limits>

#include "../types.h"

namespace isomap {
    namespace client {
        namespace unit {

            class PathCondition {
            public:
                virtual bool hasReached( uint32_t idx ) const = 0;

                virtual uint32_t maxValue() const {
                    return std::numeric_limits<uint32_t>::max();
                }

                virtual bool canReach( uint32_t idxFrom, uint32_t idxTo ) const {
                    return true;
                }

            };


        }

    }
}

