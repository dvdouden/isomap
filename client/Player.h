#pragma once

#include <map>
#include <memory>
#include "types.h"
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

            uint32_t credits() const {
                return m_credits;
            }

            uint32_t maxCredits() const {
                return m_maxCredits;
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

            Structure* getStructure( id_t id );

            Unit* getUnit( id_t id );

            Match* match() {
                return m_match;
            }

            Terrain* terrain() {
                return m_terrain;
            }

            std::map<id_t, std::unique_ptr<Structure>>& structures() {
                return m_structures;
            }

            std::map<id_t, std::unique_ptr<Unit>>& units() {
                return m_units;
            }

            void setCredits( uint32_t credits ) {
                m_credits = credits;
            }

            void setMaxCredits( uint32_t maxCredits ) {
                m_maxCredits = maxCredits;
            }

            uint32_t incCredits( uint32_t amount ) {
                uint32_t maxAmount = m_maxCredits - m_credits;
                if ( amount > maxAmount ) {
                    amount = maxAmount;
                }

                if ( amount == 0 ) {
                    return 0;
                }

                m_credits += amount;
                return amount;
            }

            uint32_t decCredits( uint32_t amount ) {
                if ( amount > m_credits ) { ;
                    return false;
                }
                m_credits -= amount;
                return amount;
            }

            void dump() const;

        private:
            Match* m_match;
            id_t m_id;
            std::string m_name;
            uint32_t m_credits;
            uint32_t m_maxCredits;

            Terrain* m_terrain = nullptr;
            // renderer needs to be on on top
            // since the destructor works from the bottom up
            // so units and structures get deleted before the renderer
            // and they will need access to the renderer when they are deleted.
            std::unique_ptr<player::Renderer> m_renderer;

            std::map<id_t, std::unique_ptr<Structure>> m_structures;
            std::map<id_t, std::unique_ptr<Unit>> m_units;

            // controller needs to be cleaned up first since it unregisters itself with the units
            std::unique_ptr<player::Controller> m_controller;
        };
    }
}


