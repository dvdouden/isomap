#include "Player.h"
#include "Structure.h"
#include "Terrain.h"
#include "Unit.h"
#include "../common/PlayerMessage.h"
#include "../common/StructureMessage.h"
#include "../common/StructureType.h"
#include "../common/UnitMessage.h"
#include "../util/math.h"

namespace isomap {
    namespace client {

        common::PlayerCommandMessage*
        Player::buildStructure( int32_t tileX, int32_t tileY, common::StructureType* structureType,
                                uint32_t rotation ) {
            int32_t tileZ = m_terrain->heightMap()[tileY * m_terrain->width() + tileX];
            m_terrain->reserve( tileX, tileY, structureType->footPrint( rotation ) );
            return common::PlayerCommandMessage::buildStructureMsg( tileX, tileY, tileZ, structureType->id(),
                                                                    rotation );
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

                case common::PlayerServerMessage::BuildStructureAccepted: {
                    auto* str = new Structure(
                            {
                                    msg->id(),
                                    msg->typeId(),
                                    msg->x(),
                                    msg->y(),
                                    msg->z(),
                                    msg->orientation()
                            } );
                    // TODO: Make sure there's no structure with the given id
                    m_structures[msg->id()] = str;
                    str->initRender( m_rendering );
                    m_terrain->occupy( msg->x(), msg->y(), str->footPrint() );
                    break;
                }

                case common::PlayerServerMessage::BuildStructureRejected: {
                    m_terrain->unreserve( msg->x(), msg->y(),
                                          common::StructureType::get( msg->typeId() )->footPrint(
                                                  msg->orientation() ) );
                    break;
                }

                case common::PlayerServerMessage::UnitCreated: {
                    auto* unit = new Unit( msg->id(), msg->x(), msg->y(), msg->z() );
                    // TODO: Make sure there's no unit with the given id
                    m_units[msg->id()] = unit;
                    unit->initRender( m_rendering );
                    break;
                }

                case common::PlayerServerMessage::StructureMessage: {
                    processMessage( msg->structureMessage() );
                    break;
                }

                case common::PlayerServerMessage::UnitMessage: {
                    processMessage( msg->unitMessage() );
                    break;
                }

                default:
                    break;
            }
        }

        void Player::processMessage( common::StructureServerMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            m_structures[msg->id()]->processMessage( msg );
        }

        void Player::processMessage( common::UnitServerMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            m_units[msg->id()]->processMessage( msg );
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

        bool Player::canPlace( int32_t worldX, int32_t worldY, common::StructureType* structureType,
                               uint32_t rotation ) const {
            if ( worldX < 0 || worldY < 0 || worldX + structureType->width( rotation ) >= m_terrain->width() ||
                 worldY + structureType->height( rotation ) >= m_terrain->height() ) {
                return false;
            }
            for ( uint32_t y = 0; y < structureType->height( rotation ); ++y ) {
                for ( uint32_t x = 0; x < structureType->width( rotation ); ++x ) {
                    if ( structureType->footPrint( rotation, x, y ) == 0 ) {
                        continue;
                    }
                    if ( !m_terrain->isVisible( worldX + x, worldY + y ) ) {
                        return false;
                    }
                    if ( m_terrain->occupied( worldX + x, worldY + y ) != 0 ) {
                        return false;
                    }
                }
            }
            return true;
        }

    }

}