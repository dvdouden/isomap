#include <vlGraphics/FontManager.hpp>

#include "MainWindow.h"

#include "game_map.h"
#include "game_unit.h"

#include "server/Match.h"
#include "server/Player.h"
#include "server/Terrain.h"
#include "server/Unit.h"

#include "client/Player.h"
#include "client/Terrain.h"
#include "client/Unit.h"

#include "common/PlayerMessage.h"
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

    m_serverMatch = new isomap::server::Match();
    regenerateMap();

    m_clientTerrain = new isomap::client::Terrain();
    isomap::common::TerrainMessage* msg = m_serverMatch->terrain()->createMessage();
    m_clientTerrain->processMessage( msg );
    delete msg;

    // FIXME: this sucks, should be changed
    m_serverPlayer = new isomap::server::Player( m_serverMatch );
    m_serverMatch->addPlayer( m_serverPlayer );
    m_serverMatch->start();


    m_serverPlayer->unFog( 10, 10, 20 );
    msg = m_serverPlayer->createTerrainMessage();
    m_clientTerrain->processMessage( msg );
    delete msg;

    m_clientTerrain->initRender( rendering() );

    m_clientPlayer = new isomap::client::Player( m_clientTerrain );
    m_clientPlayer->initRender( rendering() );
    printf( "init rendering\n" );
    auto* playerCmd = m_clientPlayer->buildUnit( 0, 0 );
    printf( "build unit\n" );
    m_serverPlayer->processMessage( playerCmd );
    printf( "server processed message\n" );
    delete playerCmd;
    playerCmd = m_clientPlayer->buildStructure( 10, 10 );
    printf( "build structure\n" );
    m_serverPlayer->processMessage( playerCmd );
    printf( "server processed message\n" );
    delete playerCmd;

    playerCmd = m_clientPlayer->buildStructure( 13, 10 );
    printf( "build structure\n" );
    m_serverPlayer->processMessage( playerCmd );
    printf( "server processed message\n" );
    delete playerCmd;

    for ( auto* playerMsg : m_serverPlayer->serverMessages() ) {
        printf( "got player server message\n" );
        m_clientPlayer->processMessage( playerMsg );
        printf( "client player processed server message\n" );
        delete playerMsg;
    }
    printf( "init done\n" );

}

void MainWindow::resizeEvent( int w, int h ) {
    rendering()->as<vl::Rendering>()->camera()->viewport()->set( 0, 0, w, h );
    updateProjection();
}


