#include "MainWindow.h"

#include "game_map.h"
#include "game_unit.h"


const int ZOOMLEVELS[] = { 8, 12, 16, 20, 24, 32, 48, 64, 96, 128, 192, 256 };

void MainWindow::initEvent() {
    m_world = new isomap::game_map( m_width, m_height );
    m_zoom = 6;
    m_x = 0;
    m_y = 0;
    m_orientation = 0;

    sceneManager()->setCullingEnabled( false );

    m_unit = new isomap::game_unit( rendering(), m_world );

    for ( int i = 0 ; i < 100; ++i ) {
        m_units.push_back( new isomap::game_unit( rendering(), m_world ) );
    }
}

void MainWindow::resizeEvent(int w, int h) {
    rendering()->as<vl::Rendering>()->camera()->viewport()->set( 0, 0, w, h );
    updateProjection();
}


void MainWindow::keyPressEvent(unsigned short ch, vl::EKey key) {
    switch( key ) {
        case vl::Key_Left:
            m_width /= 2;
            if ( m_width < 8 ) {
                m_width = 8;
            }
            break;
        case vl::Key_Right:
            m_width *= 2;
            if ( m_width > 4096 ) {
                m_width = 4096;
            }
            break;

        case vl::Key_Up:
            m_height *= 2;
            if ( m_height > 4096 ) {
                m_height = 4096;
            }
            break;

        case vl::Key_Down:
            m_height /= 2;
            if ( m_height < 8 ) {
                m_height = 8;
            }
            break;

        case vl::Key_PageUp:
            m_smooth += 4;
            break;

        case vl::Key_PageDown:
            m_smooth -= 4;
            break;

        case vl::Key_Insert:    m_oreAmount += 4;   break;
        case vl::Key_Delete:    m_oreAmount -= 4;   break;
        case vl::Key_Home:      m_oreDensity += 4;  break;
        case vl::Key_End:       m_oreDensity -= 4;  break;

        case vl::Key_Space:
            m_paused = !m_paused;
            break;

        default:
            Applet::keyPressEvent( ch, key );
            break;
    }
}

void MainWindow::updateProjection() {
    int w = rendering()->as<vl::Rendering>()->camera()->viewport()->width();
    int h = rendering()->as<vl::Rendering>()->camera()->viewport()->height();
    vl::real hx = (w / (ZOOMLEVELS[m_zoom] / ::sqrt( 2.0 ) )) / 2.0;
    vl::real hy = (h / (ZOOMLEVELS[m_zoom] / ::sqrt( 2.0 ) )) / 2.0;
    rendering()->as<vl::Rendering>()->camera()->setProjectionOrtho(
            -hx,
            hx,
            -hy,
            hy,
            0.05,
            10000.0
    );
    updateCamera();
}

void MainWindow::updateCamera() {
    vl::real distance = m_width > m_height ? m_width : m_height;

    vl::real eye_x = cos( (m_angle - 45.0) * vl::dDEG_TO_RAD ) * distance;
    vl::real eye_y = sin( (m_angle - 45.0) * vl::dDEG_TO_RAD ) * distance;

    /* define the camera position and orientation */
    vl::real ratio = ::sqrt( 1.0 / 3.0 );

    // the center of any tile is at x.5, y.5
    vl::real x = m_x + m_x_off;
    vl::real y = m_y + m_y_off;

    vl::vec3 eye    = vl::vec3(x + eye_x, y + eye_y, distance * ratio); // camera position
    vl::vec3 center = vl::vec3(x, y,0);   // point the camera is looking at
    vl::vec3 up     = vl::vec3(0,0,1);   // up direction
    vl::mat4 view_mat = vl::mat4::getLookAt(eye, center, up);

    rendering()->as<vl::Rendering>()->camera()->setViewMatrix( view_mat );
}

// called every frame
void MainWindow::updateScene() {

    switch ( m_orientation ) {
        default:
        case 0:
            if ( m_angle != 0 ) {
                if (m_angle > 180) {
                    m_angle++;
                } else {
                    m_angle--;
                }
                if ( m_angle < 0 ) {
                    m_angle += 360;
                } else if ( m_angle >= 360 ) {
                    m_angle -= 360;
                }
                updateCamera();
            }
            break;
        case 1:
            if ( m_angle != 90 ) {
                if (m_angle > 270 || m_angle < 90) {
                    m_angle++;
                } else {
                    m_angle--;
                }
                if ( m_angle < 0 ) {
                    m_angle += 360;
                } else if ( m_angle >= 360 ) {
                    m_angle -= 360;
                }
                updateCamera();
            }
            break;
        case 2:
            if ( m_angle != 180 ) {
                if (m_angle < 180 ) {
                    m_angle++;
                } else {
                    m_angle--;
                }
                if ( m_angle < 0 ) {
                    m_angle += 360;
                } else if ( m_angle >= 360 ) {
                    m_angle -= 360;
                }
                updateCamera();
            }
            break;
        case 3:
            if ( m_angle != 270 ) {
                if (m_angle < 90 || m_angle > 270 ) {
                    m_angle--;
                } else {
                    m_angle++;
                }
                if ( m_angle < 0 ) {
                    m_angle += 360;
                } else if ( m_angle >= 360 ) {
                    m_angle -= 360;
                }
                updateCamera();
            }
            break;
    }

    static int seed = 0;
    if ( m_width != m_world->width() || m_height != m_world->height() ) {
        m_world->setSize(m_width, m_height);
        m_unit->updateWorldSize();
        for (isomap::game_unit *unit : m_units) {
            unit->updateWorldSize();
        }
    }
    m_world->generate( 5, (seed) / 256, m_smooth, m_oreAmount, m_oreDensity );

    sceneManager()->tree()->actors()->clear();

    vl::ref<vl::ResourceDatabase> resource_db = m_world->getDb();
    for(size_t ires=0; ires<resource_db->resources().size(); ++ires) {
        vl::Actor *act = resource_db->resources()[ires]->as<vl::Actor>();
        if (!act)
            continue;
        sceneManager()->tree()->addActor(act);
    }

    if ( !m_paused ) {
        m_world->update();
        m_unit->update(true);
        for (isomap::game_unit *unit : m_units) {
            if (unit->hasReachedTarget()) {
                static math::rng rnd(0);
                unit->moveTo(rnd(0, m_width - 1), rnd(0, m_height));
            }
            unit->update(true);
        }
    }
}

