#pragma once

#include "types.h"

namespace isomap {
    namespace common {


        class StructureCommandMessage {
        public:
            enum Type {
                Move,
            };

            explicit StructureCommandMessage( Type type ) :
                    m_type( type ) { }

            Type type() const {
                return m_type;
            }

            //static StructureCommandMessage* moveMsg(  );

        private:
            Type m_type;
        };


        class StructureServerMessage {

        public:
            enum Type {
                Create,
                Status
            };

            explicit StructureServerMessage( Type type ) :
                    m_type( type ) { }

            Type type() const {
                return m_type;
            }

            int32_t x() const {
                return m_x;
            }

            int32_t y() const {
                return m_y;
            }

            int32_t z() const {
                return m_z;
            }

            static StructureServerMessage* createMsg( int32_t x, int32_t y, int32_t z );

            static StructureServerMessage* statusMsg( int32_t x, int32_t y, int32_t z );

        private:
            Type m_type;

            int32_t m_x;
            int32_t m_y;
            int32_t m_z;

        };

    }
}


