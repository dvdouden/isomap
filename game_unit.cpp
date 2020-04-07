#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <queue>
#include "game_map.h"
#include "game_unit.h"

namespace isomap {

    game_unit::game_unit(vl::RenderingAbstract *rendering, game_map *world) :
            m_world(world) {
        m_transform = new vl::Transform;
        rendering->as<vl::Rendering>()->transform()->addChild(m_transform.get());

        m_geom = vl::makeBox(vl::vec3(0, 0, 0), 1, 1, 1);
        m_geom->computeNormals();

        m_effect = new vl::Effect;
        m_effect->shader()->gocMaterial()->setColorMaterialEnabled(true);
        m_effect->shader()->gocMaterial()->setDiffuse(vl::crimson);
        m_effect->shader()->enable(vl::EN_DEPTH_TEST);
        m_effect->shader()->enable(vl::EN_LIGHTING);
        m_effect->lod(0)->push_back(new vl::Shader);
        m_effect->shader(0, 1)->enable(vl::EN_BLEND);
        //m_effect->shader(0,1)->enable(vl::EN_LINE_SMOOTH);
        m_effect->shader(0, 1)->enable(vl::EN_DEPTH_TEST);
        m_effect->shader(0, 1)->enable(vl::EN_POLYGON_OFFSET_LINE);
        m_effect->shader(0, 1)->gocPolygonOffset()->set(-1.0f, -1.0f);
        m_effect->shader(0, 1)->gocPolygonMode()->set(vl::PM_LINE, vl::PM_LINE);
        m_effect->shader(0, 1)->gocColor()->setValue(vl::lightgreen);
        m_effect->shader(0, 1)->setRenderState(m_effect->shader()->getMaterial());
        m_effect->shader(0, 1)->setRenderState(m_effect->shader()->getLight(0), 0);


        vl::ref<vl::SceneManagerActorTree> scene_manager = new vl::SceneManagerActorTree;
        scene_manager->setCullingEnabled(false);
        rendering->as<vl::Rendering>()->sceneManagers()->push_back(scene_manager.get());
        scene_manager->tree()->addActor(m_geom.get(), m_effect.get(), m_transform.get());
    }

    void game_unit::setPosition(int x, int y, int z) {
        m_x = x;
        m_y = y;
        m_z = z;
    }

    unsigned char getDirection( int x, int y ) {
        if ( x == 1 && y == 0 ) {
            return 3;
        } else if ( x == 1 && y == 1 ) {
            return 4;
        } else if ( x == 0 && y == 1 ) {
            return 5;
        } else if ( x == -1 && y == 1 ) {
            return 6;
        } else if ( x == -1 && y == 0 ) {
            return 7;
        } else if ( x == -1 && y == -1 ) {
            return 0;
        } else if ( x == 0 && y == -1 ) {
            return 1;
        } else if ( x == 1 && y == -1 ) {
            return 2;
        }
        return 0;
    }

    void getMotion(unsigned char direction, int& x, int& y ) {
        switch ( direction ) {
            default:
            case 3: x =  1; y =  0; break;
            case 4: x =  1; y =  1; break;
            case 5: x =  0; y =  1; break;
            case 6: x = -1; y =  1; break;
            case 7: x = -1; y =  0; break;
            case 0: x = -1; y = -1; break;
            case 1: x =  0; y = -1; break;
            case 2: x =  1; y = -1; break;
        }
    }

    bool game_unit::hasReachedTarget() const {
        real intx, inty;
        real modx = std::modf( m_x, &intx );
        real mody = std::modf( m_y, &inty );
        if( modx != 0.0 || mody != 0.0 ) {
            return false;
        }
        return intx == m_targetX && inty == m_targetY;
    }

