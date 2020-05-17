#include "Controller.h"
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
                m_unit->player()->enqueueMessage( m_unit->id(),
                                                  common::UnitCommandMessage::constructMsg( structure->id() ) );
                return true;
            }

            bool Controller::moveTo( uint32_t x, uint32_t y ) {
                return moveTo( x, y, nullptr );
            }

            bool Controller::moveTo( Structure* structure ) {
                return moveTo( 0, 0, structure );
            }

            void Controller::dump() {
                printf( "Controller:\n" );
            }

            void Controller::onMessage( common::UnitServerMessage::Type msgType ) {
                switch ( msgType ) {
                    case common::UnitServerMessage::Construct:
                    case common::UnitServerMessage::Status:
                    case common::UnitServerMessage::MoveTo:
                    case common::UnitServerMessage::Stop:
                    case common::UnitServerMessage::Done:
                    case common::UnitServerMessage::Abort:
                        break;
                }
            }

            void Controller::update() {
                switch ( m_unit->state() ) {
                    default:
                        break;
                }
            }

            bool Controller::moveTo( uint32_t x, uint32_t y, Structure* structure ) {
                m_wayPoints.clear();

                auto* terrain = m_unit->player()->terrain();
                if ( structure != nullptr ) {
                    printf( "Move to structure %d\n", structure->id() );
                } else {
                    printf( "Move to %d %d\n", x, y );
                }

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

                // create a todo list for the algorithm
                std::priority_queue<node, std::vector<node>, std::greater<>> todo;

                uint32_t targetIdx = y * width + x;
                uint32_t startIdx = m_unit->tileY() * width + m_unit->tileX();

                todo.push( {1, startIdx} );
                bool found = false;
                while ( !todo.empty() ) {
                    auto tile = todo.top();
                    todo.pop();
                    auto value = nodeMap[tile.from];
                    uint32_t tile_x = tile.from % width;
                    uint32_t tile_y = tile.from / width;
                    //printf( "Test %d %d\n", tile_x, tile_y);
                    if ( structure != nullptr ) {
                        if ( structure->isAdjacentTo( tile_x, tile_y ) ) {
                            targetIdx = tile.from;
                            // update the target position in the command, now that we have found a place to go
                            x = targetIdx % width;
                            y = targetIdx / width;
                            printf( "Found tile %d %d to be adjacent to structure\n", x, y );
                            found = true;
                            break;
                        }
                    } else if ( tile.from == targetIdx ) {
                        found = true;
                        break;
                    }
                    uint8_t canReach = terrain->pathMap()[tile.from];
                    //printf( "[%2d,%2d] %02X\n", tile_x, tile_y, canReach );

                    // for now we're going to move in every direction, as long as we haven't traveled there yet.
                    if ( canReach & common::path::bitDownLeft ) {
                        //printf( "down left\n" );
                        auto idx = tile.from - width - 1;
                        if ( nodeMap[idx].value == 0 ) {
                            nodeMap[idx].value = value.value + 14;
                            nodeMap[idx].from = tile.from;
                            todo.push( {value.value + 14, idx} );
                        }
                    }
                    if ( canReach & common::path::bitDown ) {
                        //printf( "down\n" );
                        auto idx = tile.from - width;
                        if ( nodeMap[idx].value == 0 ) {
                            nodeMap[idx].value = value.value + 10;
                            nodeMap[idx].from = tile.from;
                            todo.push( {value.value + 10, idx} );
                        }
                    }
                    if ( canReach & common::path::bitDownRight ) {
                        //printf( "down right\n" );
                        auto idx = tile.from - width + 1;
                        if ( nodeMap[idx].value == 0 ) {
                            nodeMap[idx].value = value.value + 14;
                            nodeMap[idx].from = tile.from;
                            todo.push( {value.value + 14, idx} );
                        }
                    }
                    if ( canReach & common::path::bitRight ) {
                        //printf( "right\n" );
                        auto idx = tile.from + 1;
                        if ( nodeMap[idx].value == 0 ) {
                            nodeMap[idx].value = value.value + 10;
                            nodeMap[idx].from = tile.from;
                            todo.push( {value.value + 10, idx} );
                        }
                    }
                    if ( canReach & common::path::bitUpRight ) {
                        //printf( "right up\n" );
                        auto idx = tile.from + width + 1;
                        if ( nodeMap[idx].value == 0 ) {
                            nodeMap[idx].value = value.value + 14;
                            nodeMap[idx].from = tile.from;
                            todo.push( {value.value + 14, idx} );
                        }
                    }
                    if ( canReach & common::path::bitUp ) {
                        //printf( "up\n" );
                        auto idx = tile.from + width;
                        if ( nodeMap[idx].value == 0 ) {
                            nodeMap[idx].value = value.value + 10;
                            nodeMap[idx].from = tile.from;
                            todo.push( {value.value + 10, idx} );
                        }
                    }
                    if ( canReach & common::path::bitUpLeft ) {
                        //printf( "up left\n" );
                        auto idx = tile.from + width - 1;
                        if ( nodeMap[idx].value == 0 ) {
                            nodeMap[idx].value = value.value + 14;
                            nodeMap[idx].from = tile.from;
                            todo.push( {value.value + 14, idx} );
                        }
                    }
                    if ( canReach & common::path::bitLeft ) {
                        //printf( "left\n" );
                        auto idx = tile.from - 1;
                        if ( nodeMap[idx].value == 0 ) {
                            nodeMap[idx].value = value.value + 10;
                            nodeMap[idx].from = tile.from;
                            todo.push( {value.value + 10, idx} );
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
                    m_unit->player()->enqueueMessage( m_unit->id(),
                                                      common::UnitCommandMessage::moveMsg( m_wayPoints ) );
                    return true;
                } else {
                    printf( "No route!\n" );
                    return false;
                }
            }

        }
    }
}