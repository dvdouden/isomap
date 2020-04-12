#include "Match.h"
#include "Object.h"
#include "Player.h"
#include "Terrain.h"
#include "TerrainGenerator.h"

namespace isomap {
    namespace server {

        Match::Match() {

        }

        Match::~Match() {
            delete m_terrain;
            for ( auto obj : m_objects ) {
                delete obj.second;
            }
            for ( auto player : m_players ) {
                delete player;
            }
        }

        void Match::generateWorld( uint32_t width, uint32_t height ) {
            TerrainGenerator generator;
            generateWorld( width, height, &generator );
        }

        void Match::generateWorld( uint32_t width, uint32_t height, TerrainGenerator* generator ) {
            delete m_terrain;
            m_terrain = generator->generate( width, height );
            for ( auto* player : m_players ) {
                player->setTerrain( m_terrain );
            }
        }

        void Match::addPlayer( Player* player ) {
            m_players.push_back( player );
            player->setTerrain( m_terrain );
        }

        void Match::addObject( Object* object ) {
            m_objects[object->id()] = object;
        }

        void Match::start() {
            for ( auto* player : m_players ) {
                player->init();
            }
            m_time = 0;
        }

        void Match::update() {
            for ( auto* player : m_players ) {
                player->update();
            }

            for ( auto obj : m_objects ) {
                obj.second->update( m_terrain );
            }
        }


    }
}
