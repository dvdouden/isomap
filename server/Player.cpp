#include <cstring>
#include "Player.h"
#include "Terrain.h"
#include "../common/TerrainMessage.h"

namespace isomap { namespace server {

    Player::Player() {

    }

    Player::~Player() {
        delete[] m_fogMap;
    }

    void Player::init( Terrain* terrain ) {
        m_terrain = terrain;
        auto size = terrain->width() * terrain->height();
        m_fogMap = new uint8_t[size];
        ::memset( m_fogMap, 0, size );
        m_uncoveredTiles.clear();
    }

    void Player::unfog( int32_t tile_x, int32_t tile_y, int32_t radius ) {
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
                    m_fogMap[ idx ] = 255;
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
} }
