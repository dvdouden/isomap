#include <vlGraphics/FontManager.hpp>
#include <thread>

#include "MainWindow.h"

#include "server/Match.h"
#include "server/Player.h"
#include "server/Structure.h"
#include "server/Terrain.h"
#include "server/Unit.h"

#include "client/Match.h"
#include "client/Player.h"
#include "client/Terrain.h"
#include "client/Unit.h"

#include "common/StructureType.h"
#include "server/TerrainGenerator.h"


const int ZOOM_LEVELS[] = {2, 4, 8, 12, 16, 20, 24, 32, 48, 64, 96, 128, 192, 256};

void MainWindow::initEvent() {

    isomap::common::StructureType::load();
    isomap::common::UnitType::load();

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

    // this is a bit yucky, but we need a server and two clients (human player and AI)
    m_serverMatch = new isomap::server::Match();
    regenerateMap();

    m_clientPlayerMatch = new isomap::client::Match( 0x12345678 );
    m_clientPlayerMatch->setRenderer( new isomap::client::match::Renderer( m_clientPlayerMatch ) );
    m_clientPlayerMatch->renderer()->init( rendering() );
    m_clientPlayerMatch->registerPlayer( "Player1" );
    sendAndReceiveMessages();
    m_clientPlayerMatch->startMatch();
    sendAndReceiveMessages();

    m_clientAIMatch = new isomap::client::Match( 0xA1A1A1A1 );
    m_clientAIMatch->setRenderer( new isomap::client::match::Renderer( m_clientAIMatch ) );
    m_clientAIMatch->renderer()->init( rendering() );
    m_clientAIMatch->registerPlayer( "AI" );
    sendAndReceiveMessages();
    m_clientAIMatch->startMatch();
    sendAndReceiveMessages();

    // FIXME: make sure player is not null
    m_clientPlayer = m_clientPlayerMatch->player();
    m_clientAI = m_clientAIMatch->player();

    m_clientPlayerMatch->renderer()->disable();
    m_clientAIMatch->renderer()->disable();
    m_renderMatch = m_clientPlayerMatch;
    m_renderMatch->renderer()->enable();
    m_renderTerrain = m_renderMatch->terrain();
    m_controllingPlayer = m_renderMatch->player();

    // we need to uncover this bit of terrain, otherwise construction will end up underground...
    m_serverMatch->getPlayer( m_clientPlayer->id() )->unFog( 10, 10, 10 );
    m_serverMatch->getPlayer( m_clientAI->id() )->unFog( 30, 10, 10 );
    sendMessages();
    m_serverMatch->update();
    receiveMessages();

    m_clientPlayer->buildUnit( 8, 8, isomap::common::UnitType::get( 1 ), 0 );
    m_clientPlayer->buildStructure( 10, 10, isomap::common::StructureType::get( 1 ), m_structureOrientation );
    m_clientAI->buildUnit( 28, 8, isomap::common::UnitType::get( 1 ), 0 );
    m_clientAI->buildStructure( 30, 10, isomap::common::StructureType::get( 2 ), m_structureOrientation );

    sendAndReceiveMessages();
    printf( "init done\n" );
}


