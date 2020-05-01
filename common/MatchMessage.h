#pragma once

#include <string>
#include "types.h"

namespace isomap {
    namespace common {
        class MatchClientMessage {
        public:
            enum Type {
                RegisterPlayer,
                UnRegisterPlayer,
                StartMatch,
                PlayerCommand,
            };

            static MatchClientMessage* registerPlayer( id_t id, const std::string& name );

            static MatchClientMessage* unregisterPlayer( id_t id );

            static MatchClientMessage* startMatch( id_t id );

            static MatchClientMessage* playerCommand( id_t id, PlayerCommandMessage* msg );

            Type type() const {
                return m_type;
            }

            id_t id() const {
                return m_id;
            }

            const std::string& name() const {
                return m_name;
            }

            PlayerCommandMessage* playerMsg() {
                return m_playerCommandMessage;
            }

        private:
            explicit MatchClientMessage( Type type, id_t id ) :
                    m_type( type ),
                    m_id( id ) { }

            Type m_type;

            id_t m_id;
            std::string m_name;
            PlayerCommandMessage* m_playerCommandMessage = nullptr;
        };

        class MatchServerMessage {
        public:
            enum Type {
                PlayerAccepted,
                PlayerRejected,
                PlayerJoined,
                PlayerLeft,
                MatchStarted,
                MatchUpdate,
                PlayerMessage,
                TerrainMessage,
            };


            static MatchServerMessage* playerAccepted( const std::string& name );

            static MatchServerMessage* playerRejected( const std::string& name );

            static MatchServerMessage* playerJoined( id_t joinedId, const std::string& joinedName );

            static MatchServerMessage* playerLeft( id_t leftId );

            static MatchServerMessage* matchStarted();

            static MatchServerMessage* matchUpdate( uint32_t time );

            static MatchServerMessage* playerMsg( id_t id, PlayerServerMessage* msg );

            static MatchServerMessage* terrainMsg( class TerrainMessage* msg );

            Type type() const {
                return m_type;
            }

            id_t id() const {
                return m_id;
            }

            const std::string& name() const {
                return m_name;
            }

            uint32_t time() const {
                return m_time;
            }

            PlayerServerMessage* playerMsg() {
                return m_playerServerMessage;
            }

            class TerrainMessage* terrainMsg() {
                return m_terrainMessage;
            }

        private:
            explicit MatchServerMessage( Type type ) :
                    m_type( type ) { }

            Type m_type;

            id_t m_id;
            std::string m_name;
            uint32_t m_time = 0;

            PlayerServerMessage* m_playerServerMessage = nullptr;

            class TerrainMessage* m_terrainMessage = nullptr;
        };

    }
}


