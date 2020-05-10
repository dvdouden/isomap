#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <queue>
#include "game_map.h"
#include "game_unit.h"

namespace isomap {


    bool game_unit::hasReachedTarget() const {
        real intx, inty;
        real modx = std::modf( m_x, &intx );
        real mody = std::modf( m_y, &inty );
        if ( modx != 0.0 || mody != 0.0 ) {
            return false;
        }
        return intx == m_targetX && inty == m_targetY;
    }

    void game_unit::update( bool removeFow ) {
        real intx, inty;
        real modx = std::modf( m_x, &intx );
        real mody = std::modf( m_y, &inty );

        if ( removeFow ) {
            m_world->unfog( (int)m_x, (int)m_y, 6 );
        }

        if ( modx != 0.0 || mody != 0.0 ) {
            // not at a tile boundary, keep moving
            // so this is silly; we're totally going to overshoot our target because of floating point errors.
            real speed = 0.125;
            int x = 0;
            int y = 0;
            getMotion( m_orientation / 45, x, y );
            m_x += (real)x * speed;
            m_y += (real)y * speed;

            if ( m_world->isInside( (int)m_x, (int)m_y ) ) {
                m_z = m_world->height( (int)m_x, (int)m_y );
            }
        } else {
            // tile boundary, read next movement info
            unsigned int tile_x = (int)intx;
            unsigned int tile_y = (int)inty;

            unsigned int width = m_world->width();
            if ( m_wayPoints.empty() ) {
                return;
            }
            if ( m_wayPoints.back().x == tile_x && m_wayPoints.back().y == tile_y ) {
                m_wayPoints.pop_back();
                if ( m_wayPoints.empty() ) {
                    // reached destination
                    return;
                }
            }

            m_targetOrientation = m_wayPoints.back().direction * 45;

            if ( m_orientation != m_targetOrientation ) {
                auto delta = m_targetOrientation - m_orientation;
                if ( delta < 0 ) {
                    delta += 360;
                }
                if ( delta < 180 ) {
                    m_orientation += 5;
                    if ( m_orientation >= 360 ) {
                        m_orientation -= 360;
                    }
                } else {
                    m_orientation -= 5;
                    if ( m_orientation < 0 ) {
                        m_orientation += 360;
                    }
                }
            } else {
                // already have target orientation, start moving!
                real speed = 0.125;
                int x = 0;
                int y = 0;
                int ori = m_orientation / 45;
                getMotion( ori, x, y );
                unsigned char canReach = m_world->canReach( (int)tile_x + x, (int)tile_y + y );
                if ( (canReach & (1 << ori) == 0) ) {
                    // can no longer reach destination
                    m_targetX = tile_x;
                    m_targetY = tile_y;
                    m_wayPoints.clear();
                    return;
                }

                m_x += (real)x * speed;
                m_y += (real)y * speed;

                if ( m_world->isInside( (int)m_x, (int)m_y ) ) {
                    m_z = m_world->height( (int)m_x, (int)m_y );
                }
            }
        }

    }

