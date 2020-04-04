#ifndef TESTPROJECT_MAINWINDOW_H
#define TESTPROJECT_MAINWINDOW_H

#include <vlGraphics/Applet.hpp>
#include "game_map.h"
#include "game_unit.h"



class MainWindow : public vl::Applet {

public:
    // called once after the OpenGL window has been opened
    void initEvent() override;

    void keyPressEvent(unsigned short, vl::EKey) override;


    void resizeEvent(int w, int h) override;

    void updateScene();


    void zoomIn();

    void zoomOut();

    void rotateLeft();

    void rotateRight();

    void left();

    void right();

    void up();

    void down();

    void screenToWorld(int screen_x, int screen_y, int& world_x, int& world_y);

    void highlight( int x, int y );

    void focusTileAt(int tile_x, int tile_y, int screen_x, int screen_y);

protected:
    void updateProjection();
    void updateCamera();

    game_map* m_world;
    game_unit* m_unit;
    int m_zoom;
    unsigned int m_orientation;
    int m_x;
    int m_y;
    vl::real m_x_off = 0.5;
    vl::real m_y_off = 0.5;

    int m_angle = 0;
    unsigned char m_smooth = 128;
    int m_width = 128;
    int m_height = 128;

};


#endif //TESTPROJECT_MAINWINDOW_H
