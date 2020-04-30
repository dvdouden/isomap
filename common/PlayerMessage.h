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

            static PlayerCommandMessage*
            buildStructureMsg( uint32_t x, uint32_t y, uint32_t z, id_t structureType, uint32_t rotation );

            static PlayerCommandMessage* buildUnitMsg( uint32_t x, uint32_t y, uint32_t z );

            id_t id() const {
                return m_id;
            }

            uint32_t x() const {
                return m_x;
            }

            uint32_t y() const {
                return m_y;
            }

            uint32_t z() const {
                return m_z;
            }

            uint32_t orientation() const {
                return m_orientation;
            }

        private:
            Type m_type;

            id_t m_id;
            uint32_t m_x;
            uint32_t m_y;
            uint32_t m_z;
            uint32_t m_orientation;
        };


        class PlayerServerMessage {

        public:
            enum Type {
                PlayerRegistered,
                PlayerRejected,
                Status,
                BuildStructureAccepted,
                BuildStructureRejected,
                UnitCreated,
                UnitMessage,
                StructureMessage,
            };

            explicit PlayerServerMessage( Type type ) :
                    m_type( type ) { }

            ~PlayerServerMessage() = default;

            Type type() const {
                return m_type;
            }

            static PlayerServerMessage* statusMsg();

            static PlayerServerMessage*
            buildStructureAcceptedMsg( uint32_t x, uint32_t y, uint32_t z, id_t id, id_t typeId, uint32_t rotation );

            static PlayerServerMessage*
            buildStructureRejectedMsg( uint32_t x, uint32_t y, uint32_t z, id_t typeId, uint32_t rotation );

            static PlayerServerMessage* unitCreatedMsg( uint32_t x, uint32_t y, uint32_t z, id_t id );

            static PlayerServerMessage* structureMsg( StructureServerMessage* msg );

            static PlayerServerMessage* unitMsg( UnitServerMessage* msg );

            uint32_t x() const {
                return m_x;
            }

            uint32_t y() const {
                return m_y;
            }

            uint32_t z() const {
                return m_z;
            }

            uint32_t orientation() const {
                return m_orientation;
            }

            id_t id() const {
                return m_id;
            }

            id_t typeId() const {
                return m_typeId;
            }

            StructureServerMessage* structureMessage() const {
                return m_structureMessage;
            }

            UnitServerMessage* unitMessage() const {
                return m_unitMessage;
            }

        private:
            Type m_type;

            uint32_t m_x;
            uint32_t m_y;
            uint32_t m_z;
            uint32_t m_orientation;

            id_t m_id;
            id_t m_typeId;

            StructureServerMessage* m_structureMessage = nullptr;
            UnitServerMessage* m_unitMessage = nullptr;
        };

    }
}


