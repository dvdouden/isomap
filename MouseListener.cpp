#include "MouseListener.h"

void MouseListener::mouseDownEvent(vl::EMouseButton btn, int x, int y) {
    if ( btn == vl::MiddleButton ) {
        m_mode = Mode_rotate;
        m_x = x;
        m_y = y;
    } else if ( btn == vl::RightButton ) {
        m_mode = Mode_translate;
        m_x = x;
        m_y = y;
        m_window->screenToWorld(x, y, m_tile_x, m_tile_y);
    } else if ( btn == vl::LeftButton ) {
        int wx = 0;
        int wy = 0;
        m_window->screenToWorld( x, y, wx, wy );
        m_window->highlight( wx, wy );
        m_mode = Mode_highlight;
    }
}

void MouseListener::mouseUpEvent(vl::EMouseButton btn, int x, int y) {
    if ( btn == vl::MiddleButton && m_mode == Mode_rotate ) {
        m_mode = Mode_none;
    }
    else if ( btn == vl::RightButton && m_mode == Mode_translate ) {
        m_mode = Mode_none;
    }
    else if ( btn == vl::LeftButton && m_mode == Mode_highlight ) {
        m_mode = Mode_none;
    }
}

void MouseListener::mouseMoveEvent(int x, int y) {
    switch ( m_mode ) {
        case Mode_rotate:
            if ( x - m_x > 10 ) {
                m_window->rotateRight();
                m_mode = Mode_none;
            } else if ( m_x - x > 10 ) {
                m_window->rotateLeft();
                m_mode = Mode_none;
            }
            break;
        case Mode_translate:
            m_window->focusTileAt( m_tile_x, m_tile_y, x, y );
            break;
        case Mode_highlight: {
            int wx = 0;
            int wy = 0;
            m_window->screenToWorld(x, y, wx, wy);
            m_window->highlight(wx, wy);
            break;
        }
        default:
            break;
    }
}

void MouseListener::enableEvent(bool enabled) {

}

void MouseListener::mouseWheelEvent(int rotation)  {
    if ( rotation > 0 ) {
        m_window->zoomIn();
    } else {
        m_window->zoomOut();
    }
}