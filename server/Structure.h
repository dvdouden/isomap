#pragma once

#include "Object.h"
#include "../common/StructureData.h"
#include "../common/StructureType.h"

namespace isomap {
    namespace server {

        class Structure : public Object {
        public:
            Structure(
                    Player* owner,
                    uint32_t x,
                    uint32_t y,
                    uint32_t z,
                    common::StructureType* structureType,
                    uint32_t orientation );

            ~Structure() override = default;

            Structure( const Structure& ) = delete;

            const Structure& operator=( const Structure& ) = delete;

            common::PlayerServerMessage* update( Terrain* world ) override;

            void processMessage( common::StructureCommandMessage* msg );

            common::StructureServerMessage* completedMessage();

            common::StructureServerMessage* statusMessage();

            void destroy();

            uint32_t x() const {
                return m_data.x;
            }

            uint32_t y() const {
                return m_data.y;
            }

            uint32_t z() const {
                return m_data.z;
            }

            uint32_t orientation() const {
                return m_data.orientation;
            }

            uint32_t constructionProcess() const {
                return m_data.constructionProgress;
            }

            bool constructionCompleted() const {
                return m_data.constructionProgress == 100;
            }

            void constructionTick();

            const common::StructureData& data() const {
                return m_data;
            }

            common::StructureType* type() const {
                return m_type;
            }

            common::FootPrint* footPrint() const {
                return m_type->footPrint( m_data.orientation );
            }

            bool occupies( uint32_t x, uint32_t y ) const;

            bool isAdjacentTo( uint32_t x, uint32_t y ) const;

            Unit* spawnUnit() const;

            void dump() override;

        private:
            common::StructureData m_data;

            common::StructureType* m_type = nullptr;

            bool m_dirty = false;
            // position
            // health
            // type
            // state
        };
    }
}


