#include "Match.h"
#include "Player.h"
#include "Terrain.h"
#include "../common/MatchMessage.h"

namespace isomap {
    namespace client {
        Match::Match( id_t connectionID ) :
                m_connectionId( connectionID ) {
            m_terrain = new Terrain();
        }

        Match::~Match() {
            delete m_terrain;
            // TODO: clean up everything else
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
                    m_player = new Player( this, m_connectionId, msg->name() );
                    m_players[m_connectionId] = m_player;
                    break;

                case common::MatchServerMessage::PlayerRejected:
                    // yeah, no clue...
                    break;

                case common::MatchServerMessage::PlayerJoined: {
                    // TODO: add sanity checks
                    auto* player = new Player( this, msg->id(), msg->name() );
                    m_players[msg->id()] = player;
                    break;
                }

                case common::MatchServerMessage::PlayerLeft: {
                    // TODO: add sanity checks
                    Player* player = getPlayer( msg->id() );
                    // TODO: remove player
                    break;
                }

                case common::MatchServerMessage::MatchStarted: {
                    // TODO: add sanity checks
                    m_started = true;
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

        void Match::registerPlayer( const std::string& name ) {
            enqueueMessage( common::MatchClientMessage::registerPlayer( m_connectionId, name ) );
        }

        void Match::unregisterPlayer() {
            if ( m_player != nullptr ) {
                enqueueMessage( common::MatchClientMessage::unregisterPlayer( m_connectionId ) );
            }
            // TODO: add fat warning
        }

        void Match::startMatch() {
            if ( m_player != nullptr ) {
                enqueueMessage( common::MatchClientMessage::startMatch( m_connectionId ) );
            }
            // TODO: add fat warning
        }

        Player* Match::getPlayer( isomap::id_t id ) {
            auto it = m_players.find( id );
            if ( it == m_players.end() ) {
                return nullptr;
            }
            return it->second;
        }

        void Match::initRender( vl::RenderingAbstract* rendering ) {
            m_terrain->initRender( rendering );
            for ( auto player : m_players ) {
                player.second->initRender( rendering );
            }
        }

        void Match::render() {
            m_terrain->render();
            for ( auto player : m_players ) {
                player.second->render();
            }
        }

        void Match::disableRendering() {
            m_terrain->disableRendering();
            for ( auto player : m_players ) {
                player.second->disableRendering();
            }
        }

        void Match::enableRendering() {
            m_terrain->enableRendering();
            for ( auto player : m_players ) {
                player.second->enableRendering();
            }
        }

        void Match::dumpActors() {
            for ( auto player : m_players ) {
                player.second->dumpActors();
            }
        }

    }
}