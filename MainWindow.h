#ifndef TESTPROJECT_MAINWINDOW_H
#define TESTPROJECT_MAINWINDOW_H

#include <vlGraphics/Applet.hpp>
#include <vlGraphics/Text.hpp>
#include "types.h"
#include "common/types.h"


class MainWindow : public vl::Applet {

public:
    // called once after the OpenGL window has been opened
    void initEvent() override;

    void keyPressEvent( unsigned short, vl::EKey ) override;


    void resizeEvent( int w, int h ) override;

    void updateScene();


    void zoomIn();

    void zoomOut();

    void rotateLeft();

    void rotateRight();

    void screenToWorld( int screen_x, int screen_y, int& world_x, int& world_y );

    void highlight( int x, int y );

    void focusTileAt( int tile_x, int tile_y, int screen_x, int screen_y );

protected:
    void updateProjection();

    void updateCamera();

    void regenerateMap();

    vl::real m_zoom;
    int m_zoomLevel;
    unsigned int m_orientation;
    int m_x;
    int m_y;
    vl::real m_x_off = 0.5;
    vl::real m_y_off = 0.5;

    int m_angle = 0;

    uint8_t m_shoreBits = 1;

    uint32_t m_heightScale = 5;
    uint8_t m_heightNoise = 64;
    uint8_t m_minHeight = 0;
    uint8_t m_maxHeight = 255;

    uint32_t m_cliffScale = 5;
    uint8_t m_cliffNoise = 128;
    uint8_t m_cliffThreshold = 0;

    uint32_t m_oreScale = 5;
    uint8_t m_oreNoise = 128;
    uint8_t m_oreThreshold = 32;
    uint8_t m_oreDensity = 128;

    int m_width = 256;
    int m_height = 256;
    bool m_paused = true;

    isomap::server::Match* m_match = nullptr;
    isomap::server::Player* m_player = nullptr;

    isomap::client::Terrain* m_terrain = nullptr;
    isomap::server::Unit* m_serverUnit = nullptr;
    isomap::client::Unit* m_clientUnit = nullptr;

    vl::ref<vl::Text> m_text;

    void updateText();
};


#endif //TESTPROJECT_MAINWINDOW_H
