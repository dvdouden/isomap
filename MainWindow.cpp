#include "MainWindow.h"


#define MAP_WIDTH 128
#define MAP_HEIGHT 128

const int ZOOMLEVELS[] = { 8, 12, 16, 20, 24, 32, 48, 64, 96, 128, 192, 256 };

void MainWindow::initEvent() {
    world = new game_map( MAP_WIDTH, MAP_HEIGHT );
    m_zoom = 6;
    m_x = 0;
    m_y = 0;
    m_orientation = 0;
}

void dump(const vl::mat4 &m) {
    printf( "%5f %5f %5f %5f\n", m.e(0, 0), m.e(0, 1), m.e(0, 2), m.e(0, 3)  );
    printf( "%5f %5f %5f %5f\n", m.e(1, 0), m.e(1, 1), m.e(1, 2), m.e(1, 3)  );
    printf( "%5f %5f %5f %5f\n", m.e(2, 0), m.e(2, 1), m.e(2, 2), m.e(2, 3)  );
    printf( "%5f %5f %5f %5f\n", m.e(3, 0), m.e(3, 1), m.e(3, 2), m.e(3, 3)  );
    printf("\n" );
}

void MainWindow::resizeEvent(int w, int h) {
    rendering()->as<vl::Rendering>()->camera()->viewport()->set( 0, 0, w, h );
    updateProjection();
}


void MainWindow::keyPressEvent(unsigned short ch, vl::EKey key) {
    switch( key ) {
        case vl::Key_Left:
            left();
            break;
        case vl::Key_Right:
            right();
            break;

        case vl::Key_Up:
            up();
            break;

        case vl::Key_Down:
            down();
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
    vl::real distance = 100.0;
    vl::real eye_x, eye_y;
    switch ( m_orientation ) {
        default:
        case 0:
            eye_x = distance;
            eye_y = -distance;
            break;
        case 1:
            eye_x = distance;
            eye_y = distance;
            break;
        case 2:
            eye_x = -distance;
            eye_y = distance;
            break;
        case 3:
            eye_x = -distance;
            eye_y = -distance;
            break;
    }

    /* define the camera position and orientation */
    vl::real ratio = ::sqrt( 2.0 / 3.0 );

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
    static int seed = 0;
    world->generate( 5, (seed) / 256, 128 );

    sceneManager()->tree()->actors()->clear();

    vl::ref<vl::ResourceDatabase> resource_db = world->getDb();
    for(size_t ires=0; ires<resource_db->resources().size(); ++ires) {
        vl::Actor *act = resource_db->resources()[ires]->as<vl::Actor>();

        if (!act)
            continue;
        sceneManager()->tree()->addActor(act);
    }
/*
        if ( (seed % 1000) == 0) {
            printf("view matrix\n" );
            dump(rendering()->as<vl::Rendering>()->camera()->viewMatrix());
            printf("projection matrix\n" );
            dump(rendering()->as<vl::Rendering>()->camera()->projectionMatrix());

            printf( "viewport: %d %d\n\n",
                    rendering()->as<vl::Rendering>()->camera()->viewport()->width(),
                    rendering()->as<vl::Rendering>()->camera()->viewport()->height());
        }
*/

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
    --m_orientation;
    m_orientation %= 4;
    updateCamera();
}

void MainWindow::rotateRight() {
    ++m_orientation;
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
    world->highlight( x, y );
}

void MainWindow::focusTileAt(int tile_x, int tile_y, int screen_x, int screen_y) {
    // move the camera so the given tile is at the given screen position
    int tx = 0;
    int ty = 0;
    screenToWorld( screen_x, screen_y, tx, ty );
    m_x -= tx - tile_x;
    m_y -= ty - tile_y;
    if ( m_x < 0 ) m_x = 0;
    if ( m_x >= MAP_WIDTH ) m_x = MAP_WIDTH - 1;
    if ( m_y < 0 ) m_y = 0;
    if ( m_y >= MAP_HEIGHT ) m_y = MAP_HEIGHT - 1;
    updateCamera();
}
