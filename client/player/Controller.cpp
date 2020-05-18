#include "Controller.h"
#include "../Player.h"
#include "../Terrain.h"
#include "../../common/PlayerMessage.h"

namespace isomap {
    namespace client {
        namespace player {

            void Controller::buildStructure(
                    int32_t tileX,
                    int32_t tileY,
                    common::StructureType* structureType,
                    uint32_t orientation ) {
                Terrain* terrain = m_player->terrain();
                int32_t tileZ = terrain->heightMap()[tileY * terrain->width() + tileX];
                terrain->reserve( tileX, tileY, structureType->footPrint( orientation ) );
                m_player->match()->enqueueMessage( common::PlayerCommandMessage::buildStructureMsg(
                        tileX, tileY, tileZ, structureType->id(), orientation ) );
            }

            bool Controller::canPlace(
                    int32_t worldX,
                    int32_t worldY,
                    common::StructureType* structureType,
                    uint32_t rotation ) const {
                Terrain* terrain = m_player->terrain();
                if ( worldX < 0 || worldY < 0 || worldX + structureType->width( rotation ) >= terrain->width() ||
                     worldY + structureType->height( rotation ) >= terrain->height() ) {
                    return false;
                }
                for ( uint32_t y = 0; y < structureType->height( rotation ); ++y ) {
                    for ( uint32_t x = 0; x < structureType->width( rotation ); ++x ) {
                        if ( structureType->footPrint( rotation, x, y ) == 0 ) {
                            continue;
                        }
                        if ( !terrain->isVisible( worldX + x, worldY + y ) ) {
                            return false;
                        }
                        if ( terrain->occupied( worldX + x, worldY + y ) != 0 ) {
                            return false;
                        }
                    }
                }
                return true;
            }

            void Controller::buildUnit(
                    int32_t tileX,
                    int32_t tileY,
                    common::UnitType* unitType,
                    uint32_t orientation ) {
                Terrain* terrain = m_player->terrain();
                int32_t tileZ = terrain->heightMap()[tileY * terrain->width() + tileX];
                m_player->match()->enqueueMessage(
                        common::PlayerCommandMessage::buildUnitMsg( tileX, tileY, tileZ, unitType->id(),
                                                                    orientation ) );
            }

            void Controller::enqueueMessage( common::PlayerCommandMessage* msg ) {
                m_player->match()->enqueueMessage( msg );
            }

            void Controller::enqueueMessage( id_t id, common::UnitCommandMessage* msg ) {
                enqueueMessage( common::PlayerCommandMessage::unitCommandMsg( id, msg ) );
            }

        } // namespace end
    }
}