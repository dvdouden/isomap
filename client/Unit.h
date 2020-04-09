#pragma once

#include <vlGraphics/RenderingAbstract.hpp>
#include "../common/types.h"

namespace isomap {
    namespace client {
        class Unit {
        public:
            Unit() = default;

            ~Unit() = default;

            Unit( const Unit& ) = delete;

            const Unit& operator=( const Unit& ) = delete;

            void processMessage( common::UnitServerMessage* msg );

            common::UnitCommandMessage* moveTo( int32_t x, int32_t y );

            void render( vl::RenderingAbstract* rendering );

        private:
            int32_t m_x = 0;
            int32_t m_y = 0;
            int32_t m_z = 0;
            int32_t m_orientation = 0;

        };
    }
}


