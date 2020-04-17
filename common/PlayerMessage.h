#pragma once
#include "types.h"

namespace isomap {
    namespace common {


        class PlayerCommandMessage {
        public:
            enum Type {
                BuildStructure,
                BuildUnit,
            };

            explicit PlayerCommandMessage( Type type ) :
                    m_type( type ) { }

            Type type() const {
                return m_type;
            }

            static PlayerCommandMessage* buildStructureMsg( uint32_t x, uint32_t y, uint32_t z );

            static PlayerCommandMessage* buildUnitMsg( uint32_t x, uint32_t y, uint32_t z );

            uint32_t x() const {
                return m_x;
            }

            uint32_t y() const {
                return m_y;
            }

            uint32_t z() const {
                return m_z;
            }

        private:
            Type m_type;

            uint32_t m_x;
            uint32_t m_y;
            uint32_t m_z;
        };


        class PlayerServerMessage {

        public:
            enum Type {
                Status
            };

            explicit PlayerServerMessage( Type type ) :
                    m_type( type ) { }

            Type type() const {
                return m_type;
            }

            static PlayerServerMessage* statusMsg();

        private:
            Type m_type;
        };

    }
}


