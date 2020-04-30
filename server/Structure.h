#pragma once

#include "Object.h"
#include "../common/StructureData.h"
#include "../common/StructureType.h"

namespace isomap {
    namespace server {

        class Structure : public Object {
        public:
            Structure( Player* owner, uint32_t x, uint32_t y, uint32_t z, common::StructureType* structureType,
                       uint32_t orientation ) :
                    Object( owner ),
                    m_data( {id(), structureType->id(), x, y, z, orientation, 0} ),
                    m_structureType( structureType ) {
            }

            ~Structure() override = default;

            Structure( const Structure& ) = delete;

            const Structure& operator=( const Structure& ) = delete;

            common::PlayerServerMessage* update( Terrain* world ) override;


            void processMessage( common::StructureCommandMessage* msg );

            common::StructureServerMessage* statusMessage();

            common::StructureServerMessage* createMessage();

            uint32_t x() const {
                return m_data.x;
            }

            uint32_t y() const {
                return m_data.y;
            }

            uint32_t z() const {
                return m_data.z;
            }

            uint32_t constructionProcess() const {
                return m_data.constructionProgress;
            }

            const common::StructureData& data() const {
                return m_data;
            }

        private:
            common::StructureData m_data;

            common::StructureType* m_structureType = nullptr;
            // position
            // health
            // type
            // state
        };
    }
}


