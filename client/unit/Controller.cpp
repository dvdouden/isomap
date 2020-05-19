#include <vlCore/Colors.hpp>

#include "Controller.h"
#include "PathCondition.h"
#include "../player/Controller.h"
#include "../Structure.h"
#include "../Player.h"
#include "../Terrain.h"

namespace isomap {
    namespace client {
        namespace unit {

            Controller::Controller( Unit* unit ) :
                    m_unit( unit ) {

            }

            Controller::~Controller() = default;

            bool Controller::construct( Structure* structure ) {
                //printf( "Unit %d construct %d\n", id(), structure->id() );
                if ( !m_unit->type()->canConstruct() ) {
                    printf( "[%d] Construct command given to unit without construction abilities!\n",
                            m_unit->id() );
                    return false;
                }
                if ( structure->player() != m_unit->player() ) {
                    printf( "[%d] Construct command given to unit for structure %d of other player!\n",
                            m_unit->id(),
                            structure->id() );
                    return false;
                }
                if ( structure->constructionCompleted() ) {
                    printf( "[%d] Construct command given to unit for completed structure %d!\n",
                            m_unit->id(),
                            structure->id() );
                    return false;
                }
                if ( !m_unit->isAdjacentTo( structure ) ) {
                    printf( "[%d] Construct command given to unit not adjacent to structure %d (%d.%d, %d.%d)!\n",
                            m_unit->id(),
                            structure->id(),
                            m_unit->tileX(), m_unit->subTileX(),
                            m_unit->tileY(), m_unit->subTileY() );
                    return false;
                }
                m_unit->player()->controller()->enqueueMessage( m_unit->id(),
                                                                common::UnitCommandMessage::constructMsg(
                                                                        structure->id() ) );
                return true;
            }

            bool Controller::harvest() {
                //printf( "Unit %d harvest\n", id() );
                if ( !m_unit->type()->canHarvest() ) {
                    printf( "[%d] Harvest command given to unit without harvesting abilities!\n",
                            m_unit->id() );
                    return false;
                }
                if ( m_unit->payload() >= m_unit->type()->maxPayload() ) {
                    printf( "[%d] Harvest command given to unit with full payload!\n",
                            m_unit->id() );
                    return false;
                }
                if ( m_unit->player()->terrain()->ore( m_unit->tileX(), m_unit->tileY() ) == 0 ) {
                    printf( "[%d] Harvest command given to unit but no ore at tile!\n",
                            m_unit->id() );
                    return false;
                }

                m_unit->player()->controller()->enqueueMessage( m_unit->id(),
                                                                common::UnitCommandMessage::harvestMsg() );
                return true;
            }

            bool Controller::unload() {
                //printf( "Unit %d unload\n", id() );
                if ( !m_unit->type()->canHarvest() ) {
                    printf( "[%d] Unload command given to unit without harvesting abilities!\n",
                            m_unit->id() );
                    return false;
                }
                auto* structure = m_unit->player()->terrain()->getConstructedStructureAt( m_unit->tileX(),
                                                                                          m_unit->tileY() );
                if ( structure == nullptr ) {
                    return false;
                }
                if ( structure->type()->id() != m_unit->type()->dockStructureType() ) {
                    return false;
                }
                if ( !structure->dockingTileAt( m_unit->tileX(), m_unit->tileY() ) ) {
                    return false;
                }

                m_unit->player()->controller()->enqueueMessage( m_unit->id(),
                                                                common::UnitCommandMessage::unloadMsg() );
                return true;
            }

            void Controller::dump() {
                printf( "Controller:\n" );
            }

            void Controller::onMessage( common::UnitServerMessage::Type msgType ) {
                switch ( msgType ) {
                    case common::UnitServerMessage::Construct:
                    case common::UnitServerMessage::Harvest:
                    case common::UnitServerMessage::Status:
                    case common::UnitServerMessage::Stop:
                    case common::UnitServerMessage::Abort:
                    case common::UnitServerMessage::Unload:
                        break;
                    case common::UnitServerMessage::MoveTo:
                        onMove();
                        break;
                    case common::UnitServerMessage::Done:
                        if ( m_unit->lastState() == common::Moving ) {
                            onMove();
                        }
                        break;
                }
            }

            void Controller::update() {
                switch ( m_unit->state() ) {
                    case common::Harvesting:
                        if ( !m_unit->payloadFull() ) {
                            m_unit->data().payload++;
                        }
                        break;

                    case common::Unloading:
                        m_unit->data().payload -= m_unit->player()->incCredits( 1 );
                        break;

                    default:
                        break;
                }

                for ( auto& wayPoint : m_wayPoints ) {
                    m_unit->player()->terrain()->renderer()->addHighlight( wayPoint.x, wayPoint.y, vl::fuchsia );
                }
            }

