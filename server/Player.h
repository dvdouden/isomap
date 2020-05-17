#pragma once

#include <map>
#include <vector>
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

            void update();

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

            void registerNewUnit( Unit* unit );

            void dump();

        private:
            id_t m_id;
            std::string m_name;
            uint8_t* m_fogMap = nullptr;
            Terrain* m_terrain = nullptr;
            Match* m_match = nullptr;
            bool m_ready = false;

            std::vector<uint32_t> m_uncoveredTiles;

            std::map<id_t, Structure*> m_structures;
            std::map<id_t, Unit*> m_units;
        };

    }
}