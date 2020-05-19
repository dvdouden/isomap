
#include "player/AutonomousUnitsController.h"
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
                m_controller = std::make_unique<player::AutonomousUnitsController>( this );
            }
        }

        void Player::processMessage( common::PlayerServerMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            //printf( "Client Session[%s] Player %s process server msg of type %d\n", m_match->player()->name().c_str(), m_name.c_str(), msg->type() );
            switch ( msg->type() ) {
                case common::PlayerServerMessage::Status:
                    m_credits = msg->credits();
                    m_maxCredits = msg->maxCredits();
                    break;

                case common::PlayerServerMessage::BuildStructureAccepted: {
                    if ( getStructure( msg->structureData()->id ) != nullptr ) {
                        printf( "Warning: received BuildStructureAccepted msg for existing structure %d\n",
                                msg->structureData()->id );
                    }
                    auto* str = new Structure( this, *msg->structureData() );
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
                    if ( getUnit( msg->unitData()->id ) != nullptr ) {
                        printf( "Warning: received UnitCreated msg for existing unit %d\n", msg->unitData()->id );
                    }
                    auto* unit = new Unit( this, *msg->unitData() );
                    m_units[unit->id()].reset( unit );
                    if ( m_renderer ) {
                        m_renderer->addUnit( unit );
                    }
                    m_terrain->addUnit( unit );
                    if ( m_controller ) {
                        m_controller->onUnitCreated( unit, msg->id() );
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

            auto* structure = getStructure( msg->data().id );
            if ( structure == nullptr ) {
                printf( "Warning: received msg for non existing structure %d\n", msg->data().id );
                return;
            }
            structure->processMessage( msg );
        }

        void Player::processMessage( common::UnitServerMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            auto* unit = getUnit( msg->data().id );
            if ( unit == nullptr ) {
                printf( "Warning: received msg for non existing unit %d\n", msg->data().id );
                return;
            }
            unit->processMessage( msg );
        }

        void Player::update() {
            if ( m_controller ) {
                m_controller->update();
            }
            for ( auto& unit : m_units ) {
                unit.second->update();
            }
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
            printf( "%lu structures\n", m_structures.size() );
            for ( auto& structure : m_structures ) {
                structure.second->dump();
            }
            printf( "%lu units\n", m_units.size() );
            for ( auto& unit : m_units ) {
                unit.second->dump();
            }
            m_terrain->dump();
        }
    }

}