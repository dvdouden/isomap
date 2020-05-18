#pragma once

#include <vector>

#include "../types.h"
#include "../Unit.h"

namespace isomap {
    namespace client {
        namespace unit {

            class Controller {
            public:
                explicit Controller( Unit* unit );

                virtual ~Controller();

                Controller( const Controller& ) = delete;

                Controller& operator=( const Controller& ) = delete;

                bool moveTo( uint32_t x, uint32_t y );

                bool moveTo( Structure* structure );

                virtual bool construct( Structure* structure );

                virtual bool harvest();

                virtual bool unload();

                virtual void update();

                virtual void onMessage( common::UnitServerMessage::Type msgType );

                virtual void dump();

                Unit* unit() const {
                    return m_unit;
                }

                void setWorkProvider( WorkProvider* workProvider ) {
                    m_workProvider = workProvider;
                }

                WorkProvider* workProvider() const {
                    return m_workProvider;
                }

            private:
                bool moveTo( uint32_t x, uint32_t y, Structure* structure );

                Unit* m_unit;
                std::vector<common::WayPoint> m_wayPoints;
                WorkProvider* m_workProvider = nullptr;

            };
        }

    }
}