    void game_unit::moveTo( int x, int y ) {
        //printf( "Move to %d %d\n", x, y );


        auto width = m_world->width();
        auto height = m_world->height();

        m_wayPoints.clear();

        // create buffer for A* algorithm
        struct node {
            unsigned int value = 0;
            unsigned int from = 0;

            bool operator>( const node& rhs ) const {
                return value > rhs.value;
            }
        };

        auto* tmp = new node[width * height];
        memset( tmp, 0, width * height * sizeof( unsigned int ) );

        // create a todo list for the algorithm
        std::priority_queue<node, std::vector<node>, std::greater<>> todo;
        todo.push( {1, ((unsigned int)m_y) * width + ((unsigned int)m_x)} );
        while ( !todo.empty() ) {
            auto tile = todo.top();
            todo.pop();
            auto value = tmp[tile.from];
            unsigned int tile_x = tile.from % width;
            unsigned int tile_y = tile.from / width;
            //printf( "Test %d %d\n", tile_x, tile_y);
            if ( tile_x == x && tile_y == y ) {
                break;
            }
            unsigned char canReach = m_world->canReach( (int)tile_x, (int)tile_y );
            //printf( "[%2d,%2d] %02X\n", tile_x, tile_y, canReach );

            // for now we're going to move in every direction, as long as we haven't traveled there yet.
            if ( canReach & 0b0000'0001u ) {
                //printf( "down left\n" );
                auto idx = (tile_y - 1) * width + tile_x - 1;
                if ( tmp[idx].value == 0 ) {
                    tmp[idx].value = value.value + 14;
                    tmp[idx].from = tile.from;
                    todo.push( {value.value + 14, idx} );
                }
            }
            if ( canReach & 0b0000'0010u ) {
                //printf( "down\n" );
                auto idx = (tile_y - 1) * width + tile_x;
                if ( tmp[idx].value == 0 ) {
                    tmp[idx].value = value.value + 10;
                    tmp[idx].from = tile.from;
                    todo.push( {value.value + 10, idx} );
                }
            }
            if ( canReach & 0b0000'0100u ) {
                //printf( "down right\n" );
                auto idx = (tile_y - 1) * width + tile_x + 1;
                if ( tmp[idx].value == 0 ) {
                    tmp[idx].value = value.value + 14;
                    tmp[idx].from = tile.from;
                    todo.push( {value.value + 14, idx} );
                }
            }
            if ( canReach & 0b0000'1000u ) {
                //printf( "right\n" );
                auto idx = (tile_y) * width + tile_x + 1;
                if ( tmp[idx].value == 0 ) {
                    tmp[idx].value = value.value + 10;
                    tmp[idx].from = tile.from;
                    todo.push( {value.value + 10, idx} );
                }
            }
            if ( canReach & 0b0001'0000u ) {
                //printf( "right up\n" );
                auto idx = (tile_y + 1) * width + tile_x + 1;
                if ( tmp[idx].value == 0 ) {
                    tmp[idx].value = value.value + 14;
                    tmp[idx].from = tile.from;
                    todo.push( {value.value + 14, idx} );
                }
            }
            if ( canReach & 0b0010'0000u ) {
                //printf( "up\n" );
                auto idx = (tile_y + 1) * width + tile_x;
                if ( tmp[idx].value == 0 ) {
                    tmp[idx].value = value.value + 10;
                    tmp[idx].from = tile.from;
                    todo.push( {value.value + 10, idx} );
                }
            }
            if ( canReach & 0b0100'0000u ) {
                //printf( "up left\n" );
                auto idx = (tile_y + 1) * width + tile_x - 1;
                if ( tmp[idx].value == 0 ) {
                    tmp[idx].value = value.value + 14;
                    tmp[idx].from = tile.from;
                    todo.push( {value.value + 14, idx} );
                }
            }
            if ( canReach & 0b1000'0000u ) {
                //printf( "left\n" );
                auto idx = (tile_y) * width + tile_x - 1;
                if ( tmp[idx].value == 0 ) {
                    tmp[idx].value = value.value + 10;
                    tmp[idx].from = tile.from;
                    todo.push( {value.value + 10, idx} );
                }
            }
        }
        unsigned int targetIdx = y * width + x;
        unsigned int startIdx = ((unsigned int)m_y) * width + ((unsigned int)m_x);
        if ( tmp[targetIdx].value > 0 ) {
            //printf( "Found a route!\n" );
            while ( targetIdx != startIdx ) {
                int newx = (int)(targetIdx % width);
                int newy = (int)(targetIdx / width);
                targetIdx = tmp[targetIdx].from;
                int tile_x = (int)(targetIdx % width);
                int tile_y = (int)(targetIdx / width);
                unsigned char direction = getDirection( newx - tile_x, newy - tile_y );
                if ( m_wayPoints.empty() || direction != m_wayPoints.back().direction ) {
                    m_wayPoints.push_back( {newx, newy, direction} );
                }
                //printf( "[%d]: %d %d\n", tmp[targetIdx].value, tile_x, tile_y );
            }

            m_targetX = x;
            m_targetY = y;

        } else {
            //printf( "No route!\n" );
        }

        delete[] tmp;
    }


}