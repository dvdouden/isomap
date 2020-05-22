#pragma once

#include <vector>
#include "Object.h"
#include "../common/UnitData.h"
#include "../common/UnitType.h"
#include "../util/math.h"


namespace isomap {
    namespace server {
        class Unit : public Object {
        public:
            Unit( Player* owner, int32_t x, int32_t y, int32_t z, common::UnitType* unitType, uint32_t orientation );

            ~Unit() override = default;

            Unit( const Unit& ) = delete;

            const Unit& operator=( const Unit& ) = delete;

            void processMessage( common::UnitCommandMessage* msg );

            common::PlayerServerMessage* update( Terrain* terrain ) override;

            void destroy();

            common::UnitType* type() const {
                return m_type;
            }

            const common::UnitData& data() const {
                return m_data;
            }

            int32_t x() const {
                return m_data.x;
            }

            int32_t y() const {
                return m_data.y;
            }

            int32_t z() const {
                return m_data.z;
            }

            int32_t tileX() const {
                return m_data.tileX();
            }

            int32_t tileY() const {
                return m_data.tileY();
            }

            int32_t tileZ() const {
                return m_data.tileZ();
            }

            int32_t subTileX() const {
                return m_data.subTileX();
            }

            int32_t subTileY() const {
                return m_data.subTileY();
            }

            int32_t subTileZ() const {
                return m_data.subTileZ();
            }

            bool onCenterOfTile() const {
                return m_data.onCenterOfTile();
            }

            int32_t orientation() const {
                return m_data.orientation;
            }

            void dump() override;

        private:
            int32_t speedX( int32_t speed, int32_t orientation ) const;

            int32_t speedY( int32_t speed, int32_t orientation ) const;

            common::UnitData m_data;
            common::UnitType* m_type = nullptr;

            int32_t m_speed = 0;

            std::vector<common::WayPoint> m_wayPoints;

        };
    }
}
