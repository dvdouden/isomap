#pragma once

#include <map>
#include <string>
#include <vector>
#include <vlGraphics/RenderingAbstract.hpp>
#include "../common/MatchMessage.h"
#include "../common/types.h"

namespace isomap {
    namespace client {
        class Match {
        public:
            Match( id_t connectionID );

            ~Match();

            Match( const Match& ) = delete;

            const Match& operator=( const Match& ) = delete;

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

            void initRender( vl::RenderingAbstract* rendering );

            void render();

            void disableRendering();

            void enableRendering();

            void enqueueMessage( common::MatchClientMessage* msg ) {
                m_messages.push_back( msg );
            }

            void enqueueMessage( common::PlayerCommandMessage* msg ) {
                m_messages.push_back( common::MatchClientMessage::playerCommand( m_connectionId, msg ) );
            }

            std::vector<common::MatchClientMessage*> clientMessages() {
                std::vector<common::MatchClientMessage*> ret = std::move( m_messages );
                return ret;
            }

            Terrain* terrain() {
                return m_terrain;
            }

            void dumpActors();

        private:
            Player* getPlayer( id_t id );

            id_t m_connectionId = 0;
            uint32_t m_time = 0;
            bool m_started = false;
            Player* m_player = nullptr;
            std::map<id_t, Player*> m_players;
            Terrain* m_terrain = nullptr;

            std::vector<common::MatchClientMessage*> m_messages;

        };
    }
}


