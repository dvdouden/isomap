#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../common/MatchMessage.h"
#include "../common/PlayerMessage.h"
#include "../common/types.h"
#include "match/Renderer.h"

namespace isomap {
    namespace client {
        class Match {
        public:
            explicit Match( id_t connectionID );

            ~Match() = default;

            Match( const Match& ) = delete;

            const Match& operator=( const Match& ) = delete;

            void setRenderer( match::Renderer* renderer ) {
                m_renderer.reset( renderer );
            }

            match::Renderer* renderer() const {
                return m_renderer.get();
            }

            void processMessage( common::MatchServerMessage* msg );

            void update();

            void registerPlayer( const std::string& name );

            void unregisterPlayer();

            void startMatch();

            Player* player() {
                return m_player;
            }

            id_t id() const {
                return m_connectionId;
            }

            void enqueueMessage( common::MatchClientMessage* msg ) {
                //printf( "%08X enqueue match client message of type %s\n", m_connectionId, msg->typeName() );
                m_messages.push_back( msg );
            }

            void enqueueMessage( common::PlayerCommandMessage* msg ) {
                //printf( "%08X enqueue player command message of type %s\n", m_connectionId, msg->typeName() );
                m_messages.push_back( common::MatchClientMessage::playerCommand( m_connectionId, msg ) );
            }

            std::vector<common::MatchClientMessage*> clientMessages() {
                std::vector<common::MatchClientMessage*> ret = std::move( m_messages );
                return ret;
            }

            Terrain* terrain() {
                return m_terrain.get();
            }

            std::map<id_t, std::unique_ptr<Player>>& players() {
                return m_players;
            }

        private:
            Player* getPlayer( id_t id );

            id_t m_connectionId = 0;
            uint32_t m_time = 0;
            bool m_started = false;
            Player* m_player = nullptr;
            std::map<id_t, std::unique_ptr<Player>> m_players;
            std::unique_ptr<Terrain> m_terrain;

            std::vector<common::MatchClientMessage*> m_messages;

            std::unique_ptr<match::Renderer> m_renderer;
        };
    }
}


