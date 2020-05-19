#pragma once

#include "../common/StructureData.h"
#include "../common/StructureMessage.h"
#include "../common/StructureType.h"
#include "types.h"

#include "structure/Controller.h"
#include "structure/Renderer.h"


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

            void setController( structure::Controller* controller ) {
                m_controller.reset( controller );
            }

            void setRenderer( structure::Renderer* renderer ) {
                m_renderer.reset( renderer );
            }

            structure::Controller* controller() const {
                return m_controller.get();
            }

            structure::Renderer* renderer() const {
                return m_renderer.get();
            }

            void processMessage( common::StructureServerMessage* msg );

            bool occupies( uint32_t idx ) const;

            bool occupies( uint32_t x, uint32_t y ) const;

            bool isAdjacentTo( uint32_t idx ) const;

            bool isAdjacentTo( uint32_t x, uint32_t y ) const;

            bool dockingTileAt( uint32_t idx ) const;

            bool dockingTileAt( uint32_t x, uint32_t y ) const;

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

            uint32_t constructionProcess() const {
                return m_data.constructionProgress;
            }

            bool constructionCompleted() const {
                return m_data.constructionProgress == 100;
            }

            void dump() const;


        private:
            Player* m_player;
            common::StructureData m_data;
            common::StructureType* m_type = nullptr;
            bool m_visible = true;

            std::unique_ptr<structure::Controller> m_controller;
            std::unique_ptr<structure::Renderer> m_renderer;
        };
    }
}


