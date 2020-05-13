#pragma once

#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/RenderingAbstract.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>

#include "../common/StructureData.h"
#include "../common/StructureType.h"
#include "../common/types.h"


namespace isomap {
    namespace client {
        class Structure {
        public:
            Structure( Player* player, const common::StructureData& data ) :
                    m_player( player ),
                    m_data( data ),
                    m_type( common::StructureType::get( data.typeId ) ) {
            };

            ~Structure();

            Structure( const Structure& ) = delete;

            const Structure& operator=( const Structure& ) = delete;

            void processMessage( common::StructureServerMessage* msg );

            void initRender( vl::RenderingAbstract* rendering, vl::SceneManagerActorTree* sceneManager );

            void render();

            bool occupies( uint32_t x, uint32_t y ) const;

            common::StructureType* type() const {
                return m_type;
            }

            common::FootPrint* footPrint() const {
                return m_type->footPrint( m_data.orientation );
            }

            Player* player() const {
                return m_player;
            }

            const common::StructureData& data() const {
                return m_data;
            }

            void setVisible( bool visible );

            bool visible() const {
                return m_visible;
            }

            id_t id() const {
                return m_data.id;
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

            uint32_t orientation() const {
                return m_data.orientation;
            }


        private:
            Player* m_player;
            common::StructureData m_data;
            common::StructureType* m_type = nullptr;
            bool m_visible = true;

            // TODO: Separate render code from game logic
            // We don't need the AI data structures to be renderable
            vl::ref<vl::Transform> m_transform;
            vl::ref<vl::Effect> m_effect;
            std::vector<vl::Actor*> m_actors;
        };
    }
}


