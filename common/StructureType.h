#pragma once

#include "../common/types.h"

namespace isomap {
    namespace common {
        class StructureType {
        public:
            ~StructureType() {
                delete[] m_footPrint;
            }

            StructureType( const StructureType& ) = delete;

            StructureType& operator=( const StructureType& ) = delete;

            id_t id() const {
                return m_id;
            }

            uint32_t width() const {
                return m_width;
            }

            uint32_t height() const {
                return m_height;
            }

            uint8_t* footPrint() const {
                return m_footPrint;
            }

            static StructureType* get( id_t id );

            static void load();

        private:
            explicit StructureType( id_t id, uint32_t width, uint32_t height,
                                    std::initializer_list<uint8_t> footPrint );

            id_t m_id;
            uint32_t m_width = 0;
            uint32_t m_height = 0;
            uint8_t* m_footPrint = nullptr;
        };
    }
}