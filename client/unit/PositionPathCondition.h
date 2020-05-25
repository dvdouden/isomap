#pragma once

#include "PathCondition.h"

namespace isomap {
    namespace client {
        namespace unit {

            class PositionPathCondition : public PathCondition {
            public:
                explicit PositionPathCondition( uint32_t target ) :
                        m_target( target ) { }

                bool hasReached( uint32_t idx ) const override {
                    return idx == m_target;
                }

            private:
                uint32_t m_target;
            };

        }

    }
}