void MainWindow::zoomIn() {
    ++m_zoom;
    if ( m_zoom >= (sizeof(ZOOMLEVELS) / sizeof(ZOOMLEVELS[0])) ) {
        m_zoom = (sizeof(ZOOMLEVELS) / sizeof(ZOOMLEVELS[0])) - 1;
    }
    updateProjection();
}

void MainWindow::zoomOut() {
    --m_zoom;
    if ( m_zoom < 0 ) {
        m_zoom = 0;
    }
    updateProjection();
}

void MainWindow::rotateLeft() {
    ++m_orientation;
    m_orientation %= 4;
    updateCamera();
}

void MainWindow::rotateRight() {
    --m_orientation;
    m_orientation %= 4;
    updateCamera();
}

void MainWindow::left() {
    switch ( m_orientation ) {
        case 0: m_x++; break;
        case 1: m_y++; break;
        case 2: m_x--; break;
        case 3: m_y--; break;
        default: break;
    }
    updateCamera();
}

void MainWindow::up() {
    switch ( m_orientation ) {
        case 1: m_x++; break;
        case 2: m_y++; break;
        case 3: m_x--; break;
        case 0: m_y--; break;
        default: break;
    }
    updateCamera();
}


void MainWindow::right() {
    switch ( m_orientation ) {
        case 2: m_x++; break;
        case 3: m_y++; break;
        case 0: m_x--; break;
        case 1: m_y--; break;
        default: break;
    }
    updateCamera();
}


void MainWindow::down() {
    switch ( m_orientation ) {
        case 3: m_x++; break;
        case 0: m_y++; break;
        case 1: m_x--; break;
        case 2: m_y--; break;
        default: break;
    }
    updateCamera();
}

void MainWindow::screenToWorld(int screen_x, int screen_y, int& world_x, int& world_y ) {
    // get viewport size first
    int w = rendering()->as<vl::Rendering>()->camera()->viewport()->width();
    int h = rendering()->as<vl::Rendering>()->camera()->viewport()->height();

    // we'd like the center of the screen
    int hw = w / 2;
    int hh = h / 2;

    // get screen position relative to center of screen
    int dx = screen_x - hw;
    int dy = screen_y - hh;

    vl::real tile_width = ZOOMLEVELS[m_zoom];
    vl::real tile_height = tile_width / 2.0;

    // translate screen coordinates to tile coordinates
    vl::real wx = dx / tile_width;
    vl::real wy = dy / tile_height;

    // we have to offset the tile positions because the center of a tile is at the center of the screen
    // we have to use floor, otherwise both 0.5 and -0.5 will become 0. -0.5 should become -1
    int tx = (int)::floor(wy + wx + m_x_off);
    int ty = (int)::floor(wy - wx + m_y_off);

    // and then there's this monstrosity, don't ask...
    switch ( m_orientation ) {
        default:
        case 0: world_x = m_x + tx; world_y = m_y - ty; break;
        case 1: world_x = m_x + ty; world_y = m_y + tx; break;
        case 2: world_x = m_x - tx; world_y = m_y + ty; break;
        case 3: world_x = m_x - ty; world_y = m_y - tx; break;
    }
}

void MainWindow::highlight(int x, int y) {
    m_world->highlight( x, y );
    if ( m_world->isInside( x, y ) ) {
        m_unit->moveTo( x, y );
    }
}

void MainWindow::focusTileAt(int tile_x, int tile_y, int screen_x, int screen_y) {
    // move the camera so the given tile is at the given screen position
    int tx = 0;
    int ty = 0;
    screenToWorld( screen_x, screen_y, tx, ty );
    m_x -= tx - tile_x;
    m_y -= ty - tile_y;
    if ( m_x < 0 ) m_x = 0;
    if ( m_x >= m_width ) m_x = m_width - 1;
    if ( m_y < 0 ) m_y = 0;
    if ( m_y >= m_height ) m_y = m_height - 1;
    updateCamera();
}
