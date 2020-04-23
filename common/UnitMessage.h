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

            explicit UnitCommandMessage( Type type ) :
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

            explicit UnitServerMessage( Type type ) :
                    m_type( type ) { }

            Type type() const {
                return m_type;
            }

            id_t id() const {
                return m_id;
            }

            int32_t x() const {
                return m_x;
            }

            int32_t y() const {
                return m_y;
            }

            int32_t z() const {
                return m_z;
            }

            static UnitServerMessage* statusMsg( id_t id, int32_t x, int32_t y, int32_t z );

        private:
            Type m_type;

            id_t m_id;
            int32_t m_x;
            int32_t m_y;
            int32_t m_z;

        };
    }
}

