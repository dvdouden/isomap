#pragma once

#include "Object.h"

namespace isomap {
    namespace server {

        class Structure : public Object {
        public:
            Structure( Player* owner, uint32_t x, uint32_t y, uint32_t z ) :
                    Object( owner ),
                    m_x( x ),
                    m_y( y ),
                    m_z( z ){ }

            ~Structure() override = default;

            Structure( const Structure& ) = delete;

            const Structure& operator=( const Structure& ) = delete;

            bool update( Terrain* world ) override;


            void processMessage( common::StructureCommandMessage* msg );

            common::StructureServerMessage* statusMessage();
            common::StructureServerMessage* createMessage();

            int32_t getX() const {
                return m_x;
            }

            int32_t getY() const {
                return m_x;
            }

            int32_t getZ() const {
                return m_x;
            }

        private:

            int32_t m_x = 0;
            int32_t m_y = 0;
            int32_t m_z = 0;
            // position
            // health
            // type
            // state
        };
    }
}


