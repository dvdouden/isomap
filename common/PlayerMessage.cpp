#include <cstdio>
#include "PlayerMessage.h"
#include "StructureMessage.h"
#include "UnitMessage.h"

namespace isomap {
    namespace common {

        PlayerCommandMessage*
        PlayerCommandMessage::buildStructureMsg( uint32_t x, uint32_t y, uint32_t z, id_t id, uint32_t rotation ) {
            auto* msg = new PlayerCommandMessage( BuildStructure );
            msg->m_id = id;
            msg->m_x = x;
            msg->m_y = y;
            msg->m_z = z;
            msg->m_orientation = rotation;
            return msg;
        }

        PlayerCommandMessage* PlayerCommandMessage::buildUnitMsg( uint32_t x, uint32_t y, uint32_t z ) {
            auto* msg = new PlayerCommandMessage( BuildUnit );
            msg->m_x = x;
            msg->m_y = y;
            msg->m_z = z;
            return msg;
        }


        PlayerServerMessage* PlayerServerMessage::statusMsg() {
            auto* msg = new PlayerServerMessage( Status );
            return msg;
        }

        PlayerServerMessage*
        PlayerServerMessage::buildStructureAcceptedMsg( const StructureData& structureData ) {
            auto* msg = new PlayerServerMessage( BuildStructureAccepted );
            msg->m_structureData = new StructureData( structureData );
            return msg;
        }

        PlayerServerMessage*
        PlayerServerMessage::buildStructureRejectedMsg( uint32_t x, uint32_t y, uint32_t z, id_t typeId,
                                                        uint32_t rotation ) {
            auto* msg = new PlayerServerMessage( BuildStructureRejected );
            msg->m_x = x;
            msg->m_y = y;
            msg->m_z = z;
            msg->m_typeId = typeId;
            msg->m_orientation = rotation;
            return msg;
        }

        PlayerServerMessage* PlayerServerMessage::unitCreatedMsg( uint32_t x, uint32_t y, uint32_t z, id_t id ) {
            auto* msg = new PlayerServerMessage( UnitCreated );
            msg->m_x = x;
            msg->m_y = y;
            msg->m_z = z;
            msg->m_id = id;
            return msg;
        }


        PlayerServerMessage* PlayerServerMessage::structureMsg( StructureServerMessage* msg ) {
            auto* ret = new PlayerServerMessage( StructureMessage );
            ret->m_structureMessage = msg;
            return ret;
        }

        PlayerServerMessage* PlayerServerMessage::unitMsg( UnitServerMessage* msg ) {
            auto* ret = new PlayerServerMessage( UnitMessage );
            ret->m_unitMessage = msg;
            return ret;
        }

        PlayerServerMessage::PlayerServerMessage( const PlayerServerMessage& rhs ) :
                m_type( rhs.m_type ),
                m_x( rhs.m_x ),
                m_y( rhs.m_y ),
                m_z( rhs.m_z ),
                m_orientation( rhs.m_orientation ),
                m_id( rhs.m_id ),
                m_typeId( rhs.m_typeId ),
                m_structureMessage( rhs.m_structureMessage ),
                m_unitMessage( rhs.m_unitMessage ),
                m_structureData( rhs.m_structureData ) {
            if ( m_structureMessage != nullptr ) {
                m_structureMessage = new StructureServerMessage( *m_structureMessage );
            }
            if ( m_unitMessage != nullptr ) {
                m_unitMessage = new UnitServerMessage( *m_unitMessage );
            }
            if ( m_structureData != nullptr ) {
                m_structureData = new StructureData( *m_structureData );
            }
        }

        PlayerServerMessage*
        PlayerServerMessage::structureVisibleMsg( const StructureData& structureData ) {
            auto* msg = new PlayerServerMessage( StructureVisible );
            msg->m_structureData = new StructureData( structureData );
            return msg;
        }

        PlayerServerMessage* PlayerServerMessage::structureInvisibleMsg( id_t id ) {
            auto* msg = new PlayerServerMessage( StructureInvisible );
            msg->m_id = id;
            return msg;
        }

        PlayerServerMessage* PlayerServerMessage::structureDestroyedMsg( id_t id ) {
            auto* msg = new PlayerServerMessage( StructureDestroyed );
            msg->m_id = id;
            return msg;
        }

    }
}

