#pragma once

#include <map>
#include <vector>

#include "../common/types.h"

namespace isomap {
    namespace server {

        class Match {
        public:
            Match();

            ~Match();

            Match( const Match& ) = delete;

            const Match& operator=( const Match& ) = delete;

            void processMessage( common::MatchClientMessage* msg );

            void generateWorld( uint32_t width, uint32_t height );

            void generateWorld( uint32_t width, uint32_t height, TerrainGenerator* generator );

            void addObject( Object* object );

            void start();

            void update();

            Terrain* terrain() {
                return m_terrain;
            }

            void enqueueMessage( id_t playerId, common::MatchServerMessage* msg ) {
                m_messages[playerId].push_back( msg );
            }

            void enqueueMessageAll( common::MatchServerMessage* msg );

            std::vector<common::MatchServerMessage*> serverMessages( id_t playerId ) {
                std::vector<common::MatchServerMessage*> ret = std::move( m_messages[playerId] );
                return ret;
            }

            Player* getPlayer( id_t id );

        private:

            uint32_t m_time = 0;
            Terrain* m_terrain = nullptr;
            std::map<id_t, Object*> m_objects;
            std::map<id_t, Player*> m_players;
            uint32_t m_minPlayers = 2;
            uint32_t m_maxPlayers = 4;
            bool m_started = false;

            std::map<id_t, std::vector<common::MatchServerMessage*>> m_messages;
        };

    }
}
