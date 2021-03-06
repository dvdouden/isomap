#pragma once

#include "types.h"

namespace isomap {
    namespace common {


        class PlayerCommandMessage {
        public:
            enum Type {
                BuildStructure,
                BuildUnit,
                UnitCommand,
            };

            explicit PlayerCommandMessage( Type type ) :
                    m_type( type ) { }

            ~PlayerCommandMessage();

            const char* typeName();

            Type type() const {
                return m_type;
            }

            static PlayerCommandMessage*
            buildStructureMsg( uint32_t x, uint32_t y, uint32_t z, id_t structureType, uint32_t orientation );

            static PlayerCommandMessage*
            buildUnitMsg( uint32_t x, uint32_t y, uint32_t z, id_t unitType, uint32_t orientation );

            static PlayerCommandMessage* unitCommandMsg( id_t id, UnitCommandMessage* msg );

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

            UnitCommandMessage* unitCommandMessage() const {
                return m_unitCommandMessage;
            }

        private:
            Type m_type;

            id_t m_id = 0;
            uint32_t m_x = 0;
            uint32_t m_y = 0;
            uint32_t m_z = 0;
            uint32_t m_orientation = 0;
            UnitCommandMessage* m_unitCommandMessage = nullptr;
        };


        class PlayerServerMessage {

        public:
            enum Type {
                Status,
                BuildStructureAccepted,
                BuildStructureRejected,
                StructureVisible,
                StructureInvisible,
                StructureDestroyed,
                StructureMessage,
                UnitCreated,
                UnitVisible,
                UnitInvisible,
                UnitDestroyed,
                UnitMessage,
            };

            explicit PlayerServerMessage( Type type ) :
                    m_type( type ) { }

            PlayerServerMessage( const PlayerServerMessage& rhs );

            ~PlayerServerMessage();

            const char* typeName() const;

            Type type() const {
                return m_type;
            }

            static PlayerServerMessage* statusMsg( uint32_t credits, uint32_t maxCredits );

            static PlayerServerMessage*
            buildStructureAcceptedMsg( const StructureData& structureData );

            static PlayerServerMessage*
            buildStructureRejectedMsg( uint32_t x, uint32_t y, uint32_t z, id_t typeId, uint32_t rotation );

            static PlayerServerMessage*
            structureVisibleMsg( const StructureData& structureData );

            static PlayerServerMessage*
            structureInvisibleMsg( id_t id );

            static PlayerServerMessage*
            structureDestroyedMsg( id_t id );

            static PlayerServerMessage* unitCreatedMsg( const UnitData& unitData, id_t structureId );

            static PlayerServerMessage* unitVisibleMsg( const UnitData& unitData );

            static PlayerServerMessage* unitInvisibleMsg( id_t id );

            static PlayerServerMessage* unitDestroyedMsg( id_t id );

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

            uint32_t credits() const {
                return m_credits;
            }

            uint32_t maxCredits() const {
                return m_maxCredits;
            }

            StructureServerMessage* structureMessage() const {
                return m_structureMessage;
            }

            UnitServerMessage* unitMessage() const {
                return m_unitMessage;
            }

            StructureData* structureData() const {
                return m_structureData;
            }

            UnitData* unitData() const {
                return m_unitData;
            }


        private:
            Type m_type;

            uint32_t m_x = 0;
            uint32_t m_y = 0;
            uint32_t m_z = 0;
            uint32_t m_orientation = 0;

            id_t m_id = 0;
            id_t m_typeId = 0;
            uint32_t m_credits = 0;
            uint32_t m_maxCredits = 0;

            StructureData* m_structureData = nullptr;
            UnitData* m_unitData = nullptr;
            StructureServerMessage* m_structureMessage = nullptr;
            UnitServerMessage* m_unitMessage = nullptr;
        };

    }
}


