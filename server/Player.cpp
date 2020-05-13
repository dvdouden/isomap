#include <cstring>
#include "Match.h"
#include "Player.h"
#include "Structure.h"
#include "Terrain.h"
#include "Unit.h"
#include "../common/MatchMessage.h"
#include "../common/PlayerMessage.h"
#include "../common/StructureType.h"
#include "../common/TerrainMessage.h"

namespace isomap {
    namespace server {

        Player::Player( Match* match, id_t id, std::string name )
                :
                m_match( match ),
                m_id( id ),
                m_name( std::move( name ) ) {

        }

        Player::~Player() {
            delete[] m_fogMap;
        }

        void Player::init() {
            auto size = m_terrain->width() * m_terrain->height();
            m_fogMap = new uint8_t[size]();
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

        void Player::uncoverAll() {
            m_match->enqueueMessage( m_id, common::MatchServerMessage::terrainMsg( m_terrain->uncoverAll() ) );
        }

        void Player::update() {
            //printf( "Player %s update!\n", m_name.c_str() );
            auto* scratch = m_fogMap;
            for ( uint32_t y = 0; y < m_terrain->height(); ++y ) {
                for ( uint32_t x = 0; x < m_terrain->width(); ++x ) {
                    if ( *scratch > 1 ) {
                        *scratch -= 1;
                    }
                    ++scratch;
                }
            }
            auto* terrainMessage = terrainUpdateMessage();
            if ( terrainMessage != nullptr ) {
                m_match->enqueueMessage( m_id, common::MatchServerMessage::terrainMsg( terrainMessage ) );
            }
        }

        common::TerrainMessage* Player::terrainUpdateMessage() {
            if ( m_uncoveredTiles.empty() ) {
                return nullptr;
            }
            for ( uint32_t tile : m_uncoveredTiles ) {
                uint32_t x = tile % m_terrain->width();
                uint32_t y = tile / m_terrain->width();
                Structure* str = m_terrain->getStructureAt( x, y );
                if ( str != nullptr ) {
                    if ( !str->isSubscribed( this ) ) {
                        str->subscribe( this );
                        m_match->enqueueMessage( m_id,
                                                 common::MatchServerMessage::playerMsg(
                                                         str->player()->id(),
                                                         common::PlayerServerMessage::structureVisibleMsg(
                                                                 str->data() ) ) );
                    }
                }
                Unit* unit = m_terrain->getUnitAt( x, y );
                if ( unit != nullptr ) {

                    if ( !unit->isSubscribed( this ) ) {
                        unit->subscribe( this );
                        m_match->enqueueMessage( m_id,
                                                 common::MatchServerMessage::playerMsg(
                                                         unit->player()->id(),
                                                         common::PlayerServerMessage::unitVisibleMsg(
                                                                 unit->data() ) ) );
                    }
                }
            }
            auto* msg = m_terrain->updateMessage( m_uncoveredTiles );
            m_uncoveredTiles.clear();
            return msg;
        }

        void Player::processMessage( common::PlayerCommandMessage* msg ) {
            //printf( "Server Player %s process client msg of type %d\n", m_name.c_str(), msg->type() );
            switch ( msg->type() ) {
                case common::PlayerCommandMessage::BuildStructure: {
                    // TODO: validate parameters, send rejection on fail
                    auto* str = new Structure( this, msg->x(), msg->y(), msg->z(),
                                               common::StructureType::get( msg->id() ), msg->orientation() );
                    m_match->updateSubscriptions( str );
                    m_match->enqueueMessage( str, common::PlayerServerMessage::buildStructureAcceptedMsg(
                            str->data() ) );
                    unFog( str->x(), str->y(), 10 );
                    m_match->addObject( str );
                    m_terrain->addStructure( str );
                    m_structures[str->id()] = str;
                    break;
                }

                case common::PlayerCommandMessage::BuildUnit: {
                    auto* unit = new Unit( this, msg->x(), msg->y(), msg->z(),
                                           common::UnitType::get( msg->id() ), msg->orientation() );
                    m_match->enqueueMessage( unit, common::PlayerServerMessage::unitCreatedMsg( unit->data() ) );
                    unFog( unit->tileX(), unit->tileY(), 10 );
                    m_match->addObject( unit );
                    m_terrain->addUnit( unit );
                    m_units[unit->id()] = unit;
                    m_match->updateSubscriptions( unit );
                    break;
                }

                case common::PlayerCommandMessage::UnitCommand: {
                    // FIXME: check if unit actually exists
                    auto* unit = m_units[msg->id()];
                    unit->processMessage( msg->unitCommandMessage() );
                    break;
                }

                default:
                    break;
            }
            //printf( "Server Player %s process client msg done\n", m_name.c_str() );
        }

        void Player::setTerrain( Terrain* terrain ) {
            m_terrain = terrain;
        }

        bool Player::canSee( Structure* structure ) const {
            auto* footPrint = structure->footPrint();
            for ( uint32_t footPrintY = 0; footPrintY < footPrint->height(); ++footPrintY ) {
                for ( uint32_t footPrintX = 0; footPrintX < footPrint->width(); ++footPrintX ) {
                    if ( footPrint->get( footPrintX, footPrintY ) == 0 ) {
                        continue;
                    }
                    if ( m_fogMap[(structure->y() + footPrintY) * m_terrain->width() + (structure->x() + footPrintX)] >=
                         1 ) {
                        return true;
                    }
                }
            }
            return false;
        }

        bool Player::canSee( Unit* unit ) const {
            return m_fogMap[unit->tileY() * m_terrain->width() + unit->tileX()] >= 1;
        }

        void Player::destroyStructure( Structure* structure ) {
            // FIXME: add sanity checks!
            m_match->enqueueMessage( structure, common::PlayerServerMessage::structureDestroyedMsg( structure->id() ) );
            m_terrain->removeStructure( structure );
            m_structures.erase( structure->id() );
            // FIXME: this will invalidate iterators, so can't be called from Match::update!
            m_match->removeObject( structure );
        }

        void Player::destroyUnit( Unit* unit ) {
            // FIXME: add sanity checks!
            m_match->enqueueMessage( unit, common::PlayerServerMessage::unitDestroyedMsg( unit->id() ) );
            m_terrain->removeUnit( unit );
            m_units.erase( unit->id() );
            // FIXME: this will invalidate iterators, so can't be called from Match::update!
            m_match->removeObject( unit );
        }

        void Player::dump() {
            printf( "Player %08X [%s]:\n", m_id, m_name.c_str() );
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
