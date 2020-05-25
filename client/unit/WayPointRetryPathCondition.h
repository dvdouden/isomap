#pragma once

#include "PositionPathCondition.h"
#include "../../common/TerrainData.h"

namespace isomap {
    namespace client {
        namespace unit {

            class WayPointRetryPathCondition : public PositionPathCondition {
            public:
                WayPointRetryPathCondition( const common::WayPoint& wayPoint, const common::TerrainData& terrain,
                                            uint32_t value ) :
                        PositionPathCondition( wayPoint.y * terrain.mapWidth + wayPoint.x ),
                        m_value( value ),
                        m_terrain( terrain ) { }

                uint32_t maxValue() const override {
                    return m_value;
                }

                bool canReach( uint32_t idxFrom, uint32_t idxTo ) const override {
                    return (m_terrain.occupancyMap[idxTo] & common::occupancy::bitUnit) == 0;
                }

            private:
                uint32_t m_value;
                const common::TerrainData& m_terrain;
            };
        }
    }
}
