#pragma once


#include <vector>
#include "types.h"

namespace isomap {
    namespace common {

        class UnitCommandMessage {
        public:
            enum Type {
                Move,
            };

            UnitCommandMessage( Type type ) :
                    m_type( type ) { }

            Type type() const {
                return m_type;
            }

            struct WayPoint {
                int32_t x;
                int32_t y;
            };

            const std::vector<WayPoint>& wayPoints() const {
                return m_wayPoints;
            }

            static UnitCommandMessage* moveMsg( std::vector<WayPoint>& wayPoints );

        private:
            Type m_type;
            std::vector<WayPoint> m_wayPoints;
        };

        class UnitServerMessage {
        public:
            enum Type {
                Create,
                Status
            };

            UnitServerMessage( Type type ) :
                    m_type( type ) { }

            Type type() const {
                return m_type;
            }

            int32_t x() const {
                return m_x;
            }

            int32_t y() const {
                return m_y;
            }

            static UnitServerMessage* statusMsg( int32_t x, int32_t y );

        private:
            Type m_type;

            int32_t m_x;
            int32_t m_y;

        };
    }
}

