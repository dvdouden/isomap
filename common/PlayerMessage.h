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
                Status,
                StructureCreated,
                UnitCreated,
            };

            explicit PlayerServerMessage( Type type ) :
                    m_type( type ) { }

            Type type() const {
                return m_type;
            }

            static PlayerServerMessage* statusMsg();

            static PlayerServerMessage* structureCreatedMsg( uint32_t x, uint32_t y, uint32_t z, id_t id );

            static PlayerServerMessage* unitCreatedMsg( uint32_t x, uint32_t y, uint32_t z, id_t id );

            uint32_t x() const {
                return m_x;
            }

            uint32_t y() const {
                return m_y;
            }

            uint32_t z() const {
                return m_z;
            }

            id_t id() const {
                return m_id;
            }

        private:
            Type m_type;

            uint32_t m_x;
            uint32_t m_y;
            uint32_t m_z;

            id_t m_id;
        };

    }
}