void MainWindow::destroyEvent() {
    delete m_serverMatch;
    delete m_clientPlayerMatch;
    delete m_clientAIMatch;
    isomap::common::UnitType::clear();
    isomap::common::StructureType::clear();
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

        case vl::Key_BackSpace:
            m_serverMatch->getPlayer( m_controllingPlayer->id() )->uncoverAll();
            break;

        case vl::Key_BackSlash:
            m_renderTerrain->renderer()->toggleRenderFog();
            break;

        case vl::Key_F2:
            m_renderColumn = !m_renderColumn;
            break;

        case vl::Key_F3:
            m_renderTerrain->renderer()->toggleRenderOccupancy();
            break;

        case vl::Key_F6:
            m_renderMatch->renderer()->disable();
            m_renderMatch = m_clientPlayerMatch;
            m_renderMatch->renderer()->enable();
            m_renderTerrain = m_renderMatch->terrain();
            m_controllingPlayer = m_renderMatch->player();
            break;

        case vl::Key_F7:
            m_renderMatch->renderer()->disable();
            m_renderMatch = m_clientAIMatch;
            m_renderMatch->renderer()->enable();
            m_renderTerrain = m_renderMatch->terrain();
            m_controllingPlayer = m_renderMatch->player();
            break;

        case vl::Key_F8:
            m_renderMatch->renderer()->dumpActors();
            break;

        case vl::Key_F9:
            //m_serverMatch->dump();
            m_controllingPlayer->dump();
            break;

        case vl::Key_Z:
            if ( m_structureType > 1 ) {
                m_structureType--;
            }
            break;

        case vl::Key_X:
            if ( m_structureType < 5 ) {
                m_structureType++;
            }
            break;

        case vl::Key_A:
            m_structureOrientation--;
            if ( m_structureOrientation < 0 ) {
                m_structureOrientation += 4;
            }
            break;

        case vl::Key_S:
            m_structureOrientation++;
            if ( m_structureOrientation >= 4 ) {
                m_structureOrientation -= 4;
            }
            break;

        case vl::Key_B:
            m_mode = PlaceStructure;
            break;

        case vl::Key_V:
            m_mode = DeleteStructure;
            break;

        case vl::Key_M:
            if ( m_controllingPlayer->getUnit( m_selectedUnit ) != nullptr ) {
                m_mode = MoveUnit;
            } else {
                m_mode = SelectUnit;
            }
            break;

        case vl::Key_N:
            m_mode = SelectUnit;
            break;

        case vl::Key_G:
            m_mode = PlaceUnit;
            break;

        case vl::Key_F:
            m_mode = DeleteUnit;
            break;

        default:
            Applet::keyPressEvent( ch, key );
            break;
    }
}

