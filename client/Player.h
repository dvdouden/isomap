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

            void processMessage( common::StructureServerMessage* msg );

            void processMessage( common::UnitServerMessage* msg );

            common::PlayerCommandMessage*
            buildStructure( int32_t tileX, int32_t tileY, common::StructureType* structureType, uint32_t rotation );

            common::PlayerCommandMessage* buildUnit( int32_t tileX, int32_t tileY );

            void initRender( vl::RenderingAbstract* rendering ) {
                m_rendering = rendering;
            }

            bool
            canPlace( int32_t worldX, int32_t worldY, common::StructureType* structureType, uint32_t rotation ) const;

            void render();

        private:

            Terrain* m_terrain = nullptr;

            std::map<id_t, Structure*> m_structures;
            std::map<id_t, Unit*> m_units;

            vl::RenderingAbstract* m_rendering = nullptr;
        };
    }
}


