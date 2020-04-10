#include <iostream>

#include <vlCore/VisualizationLibrary.hpp>
#include <vlGraphics/Applet.hpp>
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/Rendering.hpp>

#include <vlCore/Time.hpp>
#include <vlGraphics/Light.hpp>
#include <vlWin32/Win32Window.hpp>
#include "MouseListener.h"
#include "MainWindow.h"

void showWin32Console() {
    if ( AllocConsole() == 0 )
        return;
    FILE* f_new_stdout = nullptr;
    FILE* f_new_stderr = nullptr;
    FILE* f_new_stdin = nullptr;
    ::freopen_s( &f_new_stdout, "CONOUT$", "w", stdout );
    ::freopen_s( &f_new_stderr, "CONOUT$", "w", stderr );
    ::freopen_s( &f_new_stdin, "CONIN$", "r", stdin );
    std::cout.clear();
    std::cerr.clear();
    std::cin.clear();
    std::wcout.clear();
    std::wcerr.clear();
    std::wcin.clear();
}


int APIENTRY
WinMain( HINSTANCE /*hCurrentInst*/, HINSTANCE /*hPreviousInst*/, LPSTR /*lpszCmdLine*/, int /*nCmdShow*/ ) {
    /* open a console so we can see the applet's output on stdout */
    showWin32Console();

    /* init Visualization Library */
    vl::VisualizationLibrary::init();

    /* setup the OpenGL context format */
    vl::OpenGLContextFormat format;
    format.setDoubleBuffer( true );
    format.setRGBABits( 8, 8, 8, 0 );
    format.setDepthBufferBits( 24 );
    format.setStencilBufferBits( 8 );
    format.setFullscreen( false );
    format.setMultisampleSamples( 16 );
    format.setMultisample( false );

    /* create the applet to be run */
    MainWindow* w = new MainWindow;
    vl::ref<vl::Applet> applet = w;
    applet->initialize();
    applet->setTrackball( new MouseListener( w ) );
    applet->trackball()->setCamera( applet->rendering()->as<vl::Rendering>()->camera() );

    /* create a native Win32 window */
    vl::ref<vlWin32::Win32Window> win32_window = new vlWin32::Win32Window;

    /* bind the applet so it receives all the GUI events related to the OpenGLContext */
    win32_window->addEventListener( applet.get() );

    /* target the window so we can render on it */
    applet->rendering()->as<vl::Rendering>()->renderer()->setFramebuffer( win32_window->framebuffer() );

    /* black background */
    applet->rendering()->as<vl::Rendering>()->camera()->viewport()->setClearColor( vl::black );
    applet->rendering()->as<vl::Rendering>()->camera()->setProjectionOrtho();

    /* define the camera position and orientation */
    vl::real ratio = ::sqrt( 2.0 / 3.0 );
    vl::real distance = 100.0;
    vl::vec3 eye = vl::vec3( distance, -distance, distance * ratio ); // camera position
    vl::vec3 center = vl::vec3( 0, 0, 0 );   // point the camera is looking at
    vl::vec3 up = vl::vec3( 0, 0, 1 );   // up direction
    vl::mat4 view_mat = vl::mat4::getLookAt( eye, center, up );

    applet->rendering()->as<vl::Rendering>()->camera()->setViewMatrix( view_mat );

    /* Initialize the OpenGL context and window properties */
    int x = 0;
    int y = 0;
    int width = 800;
    int height = 600;
    win32_window->initWin32GLWindow( NULL, NULL, "Isometric grid rendering using Visualization Library", format, x, y,
                                     width, height );

    /* show the window */
    win32_window->show();

    /* run the Win32 message loop */
    int res = vlWin32::messageLoop();

    /* deallocate the window with all the OpenGL resources before shutting down Visualization Library */
    win32_window = NULL;

    /* shutdown Visualization Library */
    vl::VisualizationLibrary::shutdown();


    return res;
}