#include <vlGraphics/FontManager.hpp>

#include "MainWindow.h"

#include "game_map.h"
#include "game_unit.h"

#include "server/Match.h"
#include "server/Player.h"
#include "server/Terrain.h"
#include "server/Unit.h"

#include "client/Terrain.h"
#include "client/Unit.h"

#include "common/TerrainMessage.h"
#include "common/UnitMessage.h"
#include "server/TerrainGenerator.h"


const int ZOOM_LEVELS[] = {2, 4, 8, 12, 16, 20, 24, 32, 48, 64, 96, 128, 192, 256};

void MainWindow::initEvent() {

    vl::ref<vl::Effect> text_fx = new vl::Effect;
    text_fx->shader()->enable( vl::EN_BLEND );
    m_text = new vl::Text;
    m_text->setFont( vl::defFontManager()->acquireFont( "/fonts/typed.ttf", 10 ) );
    m_text->setAlignment( vl::AlignLeft | vl::AlignTop );
    m_text->setViewportAlignment( vl::AlignLeft | vl::AlignTop );
    m_text->translate( 0, -10, 0 );
    sceneManager()->tree()->addActor( m_text.get(), text_fx.get() );
    updateText();

    m_zoomLevel = 8;
    m_zoom = ZOOM_LEVELS[m_zoomLevel];
    m_x = 0;
    m_y = 0;
    m_orientation = 0;

    sceneManager()->setCullingEnabled( false );

    m_match = new isomap::server::Match();
    regenerateMap();
    m_player = new isomap::server::Player();
    m_match->addPlayer( m_player );
    m_match->start();

    m_terrain = new isomap::client::Terrain();
    isomap::common::TerrainMessage* msg = m_match->terrain()->createMessage();
    m_terrain->processMessage( msg );
    delete msg;

    m_player->unFog( 10, 10, 20 );
    msg = m_player->createTerrainMessage();
    m_terrain->processMessage( msg );
    delete msg;

    m_terrain->initRender( rendering() );

    m_serverUnit = new isomap::server::Unit( m_player, nullptr );
    m_match->addObject( m_serverUnit );
    m_clientUnit = new isomap::client::Unit;
    m_clientUnit->initRender( rendering() );
}

void MainWindow::resizeEvent( int w, int h ) {
    rendering()->as<vl::Rendering>()->camera()->viewport()->set( 0, 0, w, h );
    updateProjection();
}


