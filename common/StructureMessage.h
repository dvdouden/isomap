#pragma once

#include "types.h"
#include "StructureData.h"

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
                Completed,
                Status,
            };

            explicit StructureServerMessage( Type type ) :
                    m_type( type ) { }

            StructureServerMessage( const StructureServerMessage& ) = default;

            Type type() const {
                return m_type;
            }

            const StructureData& data() {
                return m_data;
            }

            static StructureServerMessage* completedMsg( const StructureData& data );

            static StructureServerMessage* statusMsg( const StructureData& data );

        private:
            Type m_type;

            StructureData m_data;

        };

    }
}


