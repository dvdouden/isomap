
#include "player/AutonomousUnitsAI.h"
#include "Match.h"
#include "Player.h"

#include <memory>
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

        Player::Player( Match* match, id_t id, std::string name, bool useAi ) :
                m_match( match ),
                m_id( id ),
                m_name( std::move( name ) ) {
            if ( useAi ) {
                m_controller = std::make_unique<AutonomousUnitsAI>( this );
            }
        }

        void Player::buildStructure( int32_t tileX, int32_t tileY, common::StructureType* structureType,
                                     uint32_t orientation ) {
            int32_t tileZ = m_terrain->heightMap()[tileY * m_terrain->width() + tileX];
            m_terrain->reserve( tileX, tileY, structureType->footPrint( orientation ) );
            m_match->enqueueMessage( common::PlayerCommandMessage::buildStructureMsg(
                    tileX, tileY, tileZ, structureType->id(), orientation ) );
        }

        void Player::buildUnit( int32_t tileX, int32_t tileY, common::UnitType* unitType, uint32_t orientation ) {
            int32_t tileZ = m_terrain->heightMap()[tileY * m_terrain->width() + tileX];
            m_match->enqueueMessage(
                    common::PlayerCommandMessage::buildUnitMsg( tileX, tileY, tileZ, unitType->id(), orientation ) );
        }

        void Player::processMessage( common::PlayerServerMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            //printf( "Client Session[%s] Player %s process server msg of type %d\n", m_match->player()->name().c_str(), m_name.c_str(), msg->type() );
            switch ( msg->type() ) {
                case common::PlayerServerMessage::Status:
                    break;

                case common::PlayerServerMessage::BuildStructureAccepted: {
                    auto* str = new Structure( this, *msg->structureData() );
                    // TODO: Make sure there's no structure with the given id
                    m_structures[str->id()].reset( str );
                    if ( m_renderer ) {
                        m_renderer->addStructure( str );
                    }
                    m_terrain->unreserve( str->x(), str->y(), str->footPrint() );
                    m_terrain->addStructure( str );
                    if ( m_controller ) {
                        m_controller->onStructureCreated( str );
                    }
                    break;
                }

                case common::PlayerServerMessage::BuildStructureRejected: {
                    m_terrain->unreserve( msg->x(), msg->y(),
                                          common::StructureType::get( msg->typeId() )->footPrint(
                                                  msg->orientation() ) );
                    break;
                }

                case common::PlayerServerMessage::StructureVisible: {
                    auto* str = getStructure( msg->structureData()->id );
                    if ( str == nullptr ) {
                        str = new Structure( this, *msg->structureData() );
                        m_structures[str->id()].reset( str );
                        if ( m_renderer ) {
                            m_renderer->addStructure( str );
                        }
                        m_terrain->addStructure( str );
                    } else {
                        str->setVisible( true );
                    }
                    break;
                }

                case common::PlayerServerMessage::StructureInvisible: {
                    auto* str = getStructure( msg->id() );
                    if ( str != nullptr ) {
                        str->setVisible( false );
                    }
                    break;
                }

                case common::PlayerServerMessage::StructureDestroyed: {
                    auto* str = getStructure( msg->id() );
                    if ( str != nullptr ) {
                        if ( m_controller ) {
                            m_controller->onStructureDestroyed( str );
                        }
                        m_terrain->removeStructure( str );
                        m_structures.erase( msg->id() );
                    }
                    break;
                }

                case common::PlayerServerMessage::UnitCreated: {
                    auto* unit = new Unit( this, *msg->unitData() );
                    // TODO: Make sure there's no unit with the given id
                    m_units[unit->id()].reset( unit );
                    if ( m_renderer ) {
                        m_renderer->addUnit( unit );
                    }
                    m_terrain->addUnit( unit );
                    if ( m_controller ) {
                        m_controller->onUnitCreated( unit );
                    }
                    break;
                }

                case common::PlayerServerMessage::UnitVisible: {
                    auto* unit = getUnit( msg->unitData()->id );
                    if ( unit == nullptr ) {
                        unit = new Unit( this, *msg->unitData() );
                        m_units[unit->id()].reset( unit );
                        if ( m_renderer ) {
                            m_renderer->addUnit( unit );
                        }
                        m_terrain->addUnit( unit );
                    } else {
                        if ( !unit->visible() ) {
                            unit->setVisible( *msg->unitData() );
                            m_terrain->addUnit( unit );
                        }
                    }
                    break;
                }

                case common::PlayerServerMessage::UnitInvisible: {
                    auto* unit = getUnit( msg->id() );
                    if ( unit != nullptr ) {
                        if ( unit->visible() ) {
                            unit->setInvisible();
                            m_terrain->removeUnit( unit );
                        }
                    }
                    break;
                }

                case common::PlayerServerMessage::UnitDestroyed: {
                    auto* unit = getUnit( msg->id() );
                    if ( unit != nullptr ) {
                        if ( m_controller ) {
                            m_controller->onUnitDestroyed( unit );
                        }
                        m_terrain->removeUnit( unit );
                        m_units.erase( msg->id() );
                    }
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
            //printf( "Client Session[%s] Player %s process server msg completed\n", m_match->player()->name().c_str(), m_name.c_str());
        }

        void Player::processMessage( common::StructureServerMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            //printf( "Process message for structure %d\n", msg->data().id );
            // FIXME: check if structure with ID exists
            m_structures[msg->data().id]->processMessage( msg );
        }

        void Player::processMessage( common::UnitServerMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            // FIXME: check if unit with ID exists
            m_units[msg->data().id]->processMessage( msg );
        }

        void Player::update() {
            if ( m_controller ) {
                m_controller->update();
            }
            for ( auto& unit : m_units ) {
                unit.second->update();
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

        Structure* Player::getStructure( id_t id ) {
            auto str = m_structures.find( id );
            if ( str == m_structures.end() ) {
                return nullptr;
            }
            return str->second.get();
        }

        Unit* Player::getUnit( id_t id ) {
            auto unit = m_units.find( id );
            if ( unit == m_units.end() ) {
                return nullptr;
            }
            return unit->second.get();
        }

        void Player::startMatch() {
            m_terrain = m_match->terrain();
        }

        void Player::dump() const {
            printf( "client Player %08X [%s]:\n", m_id, m_name.c_str() );
            if ( m_controller ) {
                m_controller->dump();
            }
            printf( "%d structures\n", m_structures.size() );
            for ( auto& structure : m_structures ) {
                structure.second->dump();
            }
            printf( "%d units\n", m_units.size() );
            for ( auto& unit : m_units ) {
                unit.second->dump();
            }
        }
    }

}