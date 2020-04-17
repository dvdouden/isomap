#include "Player.h"
#include "Terrain.h"
#include "../common/PlayerMessage.h"
#include "../util/math.h"

namespace isomap {
    namespace client {

        common::PlayerCommandMessage* Player::buildStructure( int32_t tileX, int32_t tileY ) {
            int32_t tileZ = m_terrain->heightMap()[ tileY * m_terrain->width() + tileX ];
            return common::PlayerCommandMessage::buildStructureMsg( tileX, tileY, tileZ );
        }

        common::PlayerCommandMessage* Player::buildUnit( int32_t tileX, int32_t tileY ) {
            int32_t tileZ = m_terrain->heightMap()[ tileY * m_terrain->width() + tileX ];
            return common::PlayerCommandMessage::buildUnitMsg( tileX, tileY, tileZ );
        }

        void Player::processMessage( common::PlayerServerMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            switch ( msg->type() ) {
                case common::PlayerServerMessage::Status:
                    break;

                default:
                    break;
            }
        }

    }
}