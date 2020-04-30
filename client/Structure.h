#pragma once

#include <vlGraphics/RenderingAbstract.hpp>
#include <vlGraphics/Geometry.hpp>
#include "../common/StructureData.h"
#include "../common/StructureType.h"
#include "../common/types.h"


namespace isomap {
    namespace client {
        class Structure {
        public:
            explicit Structure( const common::StructureData& data ) :
                    m_data( data ),
                    m_type( common::StructureType::get( data.typeId ) ) { };

            ~Structure() = default;

            Structure( const Structure& ) = delete;

            const Structure& operator=( const Structure& ) = delete;

            void processMessage( common::StructureServerMessage* msg );

            //common::StructureCommandMessage* moveTo( int32_t tileX, int32_t tileY );

            void initRender( vl::RenderingAbstract* rendering );

            void render();

            bool occupies( uint32_t x, uint32_t y, uint32_t width, uint32_t height ) const;

            common::StructureType* type() {
                return m_type;
            }

            common::FootPrint* footPrint() {
                return m_type->footPrint( m_data.orientation );
            }

            const common::StructureData& data() const {
                return m_data;
            }

        private:
            common::StructureData m_data;
            common::StructureType* m_type = nullptr;

            // TODO: Separate render code from game logic
            // We don't need the AI data structures to be renderable
            std::vector<vl::ref<vl::Geometry>> m_geom;
            vl::ref<vl::Transform> m_transform;
            vl::ref<vl::Effect> m_effect;

        };
    }
}


