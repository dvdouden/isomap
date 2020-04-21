#pragma once

#include <map>
#include "../common/types.h"
#include <vlGraphics/RenderingAbstract.hpp>


namespace isomap {
    namespace client {
        class Player {
        public:
            explicit Player( Terrain* terrain ) :
                    m_terrain( terrain ) { }

            ~Player() = default;

            Player( const Player& ) = delete;

            const Player& operator=( const Player& ) = delete;

            void processMessage( common::PlayerServerMessage* msg );

            common::PlayerCommandMessage* buildStructure( int32_t tileX, int32_t tileY );

            common::PlayerCommandMessage* buildUnit( int32_t tileX, int32_t tileY );

            void initRender( vl::RenderingAbstract* rendering ) {
                m_rendering = rendering;
            }

            void render();

        private:

            Terrain* m_terrain = nullptr;

            std::map<id_t, Structure*> m_structures;
            std::map<id_t, Unit*> m_units;

            vl::RenderingAbstract* m_rendering = nullptr;
        };
    }
}


