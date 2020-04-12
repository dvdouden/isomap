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

            void generateWorld( uint32_t width, uint32_t height );

            void generateWorld( uint32_t width, uint32_t height, TerrainGenerator* generator );

            void addPlayer( Player* player );

            void addObject( Object* object );

            void start();

            void update();

            Terrain* terrain() {
                return m_terrain;
            }

        private:
            uint32_t m_time = 0;
            Terrain* m_terrain = nullptr;
            std::map<id_t, Object*> m_objects;
            std::vector<Player*> m_players;
        };

    }
}