void MainWindow::keyPressEvent( unsigned short ch, vl::EKey key ) {
    switch ( key ) {
        case vl::Key_Left:
            if ( m_heightScale > 2 ) {
                m_heightScale--;
                regenerateMap();
            }
            break;
        case vl::Key_Right:
            if ( m_heightScale < 12 ) {
                m_heightScale++;
                regenerateMap();
            }
            break;

        case vl::Key_Up:
            if ( m_heightNoise < 252 ) {
                m_heightNoise += 4;
                regenerateMap();
            }
            break;

        case vl::Key_Down:
            if ( m_heightNoise > 0 ) {
                m_heightNoise -= 4;
                regenerateMap();
            }
            break;

        case vl::Key_PageUp:
            if ( m_cliffThreshold < 252 ) {
                m_cliffThreshold += 4;
                regenerateMap();
            }
            break;

        case vl::Key_PageDown:
            if ( m_cliffThreshold > 0 ) {
                m_cliffThreshold -= 4;
                regenerateMap();
            }
            break;

        case vl::Key_Insert:
            if ( m_cliffNoise < 252 ) {
                m_cliffNoise += 4;
                regenerateMap();
            }
            break;
        case vl::Key_Delete:
            if ( m_cliffNoise > 0 ) {
                m_cliffNoise -= 4;
                regenerateMap();
            }
            break;
        case vl::Key_Home:
            if ( m_cliffScale < 10 ) {
                ++m_cliffScale;
                regenerateMap();
            }
            break;
        case vl::Key_End:
            if ( m_cliffScale > 2 ) {
                --m_cliffScale;
                regenerateMap();
            }
            break;

        case vl::Key_Minus:
            if ( m_oreScale > 2 ) {
                --m_oreScale;
                regenerateMap();
            }
            break;

        case vl::Key_Equal:
            if ( m_oreScale < 10 ) {
                ++m_oreScale;
                regenerateMap();
            }
            break;

        case vl::Key_LeftBracket:
            if ( m_oreNoise > 0 ) {
                m_oreNoise -= 4;
                regenerateMap();
            }
            break;

        case vl::Key_RightBracket:
            if ( m_oreNoise < 252 ) {
                m_oreNoise += 4;
                regenerateMap();
            }
            break;

        case vl::Key_Semicolon:
            if ( m_oreThreshold > 0 ) {
                m_oreThreshold -= 4;
                regenerateMap();
            }
            break;

        case vl::Key_Quote:
            if ( m_oreThreshold < 252 ) {
                m_oreThreshold += 4;
                regenerateMap();
            }
            break;

        case vl::Key_Comma:
            if ( m_oreDensity > 0 ) {
                m_oreDensity -= 4;
                regenerateMap();
            }
            break;

        case vl::Key_Period:
            if ( m_oreDensity < 252 ) {
                m_oreDensity += 4;
                regenerateMap();
            }
            break;

        case vl::Key_1:
            if ( m_shoreCount > 0 ) {
                m_shoreCount--;
                regenerateMap();
            }
            break;

        case vl::Key_2:
            if ( m_shoreCount < 4 ) {
                m_shoreCount++;
                regenerateMap();
            }
            break;

        case vl::Key_Space:
            m_paused = !m_paused;
            break;

        case vl::Key_Return:
            m_match->update();
            break;

        case vl::Key_BackSpace: {
            auto* msg = m_match->terrain()->uncoverAll();
            m_terrain->processMessage( msg );
            delete msg;
        }
            break;

        case vl::Key_Slash:
            m_terrain->toggleRenderFog();
            break;


        default:
            Applet::keyPressEvent( ch, key );
            break;
    }
}

void MainWindow::updateProjection() {
    int w = rendering()->as<vl::Rendering>()->camera()->viewport()->width();
    int h = rendering()->as<vl::Rendering>()->camera()->viewport()->height();
    vl::real hx = (w / (m_zoom / ::sqrt( 2.0 ))) / 2.0;
    vl::real hy = (h / (m_zoom / ::sqrt( 2.0 ))) / 2.0;
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

    vl::vec3 eye = vl::vec3( x + eye_x, y + eye_y, distance * ratio ); // camera position
    vl::vec3 center = vl::vec3( x, y, 0 );   // point the camera is looking at
    vl::vec3 up = vl::vec3( 0, 0, 1 );   // up direction
    vl::mat4 view_mat = vl::mat4::getLookAt( eye, center, up );

    rendering()->as<vl::Rendering>()->camera()->setViewMatrix( view_mat );
}

