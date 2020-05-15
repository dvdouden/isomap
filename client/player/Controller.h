#pragma once

#include "../../common/types.h"

namespace isomap {
    namespace client {
        namespace player {

            class Controller {
            public:
                explicit Controller( Player* player ) :
                        m_player( player ) {
                }

                virtual ~Controller() = default;

                Controller( const Controller& ) = delete;

                Controller& operator=( const Controller& ) = delete;

                virtual void update() = 0;

                virtual void onUnitCreated( Unit* unit ) = 0;

                virtual void onStructureCreated( Structure* structure ) = 0;

                Player* player() {
                    return m_player;
                }

            private:
                Player* m_player;
            };

        }
    }
}
