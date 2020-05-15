#pragma once

#include <map>
#include <memory>
#include "../common/types.h"
#include "../common/PlayerMessage.h"
#include "Match.h"
#include "Structure.h"
#include "Unit.h"
#include "player/Controller.h"
#include "player/Renderer.h"


namespace isomap {
    namespace client {
        class Player {
        public:
            Player( Match* match, id_t id, std::string name, bool useAi = false );

            ~Player() = default;

            Player( const Player& ) = delete;

            const Player& operator=( const Player& ) = delete;

            id_t id() const {
                return m_id;
            }

            const std::string& name() const {
                return m_name;
            }

            player::Controller* controller() const {
                return m_controller.get();
            }

            void setRenderer( player::Renderer* renderer ) {
                m_renderer.reset( renderer );
            }

            player::Renderer* renderer() const {
                return m_renderer.get();
            }

            void startMatch();

            void processMessage( common::PlayerServerMessage* msg );

            void processMessage( common::StructureServerMessage* msg );

            void processMessage( common::UnitServerMessage* msg );

            void update();

            void
            buildStructure( int32_t tileX, int32_t tileY, common::StructureType* structureType, uint32_t orientation );

            void buildUnit( int32_t tileX, int32_t tileY, common::UnitType* unitType, uint32_t orientation );

            bool
            canPlace( int32_t worldX, int32_t worldY, common::StructureType* structureType, uint32_t rotation ) const;


            Structure* getStructure( id_t id );

            Unit* getUnit( id_t id );

            Match* match() {
                return m_match;
            }

            Terrain* terrain() {
                return m_terrain;
            }

            void enqueueMessage( common::PlayerCommandMessage* msg ) {
                m_match->enqueueMessage( msg );
            }

            void enqueueMessage( id_t id, common::UnitCommandMessage* msg ) {
                enqueueMessage( common::PlayerCommandMessage::unitCommandMsg( id, msg ) );
            }

            std::map<id_t, std::unique_ptr<Structure>>& structures() {
                return m_structures;
            }

            std::map<id_t, std::unique_ptr<Unit>>& units() {
                return m_units;
            }

            void dump() const;

        private:
            Match* m_match;
            id_t m_id;
            std::string m_name;

            Terrain* m_terrain = nullptr;
            // renderer needs to be on on top
            // since the destructor works from the bottom up
            // so units and structures get deleted before the renderer
            // and they will need access to the renderer when they are deleted.
            std::unique_ptr<player::Renderer> m_renderer;
            std::unique_ptr<player::Controller> m_controller;

            std::map<id_t, std::unique_ptr<Structure>> m_structures;
            std::map<id_t, std::unique_ptr<Unit>> m_units;
        };
    }
}


