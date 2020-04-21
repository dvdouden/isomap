#pragma once

#include <vlGraphics/RenderingAbstract.hpp>
#include <vlGraphics/Geometry.hpp>
#include "../common/types.h"


namespace isomap {
    namespace client {
        class Structure {
        public:
            Structure( id_t id, int32_t x, int32_t y, int32_t z ) :
                    m_id( id ),
                    m_x( x ),
                    m_y( y ),
                    m_z( z ) { };

            ~Structure() = default;

            Structure( const Structure& ) = delete;

            const Structure& operator=( const Structure& ) = delete;

            void processMessage( common::StructureServerMessage* msg );

            //common::StructureCommandMessage* moveTo( int32_t tileX, int32_t tileY );

            void initRender( vl::RenderingAbstract* rendering );

            void render();

            bool occupies( uint32_t x, uint32_t y, uint32_t width, uint32_t height ) const;

        private:
            id_t m_id = 0;
            int32_t m_x = 0;
            int32_t m_y = 0;
            int32_t m_z = 0;
            int32_t m_orientation = 0;

            uint32_t m_width = 2;
            uint32_t m_height = 3;

            // TODO: Separate render code from game logic
            // We don't need the AI data structures to be renderable
            vl::ref<vl::Geometry> m_geom;
            vl::ref<vl::Transform> m_transform;
            vl::ref<vl::Effect> m_effect;

        };
    }
}