void MainWindow::keyReleaseEvent( unsigned short ch, vl::EKey key ) {
    switch ( key ) {
        // we want to use these keys for other purposes (and it'll break stuff if we let the default behavior kick in)
        case vl::Key_F: // camera controls
        case vl::Key_T: // don't touch my camera!
            break;

        default:
            Applet::keyReleaseEvent( ch, key );
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

    sendMessages();
    m_serverMatch->update();
    receiveMessages();
    for ( auto* match : {m_clientAIMatch, m_clientPlayerMatch} ) {
        match->update();
    }

    m_renderTerrain->updateFog();
    m_renderMatch->renderer()->render();
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
    m_renderTerrain->renderer()->clearHighlight();

    // get viewport size first
    int w = rendering()->as<vl::Rendering>()->camera()->viewport()->width();
    int h = rendering()->as<vl::Rendering>()->camera()->viewport()->height();

    // we'd like the center of the screen
    int hw = w / 2;
    int hh = h / 2;

    // get screen position relative to center of screen
    int dx = screen_x - hw;
    int dy = -(screen_y - hh);

    vl::real tile_width = ZOOM_LEVELS[m_zoomLevel];
    vl::real tile_height = tile_width / 2.0;

    // translate screen coordinates to tile coordinates
    vl::real wx = dx / tile_width;
    vl::real wy = dy / tile_height;

    vl::real tile_x = -wy + wx;
    vl::real tile_y = -wy - wx;

    vl::real worldTileX, worldTileY;
    // now we need to compensate for the camera offset and orientation
    switch ( m_orientation ) {
        default:
        case 0:
            worldTileX = (tile_x + m_x) + m_x_off;
            worldTileY = (-tile_y + m_y) + m_y_off;
            break;
        case 1:
            worldTileX = (tile_y + m_x) + m_y_off;
            worldTileY = (tile_x + m_y) + m_x_off;
            break;
        case 2:
            worldTileX = (-tile_x + m_x) + m_x_off;
            worldTileY = (tile_y + m_y) + m_y_off;
            break;
        case 3:
            worldTileX = (-tile_y + m_x) + m_y_off;
            worldTileY = (-tile_x + m_y) + m_x_off;
            break;
    }

    vl::real sub_x = worldTileX - ::floor( worldTileX );
    vl::real sub_y = worldTileY - ::floor( worldTileY );

    world_x = (int)worldTileX;
    world_y = (int)worldTileY;

    // our current world coordinates do not take the height of the tile into account, it assumes each tile has height 0
    // in order to fix that, we need to look at a few more tiles towards the camera
    //m_clientPlayerTerrain->addHighlight( isomap::client::Terrain::Area( world_x, world_y, 1, 1 ), vl::red );

    int x_inc = 0;
    int y_inc = 0;
    int temp_x = world_x;
    int temp_y = world_y;
    // first we need to figure out in which direction we should be looking
    bool x_first;
    switch ( m_orientation ) {
        default:
        case 0:
            x_inc = 1;
            y_inc = -1;
            x_first = sub_x + sub_y >= 1.0;
            break;
        case 1:
            x_inc = 1;
            y_inc = 1;
            x_first = sub_x >= sub_y;
            break;
        case 2:
            x_inc = -1;
            y_inc = 1;
            x_first = sub_x + sub_y <= 1.0;
            break;
        case 3:
            x_inc = -1;
            y_inc = -1;
            x_first = sub_x <= sub_y;
            break;
    }
    int c0 = (0u + m_orientation) % 4u;
    int c1 = (3u + m_orientation) % 4u;
    int c2 = (2u + m_orientation) % 4u;

    for ( int i = 0; i < 16; ++i ) {
        // for each tile, we need to find the top line
        // translate the two corners to screen space
        // then find the last tile for which the screen coordinates fall below the line
        if ( temp_x >= 0 && temp_x < m_width && temp_y >= 0 && temp_y < m_height ) {
            int x0, y0, x1, y1;

            if ( x_first == ((m_orientation % 2) == 0) ) {
                worldToScreen( temp_x, temp_y, m_renderTerrain->getCorner( temp_x, temp_y, c1 ), c1, x0, y0 );
                worldToScreen( temp_x, temp_y, m_renderTerrain->getCorner( temp_x, temp_y, c2 ), c2, x1, y1 );
            } else {
                worldToScreen( temp_x, temp_y, m_renderTerrain->getCorner( temp_x, temp_y, c0 ), c0, x0, y0 );
                worldToScreen( temp_x, temp_y, m_renderTerrain->getCorner( temp_x, temp_y, c1 ), c1, x1, y1 );
            }
            vl::fvec4 color;
            if ( isBelow( dx, dy, x0, y0, x1, y1 ) ) {
                color = vl::green;
                world_x = temp_x;
                world_y = temp_y;
            } else {
                color = vl::red;
            }
            if ( m_renderColumn ) {
                m_renderTerrain->renderer()->addHighlight( isomap::common::Area( temp_x, temp_y, 1, 1 ), color );
            }
        }

        if ( x_first ) {
            temp_x += x_inc;
        } else {
            temp_y += y_inc;
        }

        x_first = !x_first;
    }

}


bool MainWindow::isBelow( int dx, int dy, int x0, int y0, int x1, int y1 ) {
    dx -= x0;
    dy -= y0;
    x1 -= x0;
    y1 -= y0;
    if ( dy <= 0 && dy <= y1 ) {
        return true;
    }
    if ( dy > 0 && dy > y1 ) {
        return false;
    }
    return dy <= ((dx * y1) / x1);
}

void MainWindow::worldToScreen( int world_x, int world_y, int world_z, int& screen_x, int& screen_y ) {
    int tile_x, tile_y;
    switch ( m_orientation ) {
        default:
        case 0:
            tile_x = world_x - m_x;
            tile_y = -world_y + m_y;
            break;
        case 1:
            tile_y = world_x - m_x;
            tile_x = world_y - m_y;
            break;
        case 2:
            tile_x = -world_x + m_x;
            tile_y = world_y - m_y;
            break;
        case 3:
            tile_y = -world_x + m_x;
            tile_x = -world_y + m_y;
            break;
    }

    screen_x = ((tile_x - tile_y) * ZOOM_LEVELS[m_zoomLevel] / 2);
    screen_y = -((tile_x + tile_y) * ZOOM_LEVELS[m_zoomLevel] / 4);
    screen_y += world_z * ZOOM_LEVELS[m_zoomLevel] / 4;
}

void MainWindow::worldToScreen( int world_x, int world_y, int world_z, int corner, int& screen_x, int& screen_y ) {
    worldToScreen( world_x, world_y, world_z, screen_x, screen_y );
    int x_off = 0, y_off = 0;
    switch ( ((4 + corner) - m_orientation) % 4 ) {
        default:
        case 0:
            x_off = -ZOOM_LEVELS[m_zoomLevel] / 2;
            break;
        case 1:
            y_off = -ZOOM_LEVELS[m_zoomLevel] / 4;
            break;
        case 2:
            x_off = ZOOM_LEVELS[m_zoomLevel] / 2;
            break;
        case 3:
            y_off = ZOOM_LEVELS[m_zoomLevel] / 4;
            break;
    }
    screen_x += x_off;
    screen_y += y_off;
}

void MainWindow::highlight( int x, int y ) {
    m_cursorX = x;
    m_cursorY = y;
    switch ( m_mode ) {
        case PlaceStructure:
            renderStructurePlacement( x, y );
            break;

        case DeleteStructure:
            if ( x >= 0 && x < m_renderTerrain->width() && y >= 0 && y < m_renderTerrain->height() ) {
                auto* structure = m_renderTerrain->getStructureAt( x, y );
                if ( structure != nullptr ) {
                    highlightStructure( structure, structure->player()->id() == m_controllingPlayer->id() );
                } else {
                    highlightTile( x, y, false );
                }
            }
            break;

        case PlaceUnit:
            if ( x >= 0 && x < m_renderMatch->terrain()->width() && y >= 0 && y < m_renderMatch->terrain()->height() ) {
                if ( m_renderTerrain->isVisible( x, y ) ) {
                    highlightTile(
                            x,
                            y,
                            (m_renderTerrain->occupancy( x, y ) & isomap::common::occupancy::bitObstructed) == 0 );
                }
            }
            break;

        case DeleteUnit:
            if ( x >= 0 && x < m_renderTerrain->width() && y >= 0 && y < m_renderTerrain->height() ) {
                auto* unit = m_renderTerrain->getUnitAt( x, y );
                if ( unit != nullptr ) {
                    highlightTile( x, y, unit->player()->id() == m_controllingPlayer->id() );
                } else {
                    highlightTile( x, y, false );
                }
            }
            break;

        case SelectUnit:
            if ( x >= 0 && x < m_renderTerrain->width() && y >= 0 && y < m_renderTerrain->height() ) {
                auto* unit = m_renderTerrain->getUnitAt( x, y );
                if ( unit != nullptr ) {
                    highlightTile( x, y, unit->player()->id() == m_controllingPlayer->id() );
                } else {
                    highlightTile( x, y, false );
                }
            }
            break;

        case MoveUnit:
            if ( x >= 0 && x < m_renderTerrain->width() && y >= 0 && y < m_renderTerrain->height() ) {
                auto* unitAtCursor = m_renderTerrain->getUnitAt( x, y );
                auto* structureAtCursor = m_renderTerrain->getStructureAt( x, y );
                auto* selectedUnit = m_controllingPlayer->getUnit( m_selectedUnit );
                if ( unitAtCursor != nullptr ) {
                    highlightTile( x, y, unitAtCursor->player()->id() == m_controllingPlayer->id() );
                } else if ( selectedUnit != nullptr ) {
                    if ( structureAtCursor != nullptr ) {
                        highlightStructure( structureAtCursor, structureAtCursor->player() == m_controllingPlayer );
                    } else {
                        renderPathMap( x, y );
                    }
                } else {
                    m_mode = SelectUnit;
                }
            }
            break;
    }

}


void MainWindow::renderPathMap( int x, int y ) {
    if ( x >= 0 && x < m_renderTerrain->width() && y >= 0 && y < m_renderTerrain->height() ) {
        uint8_t pathBits = m_renderTerrain->pathMap()[y * m_renderTerrain->width() + x];
        if ( y < m_renderTerrain->height() - 1 ) {
            highlightTile( x, y + 1, (pathBits & isomap::common::path::bitUp) != 0 );
            if ( x > 0 ) {
                highlightTile( x - 1, y + 1, (pathBits & isomap::common::path::bitUpLeft) != 0 );
            }
            if ( x < m_renderTerrain->width() - 1 ) {
                highlightTile( x + 1, y + 1, (pathBits & isomap::common::path::bitUpRight) != 0 );
            }
        }
        if ( y > 0 ) {
            highlightTile( x, y - 1, (pathBits & isomap::common::path::bitDown) != 0 );
            if ( x > 0 ) {
                highlightTile( x - 1, y - 1, (pathBits & isomap::common::path::bitDownLeft) );
            }
            if ( x < m_renderTerrain->width() - 1 ) {
                highlightTile( x + 1, y - 1, (pathBits & isomap::common::path::bitDownRight) != 0 );
            }
        }
        if ( x > 0 ) {
            highlightTile( x - 1, y, (pathBits & isomap::common::path::bitLeft) != 0 );
        }
        if ( x < m_renderTerrain->width() - 1 ) {
            highlightTile( x + 1, y, (pathBits & isomap::common::path::bitRight) != 0 );
        }
    }

}

void MainWindow::renderStructurePlacement( int x, int y ) {
    auto* structureType = isomap::common::StructureType::get( m_structureType );
    highlightFootPrint( x, y,
                        structureType->footPrint( m_structureOrientation ),
                        m_controllingPlayer->canPlace( x, y, structureType, m_structureOrientation ) );
}


void MainWindow::highlightTile( int x, int y, bool green ) {
    m_renderTerrain->renderer()->addHighlight( isomap::common::Area( x, y, 1, 1 ), green ? vl::green : vl::red );
}

void MainWindow::highlightFootPrint( int x, int y, isomap::common::FootPrint* footPrint, bool green ) {
    m_renderTerrain->renderer()->highLight( isomap::common::Area( x, y, footPrint ), green ? vl::green : vl::red );
}

void MainWindow::highlightStructure( isomap::client::Structure* structure, bool green ) {
    highlightFootPrint( structure->x(), structure->y(), structure->footPrint(), green );
}

void MainWindow::place( int x, int y ) {
    m_cursorX = x;
    m_cursorY = y;
    switch ( m_mode ) {
        case PlaceStructure: {
            auto* structureType = isomap::common::StructureType::get( m_structureType );
            if ( m_controllingPlayer->canPlace( x, y, structureType, m_structureOrientation ) ) {
                m_controllingPlayer->buildStructure( x, y, structureType, m_structureOrientation );
            }
            break;
        }

        case DeleteStructure:
            // FIXME: shouldn't be using server structure here but client...
            if ( x >= 0 && x < m_serverMatch->terrain()->width() && y >= 0 && y < m_serverMatch->terrain()->height() ) {
                auto* structure = m_serverMatch->terrain()->getStructureAt( x, y );
                if ( structure != nullptr && structure->player()->id() == m_controllingPlayer->id() ) {
                    structure->destroy();
                }
            }
            break;

        case PlaceUnit: {
            if ( x >= 0 && x < m_renderTerrain->width() && y >= 0 && y < m_renderTerrain->height() &&
                 m_renderTerrain->isVisible( x, y ) &&
                 (m_renderTerrain->occupancy( x, y ) & isomap::common::occupancy::bitObstructed) == 0 ) {
                m_controllingPlayer->buildUnit( x, y, isomap::common::UnitType::get( 1 ), m_structureOrientation );
            }
            break;
        }

        case DeleteUnit:
            // FIXME: shouldn't be using server unit here but client...
            if ( x >= 0 && x < m_serverMatch->terrain()->width() && y >= 0 && y < m_serverMatch->terrain()->height() ) {
                auto* unit = m_serverMatch->terrain()->getUnitAt( x, y );
                if ( unit != nullptr && unit->player()->id() == m_controllingPlayer->id() ) {
                    unit->destroy();
                }
            }
            break;

        case SelectUnit:
            if ( x >= 0 && x < m_renderTerrain->width() && y >= 0 && y < m_renderTerrain->height() ) {
                auto* unit = m_renderTerrain->getUnitAt( x, y );
                if ( unit != nullptr && unit->player()->id() == m_controllingPlayer->id() ) {
                    m_selectedUnit = unit->id();
                    m_mode = MoveUnit;
                }
            }
            break;

        case MoveUnit:
            if ( x >= 0 && x < m_renderTerrain->width() && y >= 0 && y < m_renderTerrain->height() ) {
                auto* unitAtCursor = m_renderTerrain->getUnitAt( x, y );
                auto* structureAtCursor = m_renderTerrain->getStructureAt( x, y );
                auto* selectedUnit = m_controllingPlayer->getUnit( m_selectedUnit );
                if ( unitAtCursor != nullptr ) {
                    if ( unitAtCursor->player()->id() == m_controllingPlayer->id() ) {
                        m_selectedUnit = unitAtCursor->id();
                    }
                } else {
                    if ( selectedUnit != nullptr ) {
                        if ( structureAtCursor != nullptr ) {
                            if ( structureAtCursor->player() == m_controllingPlayer ) {
                                selectedUnit->construct( structureAtCursor );
                            }
                        } else {
                            selectedUnit->moveTo( x, y );
                        }
                    }
                }
            }

            break;
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
}

void MainWindow::updateText() {
    if ( m_controllingPlayer == nullptr ) {
        return;
    }

    // FIXME: Remove duplicated logic!
    switch ( m_mode ) {
        case PlaceStructure:
            m_text->setText( vl::Say( "FPS %n\n"
                                      "Mode: %s\n"
                                      "Player: %s\n"
                                      "Structure Type Id %n\n"
                                      "Structure Type Name %s\n"
                                      "Orientation %n\n"
                                      "X %n\n"
                                      "Y %n\n"
                                      "Can Place %s" )
                                     << fps()
                                     << getModeName()
                                     << m_controllingPlayer->name()
                                     << m_structureType
                                     << isomap::common::StructureType::get( m_structureType )->name()
                                     << m_structureOrientation
                                     << m_cursorX
                                     << m_cursorY
                                     << (m_controllingPlayer->canPlace(
                                             m_cursorX,
                                             m_cursorY,
                                             isomap::common::StructureType::get( m_structureType ),
                                             m_structureOrientation ) ? "yes" : "no") );
            break;

        case DeleteStructure: {
            isomap::client::Structure* structure = nullptr;
            bool canDelete = false;
            if ( m_cursorX >= 0 && m_cursorX < m_renderTerrain->width() && m_cursorY >= 0 &&
                 m_cursorY < m_renderTerrain->height() ) {
                structure = m_renderTerrain->getStructureAt( m_cursorX, m_cursorY );
                if ( structure != nullptr ) {
                    canDelete = structure->player()->id() == m_controllingPlayer->id();
                }
            }
            if ( structure ) {
                m_text->setText( vl::Say( "FPS %n\n"
                                          "Mode: %s\n"
                                          "Player: %s\n"
                                          "Structure Id: %n\n"
                                          "Structure Owner: %s\n"
                                          "Structure Type Id: %n\n"
                                          "Structure Type Name: %s\n"
                                          "Orientation: %n\n"
                                          "X: %n\n"
                                          "Y: %n\n"
                                          "Can Delete: %s" )
                                         << fps()
                                         << getModeName()
                                         << m_controllingPlayer->name()
                                         << structure->id()
                                         << structure->player()->name()
                                         << structure->type()->id()
                                         << structure->type()->name()
                                         << structure->orientation()
                                         << structure->x()
                                         << structure->y()
                                         << (canDelete ? "yes" : "no") );
            } else {
                m_text->setText( vl::Say( "FPS %n\n"
                                          "Mode: %s\n"
                                          "Player: %s\n"
                                          "X: %n\n"
                                          "Y: %n" )
                                         << fps()
                                         << getModeName()
                                         << m_controllingPlayer->name()
                                         << m_cursorX
                                         << m_cursorY );
            }

            break;
        }

        case PlaceUnit:
            if ( m_cursorX >= 0 && m_cursorX < m_serverMatch->terrain()->width() && m_cursorY >= 0 &&
                 m_cursorY < m_serverMatch->terrain()->height() ) {
                if ( m_renderTerrain->isVisible( m_cursorX, m_cursorY ) ) {
                    if ( m_renderTerrain->occupancy( m_cursorX, m_cursorY ) &
                         isomap::common::occupancy::bitObstructed ) {
                        m_text->setText( vl::Say( "FPS %n\n"
                                                  "Mode: %s\n"
                                                  "Player: %s\n"
                                                  "Unit Type Id %n\n"
                                                  "Unit Type Name %s\n"
                                                  "Orientation %n\n"
                                                  "X %n\n"
                                                  "Y %n\n"
                                                  "Cannot place, obstructed" )
                                                 << fps()
                                                 << getModeName()
                                                 << m_controllingPlayer->name()
                                                 << m_unitType
                                                 << isomap::common::UnitType::get( m_unitType )->name()
                                                 << m_structureOrientation
                                                 << m_cursorX
                                                 << m_cursorY );
                    } else {
                        m_text->setText( vl::Say( "FPS %n\n"
                                                  "Mode: %s\n"
                                                  "Player: %s\n"
                                                  "Unit Type Id %n\n"
                                                  "Unit Type Name %s\n"
                                                  "Orientation %n\n"
                                                  "X %n\n"
                                                  "Y %n\n"
                                                  "Can place" )
                                                 << fps()
                                                 << getModeName()
                                                 << m_controllingPlayer->name()
                                                 << m_unitType
                                                 << isomap::common::UnitType::get( m_unitType )->name()
                                                 << m_structureOrientation
                                                 << m_cursorX
                                                 << m_cursorY );
                    }
                } else {
                    m_text->setText( vl::Say( "FPS %n\n"
                                              "Mode: %s\n"
                                              "Player: %s\n"
                                              "Unit Type Id %n\n"
                                              "Unit Type Name %s\n"
                                              "Orientation %n\n"
                                              "X %n\n"
                                              "Y %n\n"
                                              "Cannot place, fogged" )
                                             << fps()
                                             << getModeName()
                                             << m_controllingPlayer->name()
                                             << m_unitType
                                             << isomap::common::UnitType::get( m_unitType )->name()
                                             << m_structureOrientation
                                             << m_cursorX
                                             << m_cursorY );
                }
            } else {
                m_text->setText( vl::Say( "FPS %n\n"
                                          "Mode: %s\n"
                                          "Player: %s\n"
                                          "Unit Type Id %n\n"
                                          "Unit Type Name %s\n"
                                          "Orientation %n\n"
                                          "X %n\n"
                                          "Y %n\n"
                                          "Cannot place, out of bounds" )
                                         << fps()
                                         << getModeName()
                                         << m_controllingPlayer->name()
                                         << m_unitType
                                         << isomap::common::UnitType::get( m_unitType )->name()
                                         << m_structureOrientation
                                         << m_cursorX
                                         << m_cursorY );
            }
            break;

        case DeleteUnit: {
            isomap::client::Unit* unit = nullptr;
            bool canDelete = false;
            if ( m_cursorX >= 0 && m_cursorX < m_renderTerrain->width() && m_cursorY >= 0 &&
                 m_cursorY < m_renderTerrain->height() ) {
                unit = m_renderTerrain->getUnitAt( m_cursorX, m_cursorY );
                if ( unit != nullptr ) {
                    canDelete = unit->player()->id() == m_controllingPlayer->id();
                }
            }
            if ( unit != nullptr ) {
                m_text->setText( vl::Say( "FPS %n\n"
                                          "Mode: %s\n"
                                          "Player: %s\n"
                                          "Unit Id: %n\n"
                                          "Unit Owner: %s\n"
                                          "Unit Type Id: %n\n"
                                          "Unit Type Name: %s\n"
                                          "Orientation: %n\n"
                                          "X: %n.%n\n"
                                          "Y: %n.%n\n"
                                          "Z: %n.%n\n"
                                          "Can Delete: %s" )
                                         << fps()
                                         << getModeName()
                                         << m_controllingPlayer->name()
                                         << unit->id()
                                         << unit->player()->name()
                                         << unit->type()->id()
                                         << unit->type()->name()
                                         << unit->orientation()
                                         << unit->tileX() << unit->subTileX()
                                         << unit->tileY() << unit->subTileY()
                                         << unit->tileZ() << unit->subTileZ()
                                         << (canDelete ? "yes" : "no") );
            } else {
                m_text->setText( vl::Say( "FPS %n\n"
                                          "Mode: %s\n"
                                          "Player: %s\n"
                                          "X: %n\n"
                                          "Y: %n" )
                                         << fps()
                                         << getModeName()
                                         << m_controllingPlayer->name()
                                         << m_cursorX
                                         << m_cursorY );
            }
            break;
        }

        case SelectUnit: {
            isomap::client::Unit* unit = nullptr;
            bool canSelect = false;
            if ( m_cursorX >= 0 && m_cursorX < m_renderTerrain->width() && m_cursorY >= 0 &&
                 m_cursorY < m_renderTerrain->height() ) {
                unit = m_renderTerrain->getUnitAt( m_cursorX, m_cursorY );
                if ( unit != nullptr ) {
                    canSelect = unit->player()->id() == m_controllingPlayer->id();
                }
            }
            if ( unit != nullptr ) {
                m_text->setText( vl::Say( "FPS %n\n"
                                          "Mode: %s\n"
                                          "Player: %s\n"
                                          "Unit Id: %n\n"
                                          "Unit Owner: %s\n"
                                          "Unit Type Id: %n\n"
                                          "Unit Type Name: %s\n"
                                          "Orientation: %n\n"
                                          "X: %n.%n\n"
                                          "Y: %n.%n\n"
                                          "Z: %n.%n\n"
                                          "Can Select: %s" )
                                         << fps()
                                         << getModeName()
                                         << m_controllingPlayer->name()
                                         << unit->id()
                                         << unit->player()->name()
                                         << unit->type()->id()
                                         << unit->type()->name()
                                         << unit->orientation()
                                         << unit->tileX() << unit->subTileX()
                                         << unit->tileY() << unit->subTileY()
                                         << unit->tileZ() << unit->subTileZ()
                                         << (canSelect ? "yes" : "no") );
            } else {
                m_text->setText( vl::Say( "FPS %n\n"
                                          "Mode: %s\n"
                                          "Player: %s\n"
                                          "X: %n\n"
                                          "Y: %n" )
                                         << fps()
                                         << getModeName()
                                         << m_controllingPlayer->name()
                                         << m_cursorX
                                         << m_cursorY );
            }
            break;
        }

        case MoveUnit:
            if ( m_controllingPlayer->getUnit( m_selectedUnit ) != nullptr ) {
                auto* unit = m_controllingPlayer->getUnit( m_selectedUnit );
                m_text->setText( vl::Say( "FPS %n\n"
                                          "Mode: %s\n"
                                          "Player: %s\n"
                                          "Unit Id: %n\n"
                                          "Unit Owner: %s\n"
                                          "Unit Type Id: %n\n"
                                          "Unit Type Name: %s\n"
                                          "Orientation: %n\n"
                                          "X: %n.%n\n"
                                          "Y: %n.%n\n"
                                          "Z: %n.%n\n"
                                          "State: %s" )
                                         << fps()
                                         << getModeName()
                                         << m_controllingPlayer->name()
                                         << unit->id()
                                         << unit->player()->name()
                                         << unit->type()->id()
                                         << unit->type()->name()
                                         << unit->orientation()
                                         << unit->tileX() << unit->subTileX()
                                         << unit->tileY() << unit->subTileY()
                                         << unit->tileZ() << unit->subTileZ()
                                         << unit->stateName() );
            } else {
                m_text->setText( vl::Say( "FPS %n\n"
                                          "Mode: %s\n"
                                          "Player: %s\n"
                                          "X: %n\n"
                                          "Y: %n" )
                                         << fps()
                                         << getModeName()
                                         << m_controllingPlayer->name()
                                         << m_cursorX
                                         << m_cursorY );
            }
            break;
    }
/*
    m_text->setText( vl::Say( "FPS %n\n"
                              "Mode: %s\n"
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
                              "Ore density %n" ) << fps() << getModeName() << m_generator.seed()
                                                 << m_generator.shoreBits()
                                                 << m_generator.terrainHeight() << m_generator.waterDepth()
                                                 << (1u << m_generator.heightScale())
                                                 << m_generator.heightNoise()
                                                 << m_generator.minHeight() << m_generator.maxHeight()
                                                 << (1u << m_generator.cliffScale()) << m_generator.cliffNoise()
                                                 << m_generator.cliffThreshold()
                                                 << (1u << m_generator.oreScale()) << m_generator.oreNoise()
                                                 << m_generator.oreThreshold() << m_generator.oreDensity() );*/
}

void MainWindow::sendMessages() {
    //printf( "Sending messages to server\n" );
    // send all messages to the server
    for ( auto* match : {m_clientAIMatch, m_clientPlayerMatch} ) {
        if ( match == nullptr ) {
            continue;
        }
        //printf( "\tfor %08X\n", match->id() );
        for ( auto* msg : match->clientMessages() ) {
            //printf( "\t\t type %s\n", msg->typeName() );
            m_serverMatch->processMessage( msg );
            delete msg;
        }
    }
    //printf( "Sending messages completed\n" );
}

void MainWindow::receiveMessages() {
    //printf( "Receiving messages from server\n" );
    // receive all messages from the server and distribute among players
    for ( auto* match : {m_clientAIMatch, m_clientPlayerMatch} ) {
        if ( match == nullptr ) {
            continue;
        }
        //printf( "\tfor %08X\n", match->id() );
        for ( auto* msg : m_serverMatch->serverMessages( match->id() ) ) {
            //printf( "\t\t type %s\n", msg->typeName() );
            match->processMessage( msg );
            delete msg;
        }
    }
    //printf( "Receiving messages completed\n" );
}

void MainWindow::sendAndReceiveMessages() {
    //printf( "Send and receive messages\n" );
    sendMessages();
    receiveMessages();
}

const char* MainWindow::getModeName() const {
    switch ( m_mode ) {
        case PlaceStructure:
            return "Place Structure (B)";
        case DeleteStructure:
            return "Delete Structure (V)";
        case PlaceUnit:
            return "Place Unit (G)";
        case DeleteUnit:
            return "Delete Unit (F)";
        case SelectUnit:
            return "Select Unit (N)";
        case MoveUnit:
            return "Move Unit (M)";
    }
    return "Unknown";
}
