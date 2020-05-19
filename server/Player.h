#pragma once

#include <map>
#include <set>
#include "types.h"

namespace isomap {
    namespace server {
        class Player {
        public:
            Player( Match* match, id_t id, std::string name );

            ~Player();

            Player( const Player& ) = delete;

            const Player& operator=( const Player& ) = delete;

            void init();

            id_t id() const {
                return m_id;
            }

            const std::string& name() const {
                return m_name;
            }

            void setTerrain( Terrain* terrain );

            common::PlayerServerMessage* update();

            void unFog( int32_t tile_x, int32_t tile_y, int32_t radius );

            void uncoverAll();

            common::TerrainMessage* terrainUpdateMessage();

            void processMessage( common::PlayerCommandMessage* msg );

            void startMatch() {
                m_ready = true;
            }

            bool ready() const {
                return m_ready;
            }

            bool canSee( Structure* structure ) const;

            bool canSee( Unit* structure ) const;

            bool canSee( uint32_t x, uint32_t y ) const;

            bool canSee( uint32_t idx ) const;

            Match* match() const {
                return m_match;
            }

            Terrain* terrain() const {
                return m_terrain;
            }

            void destroyStructure( Structure* structure );

            void destroyUnit( Unit* unit );

            Structure* getStructure( id_t id ) const;

            Unit* getUnit( id_t id ) const;

            void registerNewUnit( Unit* unit, id_t structureId );

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

                if ( m_credits % 64 == 0 || m_credits == m_maxCredits ) {
                    m_dirty = true;
                }
                return amount;
            }

            uint32_t decCredits( uint32_t amount ) {
                if ( amount > m_credits ) { ;
                    return false;
                }
                m_credits -= amount;
                if ( m_credits % 64 == 0 ) {
                    m_dirty = true;
                }
                return amount;
            }

            uint32_t credits() const {
                return m_credits;
            }

            uint32_t maxCredits() const {
                return m_maxCredits;
            }

            void markDirty() {
                m_dirty = true;
            }

            void updateMaxCredits();

            void dump();

        private:
            id_t m_id;
            std::string m_name;
            uint8_t* m_fogMap = nullptr;
            Terrain* m_terrain = nullptr;
            Match* m_match = nullptr;
            bool m_ready = false;
            uint32_t m_credits = 0;
            uint32_t m_maxCredits = 0;

            bool m_dirty = false;

            std::set<uint32_t> m_uncoveredTiles;

            std::map<id_t, Structure*> m_structures;
            std::map<id_t, Unit*> m_units;
        };

    }
}