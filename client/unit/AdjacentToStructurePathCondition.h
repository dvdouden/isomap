#pragma once

#include "PathCondition.h"
#include "../Structure.h"

namespace isomap {
    namespace client {
        namespace unit {

            class AdjacentToStructurePathCondition : public PathCondition {
            public:
                explicit AdjacentToStructurePathCondition( Structure* structure ) :
                        m_structure( structure ) { }

                bool hasReached( uint32_t idx ) const override {
                    return m_structure->isAdjacentTo( idx );
                }

            private:
                Structure* m_structure;
            };


        }

    }
}

