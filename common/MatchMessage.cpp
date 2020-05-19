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

        MatchClientMessage::~MatchClientMessage() {
            delete m_playerCommandMessage;
        }

        const char* MatchClientMessage::typeName() {
            switch ( m_type ) {
                case RegisterPlayer:
                    return "RegisterPlayer";
                case UnRegisterPlayer:
                    return "UnregisterPlayer";
                case StartMatch:
                    return "StartMatch";
                case PlayerCommand:
                    return "PlayerCommand";
            }
            return "Invalid";
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

        MatchServerMessage* MatchServerMessage::initTerrain( uint32_t width, uint32_t height ) {
            auto* msg = new MatchServerMessage( InitTerrain );
            msg->m_terrainMessage = TerrainMessage::createMsg( width, height );
            return msg;
        }

        MatchServerMessage*
        MatchServerMessage::matchStarted( uint32_t startCredits, uint32_t maxCredits ) {
            auto* msg = new MatchServerMessage( MatchStarted );
            msg->m_startCredits = startCredits;
            msg->m_maxCredits = maxCredits;
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

        MatchServerMessage::~MatchServerMessage() {
            delete m_playerServerMessage;
            delete m_terrainMessage;
        }

        const char* MatchServerMessage::typeName() {
            switch ( m_type ) {
                case PlayerAccepted:
                    return "PlayerAccepted";
                case PlayerRejected:
                    return "PlayerRejected";
                case PlayerJoined:
                    return "PlayerJoined";
                case PlayerLeft:
                    return "PlayerLeft";
                case InitTerrain:
                    return "InitTerrain";
                case MatchStarted:
                    return "MatchStarted";
                case MatchUpdate:
                    return "MatchUpdate";
                case PlayerMessage:
                    return "PlayerMessage";
                case TerrainMessage:
                    return "TerrainMessage";
            }
            return "Invalid";
        }

    }
}

