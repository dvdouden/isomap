#pragma once

#include <vlGraphics/RenderingAbstract.hpp>
#include <vlGraphics/Geometry.hpp>
#include "../common/UnitData.h"
#include "../common/UnitType.h"
#include "../common/types.h"
#include "../util/math.h"

namespace isomap {
    namespace client {
        class Unit {
        public:

            Unit( Player* player, const common::UnitData& data );

            ~Unit();

            Unit( const Unit& ) = delete;

            const Unit& operator=( const Unit& ) = delete;

            void processMessage( common::UnitServerMessage* msg );

            void update();

            void moveTo( int32_t targetX, int32_t targetY );

            void initRender( vl::RenderingAbstract* rendering, vl::SceneManagerActorTree* sceneManager );

            void render();

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

        private:
            Player* m_player;
            common::UnitData m_data;
            common::UnitType* m_type = nullptr;
            bool m_visible = true;

            // TODO: Separate render code from game logic
            // We don't need the AI data structures to be renderable
            vl::ref<vl::Transform> m_transform;
            vl::ref<vl::Effect> m_effect;
            std::vector<vl::Actor*> m_actors;
        };
    }
}



