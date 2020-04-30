#pragma once

#include "types.h"
#include "FootPrint.h"

namespace isomap {
    namespace common {
        class StructureType {
        public:
            ~StructureType() = default;

            StructureType( const StructureType& ) = delete;

            StructureType& operator=( const StructureType& ) = delete;

            id_t id() const {
                return m_id;
            }

            const std::string& name() const {
                return m_name;
            }

            uint32_t width( uint32_t rotation ) const {
                return m_footPrint[rotation % 4u]->width();
            }

            uint32_t height( uint32_t rotation ) const {
                return m_footPrint[rotation % 4u]->height();
            }

            FootPrint* footPrint( uint32_t rotation ) const {
                return m_footPrint[rotation % 4u];
            }

            uint8_t footPrint( uint32_t rotation, uint32_t x, uint32_t y ) const {
                return m_footPrint[rotation % 4u]->get( x, y );
            }

            static StructureType* get( id_t id );

            static void load();

        private:
            StructureType( id_t id, std::string name, FootPrint* footPrint );

            id_t m_id;
            FootPrint* m_footPrint[4];
            std::string m_name;
        };
    }
}