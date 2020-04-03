#ifndef TESTPROJECT_MOUSELISTENER_H
#define TESTPROJECT_MOUSELISTENER_H

#include <vlGraphics/TrackballManipulator.hpp>
#include <vlGraphics/Camera.hpp>
#include "MainWindow.h"


class MouseListener : public vl::TrackballManipulator {
public:
    MouseListener(MainWindow* pWindow) :
    vl::TrackballManipulator(),
    m_window( pWindow ){

    }

    void mouseDownEvent(vl::EMouseButton, int x, int y) override;

    void mouseUpEvent(vl::EMouseButton, int x, int y) override;

    void mouseMoveEvent(int x, int y) override;

    void enableEvent(bool enabled) override;

    void initEvent() override {}

    void destroyEvent() override {}

    void updateEvent() override {}

    void addedListenerEvent(vl::OpenGLContext*) override {}

    void removedListenerEvent(vl::OpenGLContext*) override {}

    void mouseWheelEvent(int) override;

    void keyPressEvent(unsigned short, vl::EKey) override {}

    void keyReleaseEvent(unsigned short, vl::EKey) override {}

    void resizeEvent(int, int) override {}

    void fileDroppedEvent(const std::vector<vl::String>&) override {}

    void visibilityEvent(bool) override {}

private:
    vl::ref<vl::Camera> m_camera;
    MainWindow* m_window;

    enum Mode {
        Mode_none,
        Mode_rotate,
        Mode_translate,
        Mode_highlight,
    };
    Mode m_mode = Mode_none;

    int m_x = 0;
    int m_y = 0;
    int m_tile_x = 0;
    int m_tile_y = 0;
};


#endif //TESTPROJECT_MOUSELISTENER_H