void MainWindow::keyPressEvent( unsigned short ch, vl::EKey key ) {
    switch ( key ) {
        case vl::Key_Left:
            if ( m_generator.heightScale() > 2 ) {
                m_generator.setHeightScale( m_generator.heightScale() - 1 );
                regenerateMap();
            }
            break;
        case vl::Key_Right:
            if ( m_generator.heightScale() < 12 ) {
                m_generator.setHeightScale( m_generator.heightScale() + 1 );
                regenerateMap();
            }
            break;

        case vl::Key_Up:
            if ( m_generator.heightNoise() < 252 ) {
                m_generator.setHeightNoise( m_generator.heightNoise() + 4 );
                regenerateMap();
            }
            break;

        case vl::Key_Down:
            if ( m_generator.heightNoise() > 0 ) {
                m_generator.setHeightNoise( m_generator.heightNoise() - 4 );
                regenerateMap();
            }
            break;

        case vl::Key_PageUp:
            if ( m_generator.cliffThreshold() < 252 ) {
                m_generator.setCliffThreshold( m_generator.cliffThreshold() + 4 );
                regenerateMap();
            }
            break;

        case vl::Key_PageDown:
            if ( m_generator.cliffThreshold() > 0 ) {
                m_generator.setCliffThreshold( m_generator.cliffThreshold() - 4 );
                regenerateMap();
            }
            break;

        case vl::Key_Insert:
            if ( m_generator.cliffNoise() < 252 ) {
                m_generator.setCliffNoise( m_generator.cliffNoise() + 4 );
                regenerateMap();
            }
            break;
        case vl::Key_Delete:
            if ( m_generator.cliffNoise() > 0 ) {
                m_generator.setCliffNoise( m_generator.cliffNoise() - 4 );
                regenerateMap();
            }
            break;
        case vl::Key_Home:
            if ( m_generator.cliffScale() < 10 ) {
                m_generator.setCliffScale( m_generator.cliffScale() + 1 );
                regenerateMap();
            }
            break;
        case vl::Key_End:
            if ( m_generator.cliffScale() > 2 ) {
                m_generator.setCliffScale( m_generator.cliffScale() - 1 );
                regenerateMap();
            }
            break;

        case vl::Key_Minus:
            if ( m_generator.oreScale() > 2 ) {
                m_generator.setOreScale( m_generator.oreScale() - 1 );
                regenerateMap();
            }
            break;

        case vl::Key_Equal:
            if ( m_generator.oreScale() < 10 ) {
                m_generator.setOreScale( m_generator.oreScale() + 1 );
                regenerateMap();
            }
            break;

        case vl::Key_LeftBracket:
            if ( m_generator.oreNoise() > 0 ) {
                m_generator.setOreNoise( m_generator.oreNoise() - 4 );
                regenerateMap();
            }
            break;

        case vl::Key_RightBracket:
            if ( m_generator.oreNoise() < 252 ) {
                m_generator.setOreNoise( m_generator.oreNoise() + 4 );
                regenerateMap();
            }
            break;

        case vl::Key_Semicolon:
            if ( m_generator.oreThreshold() > 0 ) {
                m_generator.setOreThreshold( m_generator.oreThreshold() - 4 );
                regenerateMap();
            }
            break;

        case vl::Key_Quote:
            if ( m_generator.oreThreshold() < 252 ) {
                m_generator.setOreThreshold( m_generator.oreThreshold() + 4 );
                regenerateMap();
            }
            break;

        case vl::Key_Comma:
            if ( m_generator.oreDensity() > 0 ) {
                m_generator.setOreDensity( m_generator.oreDensity() - 4 );
                regenerateMap();
            }
            break;

        case vl::Key_Period:
            if ( m_generator.oreDensity() < 252 ) {
                m_generator.setOreDensity( m_generator.oreDensity() + 4 );
                regenerateMap();
            }
            break;

        case vl::Key_1:
            m_generator.setShoreBits( (m_generator.shoreBits() - 1u) & 0x0Fu );
            regenerateMap();
            break;

        case vl::Key_2:
            m_generator.setShoreBits( (m_generator.shoreBits() + 1u) & 0x0Fu );
            regenerateMap();
            break;

        case vl::Key_3:
            if ( m_generator.maxHeight() > 0 ) {
                m_generator.setMaxHeight( m_generator.maxHeight() - 1 );
                if ( m_generator.minHeight() > m_generator.maxHeight() ) {
                    m_generator.setMinHeight( m_generator.maxHeight() );
                }
                regenerateMap();
            }
            break;

        case vl::Key_4:
            if ( m_generator.maxHeight() < 255 ) {
                m_generator.setMaxHeight( m_generator.maxHeight() + 1 );
                regenerateMap();
            }
            break;

        case vl::Key_5:
            if ( m_generator.minHeight() > 0 ) {
                m_generator.setMinHeight( m_generator.minHeight() - 1 );
                regenerateMap();
            }
            break;

        case vl::Key_6:
            if ( m_generator.minHeight() < 255 ) {
                m_generator.setMinHeight( m_generator.minHeight() + 1 );
                if ( m_generator.maxHeight() < m_generator.minHeight() ) {
                    m_generator.setMaxHeight( m_generator.minHeight() );
                }
                regenerateMap();
            }
            break;

        case vl::Key_7:
            if ( m_generator.terrainHeight() > 0 ) {
                m_generator.setTerrainHeight( m_generator.terrainHeight() - 1 );
                regenerateMap();
            }
            break;

        case vl::Key_8:
            if ( m_generator.terrainHeight() < 255 ) {
                m_generator.setTerrainHeight( m_generator.terrainHeight() + 1 );
                regenerateMap();
            }
            break;

        case vl::Key_9:
            m_generator.setSeed( (m_generator.seed() - 1u) & 0xFFu );
            regenerateMap();
            break;

        case vl::Key_0:
            m_generator.setSeed( (m_generator.seed() + 1u) & 0xFFu );
            regenerateMap();
            break;

        case vl::Key_Y:
            if ( m_generator.waterDepth() > 0 ) {
                m_generator.setWaterDepth( m_generator.waterDepth() - 1 );
                regenerateMap();
            }
            break;

        case vl::Key_U:
            if ( m_generator.waterDepth() < 64 ) {
                m_generator.setWaterDepth( m_generator.waterDepth() + 1 );
                regenerateMap();
            }
            break;

        case vl::Key_Space:
            m_paused = !m_paused;
            break;

        case vl::Key_Return:
            m_serverMatch->update();
            break;

        case vl::Key_BackSpace: {
            auto* msg = m_serverMatch->terrain()->uncoverAll();
            m_clientTerrain->processMessage( msg );
            delete msg;
        }
            break;

        case vl::Key_BackSlash:
            m_clientTerrain->toggleRenderFog();
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

    m_serverMatch->update();

    for ( auto* playerMsg : m_serverPlayer->serverMessages() ) {
        m_clientPlayer->processMessage( playerMsg );
        delete playerMsg;
    }
/*
    isomap::common::UnitServerMessage* unitMsg = m_serverUnit->statusMessage();
    m_clientUnit->processMessage( unitMsg );
    delete unitMsg;
*/
    isomap::common::TerrainMessage* terrainMessage = m_serverPlayer->createTerrainMessage();
    m_clientTerrain->processMessage( terrainMessage );
    delete terrainMessage;

    m_clientTerrain->updateFog();
    m_clientTerrain->render();
    m_clientPlayer->render();
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
    /*isomap::common::UnitCommandMessage* comMsg = m_clientUnit->moveTo( x, y );
    m_serverUnit->processMessage( comMsg );
    delete comMsg;*/
    if ( m_clientPlayer->canPlace( x, y, 2, 3 ) ) {
        auto* playerCmd = m_clientPlayer->buildStructure( x, y );
        m_serverPlayer->processMessage( playerCmd );
        delete playerCmd;
    }
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
    m_serverMatch->generateWorld( m_width, m_height, &m_generator );

    if ( m_clientTerrain ) {
        auto* msg = m_serverMatch->terrain()->uncoverAll();
        m_clientTerrain->processMessage( msg );
        delete msg;
    }
}

void MainWindow::updateText() {
    m_text->setText( vl::Say( "FPS %n\n"
                              "Seed %n\n"
                              "Shore bits %n\n"
                              "Terrain height %n\n"
                              "Water depth %n\n"
                              "Height map scale %n\n"
                              "Height map noise %n\n"
                              "Min height %n\n"
                              "Max height %n\n"
                              "Cliff scale %n\n"
                              "Cliff noise %n\n"
                              "Cliff threshold %n\n"
                              "Ore scale %n\n"
                              "Ore noise %n\n"
                              "Ore threshold %n\n"
                              "Ore density %n" ) << fps() << m_generator.seed() << m_generator.shoreBits()
                                                 << m_generator.terrainHeight() << m_generator.waterDepth()
                                                 << (1u << m_generator.heightScale())
                                                 << m_generator.heightNoise()
                                                 << m_generator.minHeight() << m_generator.maxHeight()
                                                 << (1u << m_generator.cliffScale()) << m_generator.cliffNoise()
                                                 << m_generator.cliffThreshold()
                                                 << (1u << m_generator.oreScale()) << m_generator.oreNoise()
                                                 << m_generator.oreThreshold() << m_generator.oreDensity() );
}

