#pragma once

#include <map>
#include <memory>
#include <vector>

#include "types.h"
#include "Object.h"
#include "Player.h"
#include "Terrain.h"

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

            void removeObject( Object* object );

            void start();

            void update();

            Terrain* terrain() {
                return m_terrain.get();
            }

            void enqueueMessage( id_t playerId, common::MatchServerMessage* msg ) {
                m_messages[playerId].push_back( msg );
            }

            void enqueueMessage( Object* obj, common::PlayerServerMessage* msg );

            void updateSubscriptions( Structure* structure );

            void updateSubscriptions( Unit* unit );

            std::vector<common::MatchServerMessage*> serverMessages( id_t playerId ) {
                std::vector<common::MatchServerMessage*> ret = std::move( m_messages[playerId] );
                return ret;
            }

            Player* getPlayer( id_t id );

            uint32_t startCredits() const {
                return m_startCredits;
            }

            uint32_t creditLimit() const {
                return m_creditLimit;
            }

            void dump();


        private:

            uint32_t m_time = 0;
            std::unique_ptr<Terrain> m_terrain = nullptr;
            std::map<id_t, std::unique_ptr<Object>> m_objects;
            std::map<id_t, std::unique_ptr<Player>> m_players;
            uint32_t m_minPlayers = 2;
            uint32_t m_maxPlayers = 4;
            bool m_started = false;
            uint32_t m_startCredits = 5000;
            uint32_t m_creditLimit = 25000;

            std::map<id_t, std::vector<common::MatchServerMessage*>> m_messages;
        };

    }
}
