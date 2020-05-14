#include "Match.h"
#include "Object.h"
#include "Player.h"
#include "Structure.h"
#include "Terrain.h"
#include "TerrainGenerator.h"
#include "Unit.h"
#include "../common/MatchMessage.h"
#include "../common/PlayerMessage.h"

namespace isomap {
    namespace server {

        Match::Match() {

        }

        Match::~Match() {
            // clean up undelivered messages
            for ( auto msgs : m_messages ) {
                for ( auto msg : msgs.second ) {
                    delete msg;
                }
            }
        }


        void Match::processMessage( common::MatchClientMessage* msg ) {
            //printf( "Server Match process client msg of type %s\n", msg->typeName() );
            switch ( msg->type() ) {
                case common::MatchClientMessage::RegisterPlayer: {
                    if ( m_players.size() >= m_maxPlayers ) {
                        printf( "Too many players (%d/%d), reject player [%s]\n",
                                m_players.size(), m_maxPlayers, msg->name().c_str() );
                        enqueueMessage( msg->id(), common::MatchServerMessage::playerRejected( msg->name() ) );
                        return;
                    }
                    for ( auto& player : m_players ) {
                        if ( player.second->name() == msg->name() || player.first == msg->id() ) {
                            // FIXME! We should never get a new connection with the same id as an existing one
                            printf( "Player with name [%s] already registered\n", msg->name().c_str() );
                            enqueueMessage( msg->id(), common::MatchServerMessage::playerRejected( msg->name() ) );
                            return;
                        }
                    }
                    auto* newPlayer = new Player( this, msg->id(), msg->name() );
                    enqueueMessage( newPlayer->id(), common::MatchServerMessage::playerAccepted( newPlayer->name() ) );
                    for ( auto& player : m_players ) {
                        // inform other players of new player
                        enqueueMessage( player.first,
                                        common::MatchServerMessage::playerJoined( newPlayer->id(),
                                                                                  newPlayer->name() ) );
                        // inform new player of other players
                        enqueueMessage( newPlayer->id(),
                                        common::MatchServerMessage::playerJoined( player.first,
                                                                                  player.second->name() ) );
                    }
                    m_players[newPlayer->id()].reset( newPlayer );
                    newPlayer->setTerrain( m_terrain.get() );
                    break;
                }

                case common::MatchClientMessage::UnRegisterPlayer: {
                    Player* unregisteredPlayer = getPlayer( msg->id() );
                    if ( unregisteredPlayer == nullptr ) {
                        // mehhh
                        return;
                    }
                    for ( auto& player : m_players ) {
                        if ( player.second.get() != unregisteredPlayer ) {
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
                        for ( auto& player : m_players ) {
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
                    break;
                }

                default:
                    break;
            }
            //printf( "Server Match process client done\n" );
        }

        void Match::generateWorld( uint32_t width, uint32_t height ) {
            TerrainGenerator generator;
            generateWorld( width, height, &generator );
        }

        void Match::generateWorld( uint32_t width, uint32_t height, TerrainGenerator* generator ) {
            m_terrain.reset( generator->generate( width, height ) );
            for ( auto& player : m_players ) {
                player.second->setTerrain( m_terrain.get() );
            }
        }

        void Match::addObject( Object* object ) {
            m_objects[object->id()].reset( object );
        }

        void Match::removeObject( Object* object ) {
            m_objects.erase( object->id() );
        }

        void Match::start() {
            m_started = true;
            m_time = 0;
            m_terrain->init();
            for ( auto& player : m_players ) {
                player.second->init();
                enqueueMessage( player.first,
                                common::MatchServerMessage::initTerrain( m_terrain->width(), m_terrain->height() ) );
                enqueueMessage( player.first, common::MatchServerMessage::matchStarted() );
            }
        }

        void Match::update() {
            ++m_time;
            for ( auto& player : m_players ) {
                player.second->update();
            }

            for ( auto& obj : m_objects ) {
                auto* msg = obj.second->update( m_terrain.get() );
                if ( msg != nullptr ) {
                    // FIXME
                    //printf( "Enqueue message for player %s of type %d\n", obj.second->player()->name().c_str(), msg->type() );
                    enqueueMessage( obj.second.get(), msg );
                }
            }
        }

        Player* Match::getPlayer( isomap::id_t id ) {
            auto player = m_players.find( id );
            if ( player == m_players.end() ) {
                return nullptr;
            }
            return player->second.get();
        }

        void Match::enqueueMessage( Object* obj, common::PlayerServerMessage* msg ) {
            auto* matchMsg = common::MatchServerMessage::playerMsg( obj->player()->id(), msg );
            for ( auto& player : m_players ) {
                if ( player.second.get() == obj->player() ) {
                    //printf( "queue for self %s!\n", player.second->name().c_str());
                    enqueueMessage( player.first, matchMsg );
                } else if ( obj->isSubscribed( player.second.get() ) ) {
                    //printf( "queue for other %s!\n", player.second->name().c_str());
                    enqueueMessage( player.first, matchMsg->clone() );
                }
            }
        }

        void Match::updateSubscriptions( Structure* structure ) {
            for ( auto& player : m_players ) {
                if ( player.second.get() == structure->player() ) {
                    continue;
                }
                if ( player.second->canSee( structure ) ) {
                    structure->subscribe( player.second.get() );
                } else {
                    structure->unsubscribe( player.second.get() );
                }
            }
        }

        void Match::updateSubscriptions( Unit* unit ) {
            for ( auto& player : m_players ) {
                if ( player.second.get() == unit->player() ) {
                    continue;
                }
                if ( player.second->canSee( unit ) ) {
                    if ( !unit->isSubscribed( player.second.get() ) ) {
                        // set visible
                        enqueueMessage(
                                player.first,
                                common::MatchServerMessage::playerMsg(
                                        unit->player()->id(),
                                        common::PlayerServerMessage::unitVisibleMsg( unit->data() ) ) );
                        unit->subscribe( player.second.get() );
                    }
                } else {
                    if ( unit->isSubscribed( player.second.get() ) ) {
                        // set invisible
                        enqueueMessage(
                                player.first,
                                common::MatchServerMessage::playerMsg(
                                        unit->player()->id(),
                                        common::PlayerServerMessage::unitInvisibleMsg( unit->id() ) ) );
                        unit->unsubscribe( player.second.get() );
                    }
                }
            }
        }

        void Match::dump() {
            printf( "Match at time %d\n", m_time );
            for ( auto& player : m_players ) {
                player.second->dump();
            }
        }

    }
}
