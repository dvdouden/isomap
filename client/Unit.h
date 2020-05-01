#pragma once

#include <vlGraphics/RenderingAbstract.hpp>
#include <vlGraphics/Geometry.hpp>
#include "../common/types.h"

namespace isomap {
    namespace client {
        class Unit {
        public:
            Unit( id_t id, int32_t x, int32_t y, int32_t z ) :
                    m_id( id ),
                    m_x( x ),
                    m_y( y ),
                    m_z( z ) { };

            ~Unit() = default;

            Unit( const Unit& ) = delete;

            const Unit& operator=( const Unit& ) = delete;

            void processMessage( common::UnitServerMessage* msg );

            common::UnitCommandMessage* moveTo( int32_t tileX, int32_t tileY );

            void initRender( vl::RenderingAbstract* rendering, vl::SceneManagerActorTree* sceneManager );

            void render();


        private:
            id_t m_id = 0;
            int32_t m_x = 0;
            int32_t m_y = 0;
            int32_t m_z = 0;
            int32_t m_orientation = 0;

            // TODO: Separate render code from game logic
            // We don't need the AI data structures to be renderable
            vl::ref<vl::Geometry> m_geom;
            vl::ref<vl::Transform> m_transform;
            vl::ref<vl::Effect> m_effect;

        };
    }
}



