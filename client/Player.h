#pragma once

#include <map>
#include <memory>
#include "../common/types.h"
#include "../common/PlayerMessage.h"
#include "Match.h"
#include "Structure.h"
#include "Unit.h"
#include <vlGraphics/RenderingAbstract.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>


namespace isomap {
    namespace client {
        class Player {
        public:
            Player( Match* match, id_t id, std::string name );

            ~Player() = default;

            Player( const Player& ) = delete;

            const Player& operator=( const Player& ) = delete;

            id_t id() const {
                return m_id;
            }

            const std::string& name() const {
                return m_name;
            }

            void startMatch();

            void processMessage( common::PlayerServerMessage* msg );

            void processMessage( common::StructureServerMessage* msg );

            void processMessage( common::UnitServerMessage* msg );

            void update();

            void
            buildStructure( int32_t tileX, int32_t tileY, common::StructureType* structureType, uint32_t orientation );

            void buildUnit( int32_t tileX, int32_t tileY, common::UnitType* unitType, uint32_t orientation );

            void initRender( vl::RenderingAbstract* rendering );

            void disableRendering();

            void enableRendering();

            bool
            canPlace( int32_t worldX, int32_t worldY, common::StructureType* structureType, uint32_t rotation ) const;

            void render();

            const vl::fvec4& color() const {
                return m_color;
            }

            Structure* getStructure( id_t id );

            Unit* getUnit( id_t id );

            vl::SceneManagerActorTree* sceneManager() {
                return m_sceneManager.get();
            }

            Match* match() {
                return m_match;
            }

            Terrain* terrain() {
                return m_terrain;
            }

            void dumpActors();


            void enqueueMessage( common::PlayerCommandMessage* msg ) {
                m_match->enqueueMessage( msg );
            }

            void enqueueMessage( id_t id, common::UnitCommandMessage* msg ) {
                enqueueMessage( common::PlayerCommandMessage::unitCommandMsg( id, msg ) );
            }

            Structure* getStructureAt( int32_t x, int32_t y );

        private:
            Match* m_match;
            id_t m_id;
            vl::fvec4 m_color;
            std::string m_name;

            Terrain* m_terrain = nullptr;

            std::map<id_t, std::unique_ptr<Structure>> m_structures;
            std::map<id_t, std::unique_ptr<Unit>> m_units;

            vl::RenderingAbstract* m_rendering = nullptr;
            vl::ref<vl::SceneManagerActorTree> m_sceneManager;
        };
    }
}


