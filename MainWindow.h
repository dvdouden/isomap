#ifndef TESTPROJECT_MAINWINDOW_H
#define TESTPROJECT_MAINWINDOW_H

#include <vlGraphics/Applet.hpp>
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

    void left();

    void right();

    void up();

    void down();

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
    uint32_t m_mapGenScale = 5;
    unsigned char m_cliffAmount = 0;
    unsigned char m_cliffVariation = 128;
    unsigned char m_oreAmount = 32;
    unsigned char m_oreDensity = 128;
    unsigned char m_variation = 64;
    int m_width = 256;
    int m_height = 256;
    bool m_paused = true;

    isomap::server::Match* m_match;
    isomap::server::Player* m_player;

    isomap::client::Terrain* m_terrain;
    isomap::server::Unit* m_serverUnit;
    isomap::client::Unit* m_clientUnit;
};


#endif //TESTPROJECT_MAINWINDOW_H
