#pragma once
#include <vlGraphics/RenderingAbstract.hpp>
#include <vlGraphics/Geometry.hpp>
#include "../common/types.h"


namespace isomap {
    namespace client {
        class Structure {
        public:
            Structure() = default;

            ~Structure() = default;

            Structure( const Structure& ) = delete;

            const Structure& operator=( const Structure& ) = delete;

            void processMessage( common::StructureServerMessage* msg );

            //common::StructureCommandMessage* moveTo( int32_t tileX, int32_t tileY );

            void initRender( vl::RenderingAbstract* rendering );

            void render();

        private:
            int32_t m_x = 0;
            int32_t m_y = 0;
            int32_t m_z = 0;
            int32_t m_orientation = 0;

            vl::ref<vl::Geometry> m_geom;
            vl::ref<vl::Transform> m_transform;
            vl::ref<vl::Effect> m_effect;

        };
    }
}