    void game_unit::update( bool removeFow ) {
        real intx, inty;
        real modx = std::modf( m_x, &intx );
        real mody = std::modf( m_y, &inty );

        if ( removeFow ) {
            m_world->unfog((int) m_x, (int) m_y, 6);
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

            if (m_world->isInside((int)m_x, (int)m_y)) {
                m_z = m_world->height((int)m_x, (int)m_y);
            }
        } else {
            // tile boundary, read next movement info
            unsigned int tile_x = (int) intx;
            unsigned int tile_y = (int) inty;

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

            if (m_orientation != m_targetOrientation) {
                auto delta = m_targetOrientation - m_orientation;
                if (delta < 0) {
                    delta += 360;
                }
                if (delta < 180) {
                    m_orientation += 5;
                    if (m_orientation >= 360) {
                        m_orientation -= 360;
                    }
                } else {
                    m_orientation -= 5;
                    if (m_orientation < 0) {
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
                if ( (canReach & (1 << ori) == 0 ) ) {
                    // can no longer reach destination
                    m_targetX = tile_x;
                    m_targetY = tile_y;
                    m_wayPoints.clear();
                    return;
                }

                m_x += (real)x * speed;
                m_y += (real)y * speed;

                if (m_world->isInside((int)m_x, (int)m_y)) {
                    m_z = m_world->height((int)m_x, (int)m_y);
                }
            }
        }

        vl::mat4 matrix = vl::mat4::getTranslation(m_x + 0.5, m_y + 0.5, 0.5 + m_z * ::sqrt(2.0 / 3.0) / 2.0);
        matrix *= vl::mat4::getRotation( m_orientation - 135, 0, 0, 1 );
        m_transform->setLocalMatrix(matrix);
    }

    void game_unit::moveTo(int x, int y) {
        //printf( "Move to %d %d\n", x, y );


        auto width = m_world->width();
        auto height = m_world->height();

        m_wayPoints.clear();

        // create buffer for A* algorithm
        struct node {
            unsigned int value = 0;
            unsigned int from = 0;

            bool operator >( const node& rhs ) const {
                return value > rhs.value;
            }
        };

        auto* tmp = new node[width * height];
        memset( tmp, 0, width * height * sizeof(unsigned int) );

        // create a todo list for the algorithm
        std::priority_queue<node, std::vector<node>, std::greater<>> todo;
        todo.push( {1, ((unsigned int)m_y) * width + ((unsigned int)m_x) } );
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
                    todo.push( { value.value + 14, idx } );
                }
            }
            if ( canReach & 0b0000'0010u ) {
                //printf( "down\n" );
                auto idx = (tile_y - 1) * width + tile_x;
                if ( tmp[idx].value == 0 ) {
                    tmp[idx].value = value.value + 10;
                    tmp[idx].from = tile.from;
                    todo.push( { value.value + 10, idx } );
                }
            }
            if ( canReach & 0b0000'0100u ) {
                //printf( "down right\n" );
                auto idx = (tile_y - 1) * width + tile_x + 1;
                if ( tmp[idx].value == 0 ) {
                    tmp[idx].value = value.value + 14;
                    tmp[idx].from = tile.from;
                    todo.push( { value.value + 14, idx } );
                }
            }
            if ( canReach & 0b0000'1000u ) {
                //printf( "right\n" );
                auto idx = (tile_y) * width + tile_x + 1;
                if ( tmp[idx].value == 0 ) {
                    tmp[idx].value = value.value + 10;
                    tmp[idx].from = tile.from;
                    todo.push( { value.value + 10, idx } );
                }
            }
            if ( canReach & 0b0001'0000u ) {
                //printf( "right up\n" );
                auto idx = (tile_y + 1) * width + tile_x + 1;
                if ( tmp[idx].value == 0 ) {
                    tmp[idx].value = value.value + 14;
                    tmp[idx].from = tile.from;
                    todo.push( { value.value + 14, idx } );
                }
            }
            if ( canReach & 0b0010'0000u ) {
                //printf( "up\n" );
                auto idx = (tile_y + 1) * width + tile_x;
                if ( tmp[idx].value == 0 ) {
                    tmp[idx].value = value.value + 10;
                    tmp[idx].from = tile.from;
                    todo.push( { value.value + 10, idx } );
                }
            }
            if ( canReach & 0b0100'0000u ) {
                //printf( "up left\n" );
                auto idx = (tile_y + 1) * width + tile_x - 1;
                if ( tmp[idx].value == 0 ) {
                    tmp[idx].value = value.value + 14;
                    tmp[idx].from = tile.from;
                    todo.push( { value.value + 14, idx } );
                }
            }
            if ( canReach & 0b1000'0000u ) {
                //printf( "left\n" );
                auto idx = (tile_y) * width + tile_x - 1;
                if ( tmp[idx].value == 0 ) {
                    tmp[idx].value = value.value + 10;
                    tmp[idx].from = tile.from;
                    todo.push( { value.value + 10, idx } );
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
            printf( "[Found route in %d waypoints\n", m_wayPoints.size() );

            m_targetX = x;
            m_targetY = y;

        } else {
            //printf( "No route!\n" );
        }

        delete[] tmp;
    }

    void game_unit::updateWorldSize() {
        if ( ((int)m_x) >= m_world->width() ) {
            m_x = m_world->width() - 1;
        }
        if ( ((int)m_y) >= m_world->height() ) {
            m_y = m_world->height() - 1;
        }
        m_targetX = m_x;
        m_targetY = m_y;
        m_wayPoints.clear();
    }

}