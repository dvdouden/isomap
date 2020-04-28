#pragma once

#include <vector>
#include "Object.h"


namespace isomap {
    namespace server {
        class Unit : public Object {
        public:
            Unit( Player* player, uint32_t x, uint32_t y, uint32_t z ) :
                    Object( player ),
                    m_x( x ),
                    m_y( y ),
                    m_z( z ) { }

            ~Unit() override = default;

            Unit( const Unit& ) = delete;

            const Unit& operator=( const Unit& ) = delete;

            void processMessage( common::UnitCommandMessage* msg );

            common::UnitServerMessage* statusMessage();

            common::PlayerServerMessage* update( Terrain* terrain ) override;

            common::UnitType* getType() const {
                return m_type;
            }

            // state

            int32_t x() const {
                return m_x;
            }

            int32_t y() const {
                return m_y;
            }

            int32_t z() const {
                return m_z;
            }

            int32_t tileX() const {
                return m_x >> 16;
            }

            int32_t tileY() const {
                return m_y >> 16;
            }

            int32_t tileZ() const {
                return m_z >> 16;
            }

            int32_t subTileX() const {
                return m_x & 0x0000FFFF;
            }

            int32_t subTileY() const {
                return m_y & 0x0000FFFF;
            }

            int32_t subTileZ() const {
                return m_z & 0x0000FFFF;
            }

            int32_t orientation() const {
                return m_orientation;
            }

        private:
            int32_t speedX( int32_t speed, int32_t orientation ) const;

            int32_t speedY( int32_t speed, int32_t orientation ) const;

            common::UnitType* m_type;

            int32_t m_x = 0;
            int32_t m_y = 0;
            int32_t m_z = 0;
            int32_t m_orientation = 0;


            int32_t m_speed;


            enum MotionState {
                Accelerating,
                Constant,
                Decelerating,
                Stopped
            };
            MotionState m_motion;

            // motion related
            struct WayPoint {
                int32_t x;
                int32_t y;
                int32_t orientation;
            };

            std::vector<WayPoint> m_wayPoints;

        };
    }
}
