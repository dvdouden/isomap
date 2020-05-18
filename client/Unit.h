#pragma once

#include <memory>
#include <queue>
#include <vector>


#include "../common/UnitData.h"
#include "../common/UnitType.h"
#include "types.h"
#include "../util/math.h"
#include "../common/UnitMessage.h"

#include "unit/Controller.h"
#include "unit/Renderer.h"

namespace isomap {
    namespace client {
        class Unit {
        public:

            Unit( Player* player, const common::UnitData& data );

            ~Unit();

            Unit( const Unit& ) = delete;

            const Unit& operator=( const Unit& ) = delete;

            void setController( unit::Controller* controller ) {
                m_controller.reset( controller );
            }

            void setRenderer( unit::Renderer* renderer ) {
                m_renderer.reset( renderer );
            }

            unit::Controller* controller() const {
                return m_controller.get();
            }

            unit::Renderer* renderer() const {
                return m_renderer.get();
            }

            void processMessage( common::UnitServerMessage* msg );

            void update();

            void setVisible( const common::UnitData& data );

            void setInvisible();

            bool visible() const {
                return m_visible;
            }

            common::UnitType* type() const {
                return m_type;
            }

            Player* player() const {
                return m_player;
            }

            id_t id() const {
                return m_data.id;
            }

            uint32_t orientation() const {
                return m_data.orientation;
            }

            uint32_t x() const {
                return m_data.x;
            }

            uint32_t y() const {
                return m_data.y;
            }

            uint32_t z() const {
                return m_data.z;
            }

            int32_t tileX() const {
                return m_data.x >> math::fix::precisionBits;
            }

            int32_t tileY() const {
                return m_data.y >> math::fix::precisionBits;
            }

            int32_t tileZ() const {
                return m_data.z >> math::fix::precisionBits;
            }

            int32_t subTileX() const {
                return m_data.x & math::fix::precisionMask;
            }

            int32_t subTileY() const {
                return m_data.y & math::fix::precisionMask;
            }

            int32_t subTileZ() const {
                return m_data.z & math::fix::precisionMask;
            }

            bool onCenterOfTile() const {
                return subTileX() == 0 && subTileY() == 0;
            }

            bool isAdjacentTo( const Structure* structure ) const;

            common::UnitState state() const {
                return m_data.state;
            }

            common::UnitState lastState() const {
                return m_data.lastState;
            }

            const char* stateName() const {
                return m_data.stateName();
            }

            const char* lastStateName() const {
                return m_data.lastStateName();
            }

            uint32_t payload() const {
                return m_data.payload;
            }

            void dump() const;

        private:

            Player* m_player;
            common::UnitData m_data;
            common::UnitType* m_type = nullptr;
            bool m_visible = true;
            std::unique_ptr<unit::Controller> m_controller;
            std::unique_ptr<unit::Renderer> m_renderer;

        };
    }
}



