#include "MatchMessage.h"
#include "PlayerMessage.h"
#include "TerrainMessage.h"

namespace isomap {
    namespace common {

        MatchClientMessage*
        MatchClientMessage::registerPlayer( id_t id, const std::string& name ) {
            auto* msg = new MatchClientMessage( RegisterPlayer, id );
            msg->m_name = name;
            return msg;
        }

        MatchClientMessage* MatchClientMessage::unregisterPlayer( id_t id ) {
            auto* msg = new MatchClientMessage( UnRegisterPlayer, id );
            return msg;
        }

        MatchClientMessage* MatchClientMessage::startMatch( id_t id ) {
            auto* msg = new MatchClientMessage( StartMatch, id );
            return msg;
        }

        MatchClientMessage* MatchClientMessage::playerCommand( id_t id, PlayerCommandMessage* playerMsg ) {
            auto* msg = new MatchClientMessage( PlayerCommand, id );
            msg->m_playerCommandMessage = playerMsg;
            return msg;
        }

        MatchServerMessage*
        MatchServerMessage::playerAccepted( const std::string& name ) {
            auto* msg = new MatchServerMessage( PlayerAccepted );
            msg->m_name = name;
            return msg;
        }

        MatchServerMessage*
        MatchServerMessage::playerRejected( const std::string& name ) {
            auto* msg = new MatchServerMessage( PlayerRejected );
            return msg;
        }

        MatchServerMessage*
        MatchServerMessage::playerJoined( id_t joinedId, const std::string& joinedName ) {
            auto* msg = new MatchServerMessage( PlayerJoined );
            msg->m_id = joinedId;
            msg->m_name = joinedName;
            return msg;
        }

        MatchServerMessage*
        MatchServerMessage::playerLeft( id_t leftId ) {
            auto* msg = new MatchServerMessage( PlayerLeft );
            msg->m_id = leftId;
            return msg;
        }


        MatchServerMessage*
        MatchServerMessage::matchStarted() {
            auto* msg = new MatchServerMessage( MatchStarted );
            return msg;
        }


        MatchServerMessage*
        MatchServerMessage::matchUpdate( uint32_t time ) {
            auto* msg = new MatchServerMessage( MatchUpdate );
            msg->m_time = time;
            return msg;
        }

        MatchServerMessage*
        MatchServerMessage::playerMsg( id_t id, PlayerServerMessage* playerMsg ) {
            auto* msg = new MatchServerMessage( PlayerMessage );
            msg->m_id = id;
            msg->m_playerServerMessage = playerMsg;
            return msg;
        }

        MatchServerMessage* MatchServerMessage::terrainMsg( class TerrainMessage* terrainMsg ) {
            auto* msg = new MatchServerMessage( TerrainMessage );
            msg->m_terrainMessage = terrainMsg;
            return msg;
        }

        MatchServerMessage* MatchServerMessage::clone() {
            return new MatchServerMessage( *this );
        }

        MatchServerMessage::MatchServerMessage( const MatchServerMessage& rhs ) :
                m_type( rhs.m_type ),
                m_id( rhs.m_id ),
                m_name( rhs.m_name ),
                m_time( rhs.m_time ),
                m_playerServerMessage( rhs.m_playerServerMessage ) {
            if ( m_playerServerMessage != nullptr ) {
                m_playerServerMessage = new PlayerServerMessage( *m_playerServerMessage );
            }
            if ( m_terrainMessage != nullptr ) {
                m_terrainMessage = new class TerrainMessage( *m_terrainMessage );
            }
        }

    }
}