// called every frame
void MainWindow::updateScene() {

    // smooth rotation
    switch ( m_orientation ) {
        default:
        case 0:
            if ( m_angle != 0 ) {
                if ( m_angle > 180 ) {
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
                if ( m_angle > 270 || m_angle < 90 ) {
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
                if ( m_angle < 180 ) {
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
                if ( m_angle < 90 || m_angle > 270 ) {
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
    // smooth zooming
    if ( m_zoom != ZOOM_LEVELS[m_zoomLevel] ) {
        m_zoom += (ZOOM_LEVELS[m_zoomLevel] - m_zoom) / 4.0;
        static int updateCount = 0;
        ++updateCount;
        if ( updateCount > 30 ) {
            updateCount = 0;
            m_zoom = ZOOM_LEVELS[m_zoomLevel];
        }
        updateProjection();
    }
    updateText();

    m_match->update();

    isomap::common::UnitServerMessage* unitMsg = m_serverUnit->statusMessage();
    m_clientUnit->processMessage( unitMsg );
    delete unitMsg;

    isomap::common::TerrainMessage* terrainMessage = m_player->createTerrainMessage();
    m_terrain->processMessage( terrainMessage );
    delete terrainMessage;

    m_terrain->updateFog();
    m_terrain->render();
    m_clientUnit->render();
}

void MainWindow::zoomIn() {
    ++m_zoomLevel;
    if ( m_zoomLevel >= (sizeof( ZOOM_LEVELS ) / sizeof( ZOOM_LEVELS[0] )) ) {
        m_zoomLevel = (sizeof( ZOOM_LEVELS ) / sizeof( ZOOM_LEVELS[0] )) - 1;
    }
    updateProjection();
}

void MainWindow::zoomOut() {
    --m_zoomLevel;
    if ( m_zoomLevel < 0 ) {
        m_zoomLevel = 0;
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

void MainWindow::screenToWorld( int screen_x, int screen_y, int& world_x, int& world_y ) {
    // get viewport size first
    int w = rendering()->as<vl::Rendering>()->camera()->viewport()->width();
    int h = rendering()->as<vl::Rendering>()->camera()->viewport()->height();

    // we'd like the center of the screen
    int hw = w / 2;
    int hh = h / 2;

    // get screen position relative to center of screen
    int dx = screen_x - hw;
    int dy = screen_y - hh;

    vl::real tile_width = ZOOM_LEVELS[m_zoomLevel];
    vl::real tile_height = tile_width / 2.0;

    // translate screen coordinates to tile coordinates
    vl::real wx = dx / tile_width;
    vl::real wy = dy / tile_height;

    // we have to offset the tile positions because the center of a tile is at the center of the screen
    // we have to use floor, otherwise both 0.5 and -0.5 will become 0. -0.5 should become -1
    int tx = (int)::floor( wy + wx + m_x_off );
    int ty = (int)::floor( wy - wx + m_y_off );

    // and then there's this monstrosity, don't ask...
    switch ( m_orientation ) {
        default:
        case 0:
            world_x = m_x + tx;
            world_y = m_y - ty;
            break;
        case 1:
            world_x = m_x + ty;
            world_y = m_y + tx;
            break;
        case 2:
            world_x = m_x - tx;
            world_y = m_y + ty;
            break;
        case 3:
            world_x = m_x - ty;
            world_y = m_y - tx;
            break;
    }
}

void MainWindow::highlight( int x, int y ) {
    isomap::common::UnitCommandMessage* comMsg = m_clientUnit->moveTo( x, y );
    m_serverUnit->processMessage( comMsg );
    delete comMsg;
}

void MainWindow::focusTileAt( int tile_x, int tile_y, int screen_x, int screen_y ) {
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

void MainWindow::regenerateMap() {
    isomap::server::TerrainGenerator generator;
    generator.setShoreCount( m_shoreCount );

    generator.setHeightScale( m_heightScale );
    generator.setHeightNoise( m_heightNoise );

    generator.setCliffScale( m_cliffScale );
    generator.setCliffNoise( m_cliffNoise );
    generator.setCliffThreshold( m_cliffThreshold );

    generator.setOreScale( m_oreScale );
    generator.setOreNoise( m_oreNoise );
    generator.setOreThreshold( m_oreThreshold );
    generator.setOreDensity( m_oreDensity );
    m_match->generateWorld( m_width, m_height, &generator );

    if ( m_terrain ) {
        auto* msg = m_match->terrain()->uncoverAll();
        m_terrain->processMessage( msg );
        delete msg;
    }
}

void MainWindow::updateText() {
    m_text->setText( vl::Say( "FPS %n\n"
                              "Shore count %n\n"
                              "Height map scale %n\n"
                              "Height map noise %n\n"
                              "Cliff scale %n\n"
                              "Cliff noise %n\n"
                              "Cliff threshold %n\n"
                              "Ore scale %n\n"
                              "Ore noise %n\n"
                              "Ore threshold %n\n"
                              "Ore density %n" ) << fps() << m_shoreCount << (1 << m_heightScale) << m_heightNoise
                                                 << (1 << m_cliffScale) << m_cliffNoise << m_cliffThreshold
                                                 << (1 << m_oreScale) << m_oreNoise << m_oreThreshold << m_oreDensity );
}

