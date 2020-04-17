#include <cstring>
#include "Match.h"
#include "Player.h"
#include "Structure.h"
#include "Terrain.h"
#include "Unit.h"
#include "../common/TerrainMessage.h"
#include "../common/PlayerMessage.h"

namespace isomap {
    namespace server {

        Player::Player(Match* match)
        :
        m_match( match )
        {

        }

        Player::~Player() {
            delete[] m_fogMap;
        }

        void Player::init() {
            auto size = m_terrain->width() * m_terrain->height();
            m_fogMap = new uint8_t[size];
            ::memset( m_fogMap, 0, size );
            m_uncoveredTiles.clear();
        }

        void Player::unFog( int32_t tile_x, int32_t tile_y, int32_t radius ) {
            int32_t cnt = 0;
            int32_t radiusSquared = radius * radius;
            for ( int32_t y = tile_y - radius; y <= tile_y + radius; ++y ) {
                if ( y < 0 || y >= m_terrain->height() ) {
                    continue;
                }

                int32_t deltaY = (y - tile_y) * (y - tile_y);
                for ( int32_t x = tile_x - radius; x <= tile_x + radius; ++x ) {

                    if ( x < 0 || x >= m_terrain->width() ) {
                        continue;
                    }
                    ++cnt;
                    int32_t deltaX = (x - tile_x) * (x - tile_x);
                    if ( deltaX + deltaY <= radiusSquared ) {
                        uint32_t idx = y * m_terrain->width() + x;
                        if ( m_fogMap[idx] == 0 ) {
                            m_uncoveredTiles.push_back( idx );
                        }
                        m_fogMap[idx] = 255;
                    }
                }
            }
        }

        void Player::update() {
            auto* scratch = m_fogMap;
            for ( uint32_t y = 0; y < m_terrain->height(); ++y ) {
                for ( uint32_t x = 0; x < m_terrain->width(); ++x ) {
                    if ( *scratch > 1 ) {
                        *scratch -= 1;
                    }
                    ++scratch;
                }
            }
        }

        common::TerrainMessage* Player::createTerrainMessage() {
            if ( m_uncoveredTiles.empty() ) {
                return nullptr;
            }
            auto* msg = m_terrain->updateMessage( m_uncoveredTiles );
            m_uncoveredTiles.clear();
            return msg;
        }



        void Player::processMessage( common::PlayerCommandMessage* msg ) {
            switch ( msg->type() ) {
                case common::PlayerCommandMessage::BuildStructure:
                    m_match->addObject( new Structure( this, msg->x(), msg->y(), msg->z() ) );
                    break;

                case common::PlayerCommandMessage::BuildUnit:
                    m_match->addObject( new Unit( this, msg->x(), msg->y(), msg->z() ) );
                    break;

                default:
                    break;
            }
        }
    }
}
