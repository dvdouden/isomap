#include <list>
#include <cassert>
#include "FootPrint.h"


namespace isomap {
    namespace common {
        FootPrint::FootPrint( uint32_t width, uint32_t height ) :
                m_width( width ),
                m_height( height ) {
            m_map = new uint8_t[m_width * m_height]();
        }

        FootPrint::FootPrint( uint32_t width, uint32_t height, std::initializer_list<uint8_t> map ) :
                m_width( width ),
                m_height( height ) {
            m_map = new uint8_t[m_width * m_height];
            assert( map.size() == (m_width * m_height) );
            std::copy( map.begin(), map.end(), m_map );
        }

        FootPrint* FootPrint::rotate() const {
            // create new footprint, rotated 90 degrees ccw
            FootPrint* f = new FootPrint( m_height, m_width );
            for ( uint32_t y = 0; y < m_height; ++y ) {
                for ( uint32_t x = 0; x < m_width; ++x ) {
                    f->m_map[(m_width - x - 1) * m_height + y] = m_map[y * m_width + x];
                }
            }
            return f;
        }
    }
}
