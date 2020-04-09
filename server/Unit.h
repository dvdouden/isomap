#pragma once

#include <vector>
#include "Object.h"


namespace isomap {
    namespace server {
        class Unit : public Object {
        public:
            Unit( Player* owner, UnitType* type ) :
                    Object( owner ),
                    m_type( type ) {

            }

            ~Unit() override = default;

            Unit( const Unit& ) = delete;

            const Unit& operator=( const Unit& ) = delete;

            void processMessage( common::UnitCommandMessage* msg );

            bool update( Terrain* terrain ) override;

            UnitType* getType() const {
                return m_type;
            }

            // state

            int32_t getX() const {
                return m_x;
            }

            int32_t getY() const {
                return m_x;
            }

            int32_t getZ() const {
                return m_x;
            }

            int32_t getTileX() const {
                return m_x >> 16;
            }

            int32_t getTileY() const {
                return m_y >> 16;
            }

            int32_t getTileZ() const {
                return m_z >> 16;
            }

            int32_t getSubTileX() const {
                return m_x & 0x0000FFFF;
            }

            int32_t getSubTileY() const {
                return m_y & 0x0000FFFF;
            }

            int32_t getSubTileZ() const {
                return m_z & 0x0000FFFF;
            }

            int32_t getOrientation() const {
                return m_orientation;
            }

        private:
            int32_t speedX( int32_t speed, int32_t orientation ) const;

            int32_t speedY( int32_t speed, int32_t orientation ) const;

            UnitType* m_type;

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
