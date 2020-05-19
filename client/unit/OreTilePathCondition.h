#pragma once

#include "PathCondition.h"

namespace isomap {
    namespace client {
        namespace unit {

            class OreTilePathCondition : public PathCondition {
            public:
                explicit OreTilePathCondition( uint8_t* oreMap ) :
                        m_oreMap( oreMap ) { }

                bool hasReached( uint32_t idx ) const override {
                    return m_oreMap[idx] != 0;
                }

            private:
                uint8_t* m_oreMap;
            };


        }

    }
}

