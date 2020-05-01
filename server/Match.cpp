#include "Match.h"
#include "Object.h"
#include "Player.h"
#include "Terrain.h"
#include "TerrainGenerator.h"
#include "../common/MatchMessage.h"
#include "../common/PlayerMessage.h"

namespace isomap {
    namespace server {

        Match::Match() {

        }

        Match::~Match() {
            delete m_terrain;
            for ( auto obj : m_objects ) {
                delete obj.second;
            }
            for ( auto player : m_players ) {
                delete player.second;
            }
        }


        void Match::processMessage( common::MatchClientMessage* msg ) {
            switch ( msg->type() ) {
                case common::MatchClientMessage::RegisterPlayer: {
                    if ( m_players.size() >= m_maxPlayers ) {
                        enqueueMessage( msg->id(), common::MatchServerMessage::playerRejected( msg->name() ) );
                        return;
                    }
                    for ( auto player : m_players ) {
                        if ( player.second->name() == msg->name() || player.first == msg->id() ) {
                            // FIXME! We should never get a new connection with the same id as an existing one
                            enqueueMessage( msg->id(), common::MatchServerMessage::playerRejected( msg->name() ) );
                            return;
                        }
                    }
                    auto* newPlayer = new Player( this, msg->id(), msg->name() );
                    enqueueMessage( newPlayer->id(), common::MatchServerMessage::playerAccepted( newPlayer->name() ) );
                    for ( auto player : m_players ) {
                        // inform other players of new player
                        enqueueMessage( player.first,
                                        common::MatchServerMessage::playerJoined( newPlayer->id(),
                                                                                  newPlayer->name() ) );
                        // inform new player of other players
                        enqueueMessage( newPlayer->id(),
                                        common::MatchServerMessage::playerJoined( player.first,
                                                                                  player.second->name() ) );
                    }
                    m_players[newPlayer->id()] = newPlayer;
                    newPlayer->setTerrain( m_terrain );
                    break;
                }

                case common::MatchClientMessage::UnRegisterPlayer: {
                    Player* unregisteredPlayer = getPlayer( msg->id() );
                    if ( unregisteredPlayer == nullptr ) {
                        // mehhh
                        return;
                    }
                    for ( auto player : m_players ) {
                        if ( player.second != unregisteredPlayer ) {
                            enqueueMessage( player.first, common::MatchServerMessage::playerLeft( msg->id() ) );
                        }
                    }
                    // TODO: delete player
                    // TODO: remove player from player list
                    break;
                }

                case common::MatchClientMessage::StartMatch: {
                    if ( m_started ) {
                        // FIXME: log warning
                        break;
                    }
                    Player* player = getPlayer( msg->id() );
                    if ( player == nullptr ) {
                        // FIXME: log warning
                        return;
                    }
                    player->startMatch();
                    if ( m_players.size() >= m_minPlayers ) {
                        bool ready = true;
                        for ( auto player : m_players ) {
                            if ( !player.second->ready() ) {
                                ready = false;
                                break;
                            }
                        }
                        if ( ready ) {
                            start();
                        }
                    }
                    break;
                }

                case common::MatchClientMessage::PlayerCommand: {
                    Player* player = getPlayer( msg->id() );
                    if ( player == nullptr ) {
                        // TODO: log fat warning
                        return;
                    }
                    player->processMessage( msg->playerMsg() );
                }

                default:
                    break;
            }
        }

        void Match::generateWorld( uint32_t width, uint32_t height ) {
            TerrainGenerator generator;
            generateWorld( width, height, &generator );
        }

        void Match::generateWorld( uint32_t width, uint32_t height, TerrainGenerator* generator ) {
            delete m_terrain;
            m_terrain = generator->generate( width, height );
            for ( auto player : m_players ) {
                player.second->setTerrain( m_terrain );
            }
        }

        void Match::addObject( Object* object ) {
            m_objects[object->id()] = object;
        }

        void Match::start() {
            m_started = true;
            m_time = 0;
            for ( auto player : m_players ) {
                player.second->init();
                enqueueMessage( player.first, common::MatchServerMessage::matchStarted() );
            }
        }

        void Match::update() {
            ++m_time;
            for ( auto player : m_players ) {
                player.second->update();
            }

            for ( auto obj : m_objects ) {
                auto* msg = obj.second->update( m_terrain );
                if ( msg != nullptr ) {
                    // FIXME
                    //printf( "Enqueue message for player %s of type %d\n", obj.second->player()->name().c_str(), msg->type() );
                    enqueueMessageAll(
                            common::MatchServerMessage::playerMsg( obj.second->player()->id(), msg ) );
                }
            }
        }

        Player* Match::getPlayer( isomap::id_t id ) {
            auto player = m_players.find( id );
            if ( player == m_players.end() ) {
                return nullptr;
            }
            return player->second;
        }


    }
}
