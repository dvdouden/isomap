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

            StructureServerMessage( const StructureServerMessage& ) = default;

            Type type() const {
                return m_type;
            }

            id_t id() const {
                return m_id;
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

            int32_t constructionProgress() const {
                return m_constructionProgress;
            }

            static StructureServerMessage* createMsg( id_t id, int32_t x, int32_t y, int32_t z );

            static StructureServerMessage*
            statusMsg( id_t id, int32_t x, int32_t y, int32_t z, int32_t constructionProgress );

        private:
            Type m_type;

            id_t m_id = 0;
            int32_t m_x = 0;
            int32_t m_y = 0;
            int32_t m_z = 0;
            int32_t m_constructionProgress = 0;

        };

    }
}


