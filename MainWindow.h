#ifndef TESTPROJECT_MAINWINDOW_H
#define TESTPROJECT_MAINWINDOW_H

#include <vlGraphics/Applet.hpp>
#include <vlGraphics/Text.hpp>
#include "common/types.h"
#include "server/TerrainGenerator.h"


class MainWindow : public vl::Applet {

public:
    // called once after the OpenGL window has been opened
    void initEvent() override;

    void destroyEvent() override;

    void keyPressEvent( unsigned short, vl::EKey ) override;

    void keyReleaseEvent( unsigned short, vl::EKey ) override;

    void resizeEvent( int w, int h ) override;

    void updateScene() override;

    void zoomIn();

    void zoomOut();

    void rotateLeft();

    void rotateRight();

    void screenToWorld( int screen_x, int screen_y, int& world_x, int& world_y );

    void worldToScreen( int world_x, int world_y, int world_z, int& screen_x, int& screen_y );

    void worldToScreen( int world_x, int world_y, int world_z, int corner, int& screen_x, int& screen_y );

    void highlight( int x, int y );

    void place( int x, int y );

    void focusTileAt( int tile_x, int tile_y, int screen_x, int screen_y );

protected:

    enum Mode {
        PlaceStructure,
        DeleteStructure,
        PlaceUnit,
        DeleteUnit,
        SelectUnit,
        MoveUnit,
    };

    void updateProjection();

    void updateCamera();

    void regenerateMap();

    void renderPathMap( int x, int y );

    void highlightTile( int x, int y, bool green );

    void highlightFootPrint( int x, int y, isomap::common::FootPrint* footPrint, bool green );

    void highlightStructure( isomap::client::Structure* structure, bool green );

    void renderStructurePlacement( int x, int y );

    vl::real m_zoom;
    int m_zoomLevel;
    unsigned int m_orientation;
    int m_x;
    int m_y;
    vl::real m_x_off = 0.5;
    vl::real m_y_off = 0.5;

    int m_angle = 0;

    int m_width = 256;
    int m_height = 256;
    bool m_paused = true;
    bool m_renderColumn = false;

    isomap::server::Match* m_serverMatch = nullptr;

    isomap::client::Match* m_clientPlayerMatch = nullptr;
    isomap::client::Player* m_clientPlayer = nullptr;

    isomap::client::Match* m_clientAIMatch = nullptr;
    isomap::client::Player* m_clientAI = nullptr;

    isomap::client::Match* m_renderMatch = nullptr;
    isomap::client::Terrain* m_renderTerrain = nullptr;
    isomap::client::Player* m_controllingPlayer = nullptr;

    isomap::server::TerrainGenerator m_generator;

    vl::ref<vl::Text> m_text;

    Mode m_mode = PlaceStructure;

    isomap::id_t m_structureType = 1;
    isomap::id_t m_unitType = 1;
    int m_structureOrientation = 0;
    isomap::id_t m_selectedUnit = 0;
    int m_cursorX;
    int m_cursorY;

    void updateText();

    void sendMessages();

    void receiveMessages();

    void sendAndReceiveMessages();

    bool isBelow( int dx, int dy, int x0, int y0, int x1, int y1 );

    const char* getModeName() const;
};


#endif //TESTPROJECT_MAINWINDOW_H
