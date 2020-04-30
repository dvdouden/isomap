#pragma once

#include <vector>
#include "../common/types.h"

namespace isomap {
    namespace client {
        class Match {
        public:
            Match();

            ~Match();

            Match( const Match& ) = delete;

            const Match& operator=( const Match& ) = delete;

        private:
            uint32_t m_time = 0;
            std::vector<Player*> m_players;

        };
    }
}


