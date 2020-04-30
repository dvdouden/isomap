#pragma once

#include "Object.h"

namespace isomap {
    namespace server {

        class Structure : public Object {
        public:
            Structure( Player* owner, uint32_t x, uint32_t y, uint32_t z, common::StructureType* structureType ) :
                    Object( owner ),
                    m_x( x ),
                    m_y( y ),
                    m_z( z ),
                    m_structureType( structureType ) { }

            ~Structure() override = default;

            Structure( const Structure& ) = delete;

            const Structure& operator=( const Structure& ) = delete;

            common::PlayerServerMessage* update( Terrain* world ) override;


            void processMessage( common::StructureCommandMessage* msg );

            common::StructureServerMessage* statusMessage();

            common::StructureServerMessage* createMessage();

            int32_t x() const {
                return m_x;
            }

            int32_t y() const {
                return m_y;
            }

            int32_t z() const {
                return m_z;
            }

            int32_t constructionProcess() const {
                return m_constructionProgress;
            }

        private:

            int32_t m_x = 0;
            int32_t m_y = 0;
            int32_t m_z = 0;
            int32_t m_constructionProgress = 0;
            common::StructureType* m_structureType = nullptr;
            // position
            // health
            // type
            // state
        };
    }
}


