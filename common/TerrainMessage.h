#pragma once

#include <vector>
#include "types.h"

namespace isomap {
    namespace common {
        class TerrainMessage {
        public:
            enum Type {
                Create = 0,
                Update = 1
            };

            explicit TerrainMessage( Type type ) :
                    m_type( type ) { }

            Type type() const {
                return m_type;
            }

            struct Cell {
                uint32_t id;
                uint8_t height;
                uint8_t slope;
                uint8_t ore;
            };

            static TerrainMessage* createMsg( uint32_t width, uint32_t height );

            static TerrainMessage* updateMsg( std::vector<Cell>& cells );

            //static TerrainMessage* unmarshal( uint8_t* buffer, uint32_t size );

            //static uint32_t marshal( TerrainMessage* message, uint8_t*& buffer, uint32_t& size );

            uint32_t width() const {
                return m_width;
            }

            uint32_t height() const {
                return m_height;
            }

            const std::vector<Cell>& cells() const {
                return m_cells;
            }

        private:
            Type m_type;
            uint32_t m_width = 0;
            uint32_t m_height = 0;
            std::vector<Cell> m_cells;

        };
    }
}
