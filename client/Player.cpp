#include "Player.h"
#include "Structure.h"
#include "Terrain.h"
#include "Unit.h"
#include "../common/PlayerMessage.h"
#include "../util/math.h"

namespace isomap {
    namespace client {

        common::PlayerCommandMessage* Player::buildStructure( int32_t tileX, int32_t tileY ) {
            int32_t tileZ = m_terrain->heightMap()[tileY * m_terrain->width() + tileX];
            return common::PlayerCommandMessage::buildStructureMsg( tileX, tileY, tileZ );
        }

        common::PlayerCommandMessage* Player::buildUnit( int32_t tileX, int32_t tileY ) {
            int32_t tileZ = m_terrain->heightMap()[tileY * m_terrain->width() + tileX];
            return common::PlayerCommandMessage::buildUnitMsg( tileX, tileY, tileZ );
        }

        void Player::processMessage( common::PlayerServerMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            switch ( msg->type() ) {
                case common::PlayerServerMessage::Status:
                    break;

                case common::PlayerServerMessage::StructureCreated: {
                    auto* str = new Structure( msg->id(), msg->x(), msg->y(), msg->z() );
                    // TODO: Make sure there's no structure with the given id
                    m_structures[msg->id()] = str;
                    str->initRender( m_rendering );
                    break;
                }

                case common::PlayerServerMessage::UnitCreated: {
                    auto* unit = new Unit( msg->id(), msg->x(), msg->y(), msg->z() );
                    // TODO: Make sure there's no unit with the given id
                    m_units[msg->id()] = unit;
                    unit->initRender( m_rendering );
                    break;
                }

                default:
                    break;
            }
        }

        void Player::render() {
            // TODO: This shouldn't be done per player
            for ( auto it : m_structures ) {
                it.second->render();
            }
            for ( auto it : m_units ) {
                it.second->render();
            }
        }

        bool Player::canPlace( int32_t tileX, int32_t tileY, uint32_t width, uint32_t height ) const {
            if ( tileX < 0 || tileY < 0 || tileX + width >= m_terrain->width() ||
                 tileY + height >= m_terrain->height() ) {
                return false;
            }
            for ( uint32_t y = tileY; y < tileY + height; ++y ) {
                for ( uint32_t x = tileX; x < tileX + width; ++x ) {
                    if ( !m_terrain->isVisible( x, y ) ) {
                        return false;
                    }
                }
            }
            for ( const auto& structure : m_structures ) {
                if ( structure.second->occupies( tileX, tileY, width, height ) ) {
                    return false;
                }
            }
            return true;
        }

    }

}