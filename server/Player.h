#pragma once

#include <vector>
#include "../common/types.h"

namespace isomap {
    namespace server {
        class Player {
        public:
            Player(Match* match);

            ~Player();

            Player( const Player& ) = delete;

            const Player& operator=( const Player& ) = delete;

            void init();

            void setTerrain( Terrain* terrain ) {
                m_terrain = terrain;
            }

            void update();

            void unFog( int32_t tile_x, int32_t tile_y, int32_t radius );

            common::TerrainMessage* createTerrainMessage();

            void processMessage( common::PlayerCommandMessage* msg );

        private:
            uint8_t* m_fogMap = nullptr;
            Terrain* m_terrain = nullptr;
            Match* m_match = nullptr;

            std::vector<uint32_t> m_uncoveredTiles;
        };

    }
}