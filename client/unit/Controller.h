#pragma once

#include <vector>

#include "../../common/types.h"
#include "../Unit.h"

namespace isomap {
    namespace client {
        namespace unit {

            class Controller {
            public:
                Controller( Unit* unit );

                virtual ~Controller();

                Controller( const Controller& ) = delete;

                Controller& operator=( const Controller& ) = delete;

                bool moveTo( uint32_t x, uint32_t y );

                bool moveTo( Structure* structure );

                bool construct( Structure* structure );

                virtual void update();

                virtual void onMessage( common::UnitServerMessage::Type msgType );

                virtual void dump();

            private:
                bool moveTo( uint32_t x, uint32_t y, Structure* structure );

                Unit* m_unit;
                std::vector<common::WayPoint> m_wayPoints;
            };
        }

    }
}
