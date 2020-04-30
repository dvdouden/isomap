#pragma once

#include <initializer_list>
#include <cstdio>
#include "../common/types.h"

namespace isomap {
    namespace common {

        class FootPrint {
        public:
            FootPrint( uint32_t width, uint32_t height );

            FootPrint( uint32_t width, uint32_t height, std::initializer_list<uint8_t> map );

            ~FootPrint() {
                delete[] m_map;
            }

            FootPrint( const FootPrint& ) = delete;

            FootPrint& operator=( const FootPrint& ) = delete;

            uint32_t width() const {
                return m_width;
            }

            uint32_t height() const {
                return m_height;
            }

            uint8_t get( uint32_t x, uint32_t y ) const {
                return m_map[y * m_width + x];
            }

            FootPrint* rotate() const;

        private:
            uint32_t m_width = 0;
            uint32_t m_height = 0;
            uint8_t* m_map = nullptr;

        };

    }
}