            bool Controller::moveTo( const PathCondition& pathCondition ) {
                m_wayPoints.clear();

                auto* terrain = m_unit->player()->terrain();

                auto width = terrain->width();
                auto height = terrain->height();

                // create buffer for A* algorithm
                struct node {
                    uint32_t value = 0;
                    uint32_t from = 0;

                    bool operator>( const node& rhs ) const {
                        return value > rhs.value;
                    }
                };
                std::vector<node> nodeMap( width * height );

                std::priority_queue<node, std::vector<node>, std::greater<>> todo;

                uint32_t startIdx = m_unit->tileY() * width + m_unit->tileX();
                uint32_t targetIdx = 0;

                todo.push( {1, startIdx} );
                bool found = false;
                while ( !todo.empty() ) {
                    node tile = todo.top();
                    todo.pop();
                    node value = nodeMap[tile.from];
                    if ( pathCondition.hasReached( tile.from ) ) {
                        //printf( "Found a route!\n" );
                        found = true;
                        targetIdx = tile.from;
                        break;
                    }
                    uint8_t canReach = terrain->pathMap()[tile.from];
                    //printf( "[%2d,%2d] %02X\n", tile_x, tile_y, canReach );
                    uint8_t slopeBits = terrain->slopeMap()[tile.from] % 16u;

                    // for now we're going to move in every direction, as long as we haven't traveled there yet.
                    if ( canReach & common::path::bitDownLeft ) {
                        //printf( "down left\n" );
                        uint32_t idx = tile.from - width - 1;
                        if ( nodeMap[idx].value == 0 ) {
                            nodeMap[idx].value = value.value + 15 + common::slope( slopeBits, 5 );
                            nodeMap[idx].from = tile.from;
                            todo.push( {nodeMap[idx].value, idx} );
                        }
                    }
                    if ( canReach & common::path::bitDown ) {
                        //printf( "down\n" );
                        uint32_t idx = tile.from - width;
                        if ( nodeMap[idx].value == 0 ) {
                            nodeMap[idx].value = value.value + 10 + common::slope( slopeBits, 4 );
                            nodeMap[idx].from = tile.from;
                            todo.push( {nodeMap[idx].value, idx} );
                        }
                    }
                    if ( canReach & common::path::bitDownRight ) {
                        //printf( "down right\n" );
                        uint32_t idx = tile.from - width + 1;
                        if ( nodeMap[idx].value == 0 ) {
                            nodeMap[idx].value = value.value + 15 + common::slope( slopeBits, 3 );
                            nodeMap[idx].from = tile.from;
                            todo.push( {nodeMap[idx].value, idx} );
                        }
                    }
                    if ( canReach & common::path::bitRight ) {
                        //printf( "right\n" );
                        uint32_t idx = tile.from + 1;
                        if ( nodeMap[idx].value == 0 ) {
                            nodeMap[idx].value = value.value + 10 + common::slope( slopeBits, 2 );
                            nodeMap[idx].from = tile.from;
                            todo.push( {nodeMap[idx].value, idx} );
                        }
                    }
                    if ( canReach & common::path::bitUpRight ) {
                        //printf( "right up\n" );
                        uint32_t idx = tile.from + width + 1;
                        if ( nodeMap[idx].value == 0 ) {
                            nodeMap[idx].value = value.value + 15 + common::slope( slopeBits, 1 );
                            nodeMap[idx].from = tile.from;
                            todo.push( {nodeMap[idx].value, idx} );
                        }
                    }
                    if ( canReach & common::path::bitUp ) {
                        //printf( "up\n" );
                        uint32_t idx = tile.from + width;
                        if ( nodeMap[idx].value == 0 ) {
                            nodeMap[idx].value = value.value + 10 + common::slope( slopeBits, 0 );
                            nodeMap[idx].from = tile.from;
                            todo.push( {nodeMap[idx].value, idx} );
                        }
                    }
                    if ( canReach & common::path::bitUpLeft ) {
                        //printf( "up left\n" );
                        uint32_t idx = tile.from + width - 1;
                        if ( nodeMap[idx].value == 0 ) {
                            nodeMap[idx].value = value.value + 15 + common::slope( slopeBits, 7 );
                            nodeMap[idx].from = tile.from;
                            todo.push( {nodeMap[idx].value, idx} );
                        }
                    }
                    if ( canReach & common::path::bitLeft ) {
                        //printf( "left\n" );
                        uint32_t idx = tile.from - 1;
                        if ( nodeMap[idx].value == 0 ) {
                            nodeMap[idx].value = value.value + 10 + common::slope( slopeBits, 6 );
                            nodeMap[idx].from = tile.from;
                            todo.push( {nodeMap[idx].value, idx} );
                        }
                    }
                }
                if ( found ) {
                    printf( "Found a route!\n" );
                    while ( targetIdx != startIdx ) {
                        int wayPointX = (int)(targetIdx % width);
                        int wayPointY = (int)(targetIdx / width);
                        targetIdx = nodeMap[targetIdx].from;
                        // FIXME: reduce nr of waypoints!
                        //int tile_x = (int)(targetIdx % width);
                        //int tile_y = (int)(targetIdx / width);
                        //unsigned char direction = getDirection( wayPointX - tile_x, wayPointY - tile_y );
                        //if ( wayPoints.empty() || direction != wayPoints.back().direction ) {
                        m_wayPoints.push_back( {wayPointX, wayPointY} );
                        //}
                        //printf( "[%d]: %d %d\n", nodeMap[targetIdx].value, tile_x, tile_y );
                    }
                    m_unit->player()->controller()->enqueueMessage( m_unit->id(),
                                                                    common::UnitCommandMessage::moveMsg(
                                                                            m_wayPoints ) );
                    return true;
                } else {
                    printf( "No route!\n" );
                    return false;
                }
            }

            Structure* Controller::assignedStructure() {
                if ( m_assignedStructureId == 0 ) {
                    return nullptr;
                }
                Structure* structure = m_unit->player()->getStructure( m_assignedStructureId );
                if ( structure == nullptr ) {
                    m_assignedStructureId = 0;
                }
                return structure;
            }

            void Controller::onMove() {
                if ( !m_wayPoints.empty() &&
                     m_wayPoints.back().x == m_unit->tileX() &&
                     m_wayPoints.back().y == m_unit->tileY() ) {
                    m_wayPoints.pop_back();
                }
            }

        }
    }
}