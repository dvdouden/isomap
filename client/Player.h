#pragma once
#include "../common/types.h"


namespace isomap {
    namespace client {
        class Player {
        public:
            Player() = default;

            ~Player() = default;

            Player( const Player& ) = delete;

            const Player& operator=( const Player& ) = delete;

            void processMessage( common::PlayerServerMessage* msg );

            common::PlayerCommandMessage* buildStructure( int32_t tileX, int32_t tileY );

            common::PlayerCommandMessage* buildUnit( int32_t tileX, int32_t tileY );

        private:

            Terrain* m_terrain;

        };
    }
}


