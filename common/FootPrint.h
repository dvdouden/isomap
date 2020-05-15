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

            ~FootPrint();

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

        struct Area {
            uint32_t x = 0;
            uint32_t y = 0;
            uint32_t w = 0;
            uint32_t h = 0;
            const FootPrint* footPrint = nullptr;

            Area( uint32_t x, uint32_t y, uint32_t w, uint32_t h ) :
                    x( x ),
                    y( y ),
                    w( w ),
                    h( h ),
                    footPrint( nullptr ) { }

            Area( uint32_t x, uint32_t y, const FootPrint* footPrint ) :
                    x( x ),
                    y( y ),
                    w( footPrint->width() ),
                    h( footPrint->height() ),
                    footPrint( footPrint ) { }

            Area() = default;

            Area( const Area& ) = default;

            ~Area() = default;

            Area& operator=( const Area& ) = default;

            bool contains( uint32_t x, uint32_t y ) const {
                if ( x >= this->x && x < this->x + w && y >= this->y && y < this->y + h ) {
                    if ( footPrint != nullptr ) {
                        return footPrint->get( x - this->x, y - this->y ) != 0;
                    }
                    return true;
                }
                return false;
            }
        };

    }
}


