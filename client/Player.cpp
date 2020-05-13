#include <vlGraphics/Rendering.hpp>

#include "Match.h"
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

        Player::Player( Match* match, id_t id, std::string name ) :
                m_match( match ),
                m_id( id ),
                m_name( std::move( name ) ) {
            m_color = vl::fvec4(
                    ((id >> 24u) & 0xFFu) / 255.0,
                    ((id >> 16u) & 0xFFu) / 255.0,
                    ((id >> 8u) & 0xFFu) / 255.0,
                    (id & 0xFFu) / 255.0 );
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
                    if ( m_rendering != nullptr ) {
                        str->initRender( m_rendering, m_sceneManager.get() );
                    }
                    m_terrain->unreserve( str->x(), str->y(), str->footPrint() );
                    m_terrain->occupy( str->x(), str->y(), str->footPrint() );
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
                        if ( m_rendering != nullptr ) {
                            str->initRender( m_rendering, m_sceneManager.get() );
                        }
                        m_terrain->occupy( str->x(), str->y(), str->footPrint() );
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
                        m_terrain->vacate( str->x(), str->y(), str->footPrint() );
                        delete str;
                        m_structures.erase( msg->id() );
                    }
                    break;
                }

                case common::PlayerServerMessage::UnitCreated: {
                    auto* unit = new Unit( this, *msg->unitData() );
                    // TODO: Make sure there's no unit with the given id
                    m_units[unit->id()].reset( unit );
                    if ( m_rendering != nullptr ) {
                        unit->initRender( m_rendering, m_sceneManager.get() );
                    }
                    break;
                }

                case common::PlayerServerMessage::UnitVisible: {
                    auto* unit = getUnit( msg->unitData()->id );
                    if ( unit == nullptr ) {
                        unit = new Unit( this, *msg->unitData() );
                        m_units[unit->id()].reset( unit );
                        if ( m_rendering != nullptr ) {
                            unit->initRender( m_rendering, m_sceneManager.get() );
                        }
                    } else {
                        unit->setVisible( *msg->unitData() );
                    }
                    break;
                }

                case common::PlayerServerMessage::UnitInvisible: {
                    auto* unit = getUnit( msg->id() );
                    if ( unit != nullptr ) {
                        unit->setInvisible();
                    }
                    break;
                }

                case common::PlayerServerMessage::UnitDestroyed: {
                    auto* unit = getUnit( msg->id() );
                    if ( unit != nullptr ) {
                        delete unit;
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
            for ( auto& unit : m_units ) {
                unit.second->update();
            }
        }


        void Player::render() {
            // TODO: This shouldn't be done per player
            for ( auto& it : m_structures ) {
                it.second->render();
            }
            for ( auto& it : m_units ) {
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

        void Player::initRender( vl::RenderingAbstract* rendering ) {
            m_sceneManager = new vl::SceneManagerActorTree;
            m_sceneManager->setCullingEnabled( false );
            rendering->as<vl::Rendering>()->sceneManagers()->push_back( m_sceneManager.get() );
            m_rendering = rendering;
        }

        void Player::disableRendering() {
            if ( m_sceneManager ) {
                m_sceneManager->setEnableMask( 0 );
            }
        }

        void Player::enableRendering() {
            if ( m_sceneManager ) {
                m_sceneManager->setEnableMask( 0xFFFFFFFF );
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

        void Player::dumpActors() {
            for ( auto actor : m_sceneManager->tree()->actors()->vector() ) {
                printf( "%s at %f %f %f\n",
                        actor->objectName().c_str(),
                        actor->transform()->worldMatrix().getT().x(),
                        actor->transform()->worldMatrix().getT().y(),
                        actor->transform()->worldMatrix().getT().z() );
            }
        }

        void Player::startMatch() {
            m_terrain = m_match->terrain();
        }
    }

}