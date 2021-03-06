#include "Match.h"

#include <memory>
#include "Player.h"
#include "Terrain.h"
#include "../common/MatchMessage.h"
#include "../common/TerrainMessage.h"

namespace isomap {
    namespace client {
        Match::Match( id_t connectionID ) :
                m_connectionId( connectionID ) {

        }

        void Match::processMessage( common::MatchServerMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            switch ( msg->type() ) {
                case common::MatchServerMessage::PlayerAccepted:
                    if ( m_player != nullptr ) {
                        // FIXME: add warning
                        return;
                    }
                    // FIXME: check that msg->id equals m_connectionId
                    m_player = new Player( this, m_connectionId, msg->name(), true );
                    if ( m_renderer ) {
                        m_renderer->addPlayer( m_player );
                    }
                    m_players[m_connectionId].reset( m_player );
                    break;

                case common::MatchServerMessage::PlayerRejected:
                    // yeah, no clue...
                    break;

                case common::MatchServerMessage::PlayerJoined: {
                    // TODO: add sanity checks
                    m_players[msg->id()] = std::make_unique<Player>( this, msg->id(), msg->name() );
                    if ( m_renderer ) {
                        m_renderer->addPlayer( m_players[msg->id()].get() );
                    }
                    break;
                }

                case common::MatchServerMessage::PlayerLeft: {
                    // TODO: add sanity checks
                    Player* player = getPlayer( msg->id() );
                    // TODO: remove player
                    break;
                }

                case common::MatchServerMessage::InitTerrain: {
                    // TODO: add sanity checks
                    m_terrain = std::make_unique<Terrain>( msg->terrainMsg()->width(), msg->terrainMsg()->height() );
                    if ( m_renderer ) {
                        m_renderer->addTerrain( m_terrain.get() );
                    }
                    break;
                }

                case common::MatchServerMessage::MatchStarted: {
                    // TODO: add sanity checks
                    m_started = true;
                    for ( auto& player : m_players ) {
                        player.second->startMatch();
                    }
                    m_player->setCredits( msg->startCredits() );
                    m_player->setMaxCredits( msg->maxCredits() );
                    break;
                }

                case common::MatchServerMessage::MatchUpdate: {
                    // TODO: add sanity checks
                    m_time = msg->time();
                    break;
                }

                case common::MatchServerMessage::PlayerMessage: {
                    auto* player = getPlayer( msg->id() );
                    // TODO: sanity check!
                    player->processMessage( msg->playerMsg() );
                    break;
                }

                case common::MatchServerMessage::TerrainMessage: {
                    m_terrain->processMessage( msg->terrainMsg() );
                    break;
                }

                default:
                    break;
            }
        }

        void Match::update() {
            for ( auto& player : m_players ) {
                player.second->update();
            }
        }

        void Match::registerPlayer( const std::string& name ) {
            enqueueMessage( common::MatchClientMessage::registerPlayer( m_connectionId, name ) );
        }

        void Match::unregisterPlayer() {
            if ( m_player ) {
                enqueueMessage( common::MatchClientMessage::unregisterPlayer( m_connectionId ) );
            }
            // TODO: add fat warning
        }

        void Match::startMatch() {
            if ( m_player ) {
                enqueueMessage( common::MatchClientMessage::startMatch( m_connectionId ) );
            }
            // TODO: add fat warning
        }

        Player* Match::getPlayer( isomap::id_t id ) {
            auto it = m_players.find( id );
            if ( it == m_players.end() ) {
                return nullptr;
            }
            return it->second.get();
        }
    }
}