cmake_minimum_required(VERSION 3.14.2)
project(IsoMap)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")
find_package(OpenGL REQUIRED)

#set( USE_GLUT )
set( USE_SDL ON )


if ( MINGW )

elseif( USE_GLUT )
    find_package(GLUT REQUIRED)
elseif( USE_SDL )
    find_package(SDL REQUIRED)
endif()

set( VL_BASE_COMPONENTS VLMain VLX VLGraphics VLCore VLVG VLVolume VLMolecule FreeType JPG PNG TIFF ZLib )


if ( MINGW )
    find_package(VL COMPONENTS ${VL_BASE_COMPONENTS} VLWin32 REQUIRED)
elseif( USE_GLUT )
    find_package(VL COMPONENTS ${VL_BASE_COMPONENTS} VLGLUT REQUIRED)
elseif( USE_SDL )
    find_package(VL COMPONENTS ${VL_BASE_COMPONENTS} VLSDL REQUIRED)
endif()
add_definitions(-DVL_STATIC_LINKING)


# add VL include directories
include_directories(${VL_INCLUDE_DIRS})
if ( MINGW )
    set( PLATFORM_ENTRY win32_main.cpp )
elseif( USE_GLUT )
    include_directories(${GLUT_INCLUDE_DIR})
    set( PLATFORM_ENTRY glut_main.cpp )
elseif( USE_SDL )
    include_directories(${SDL_INCLUDE_DIR})
    set( PLATFORM_ENTRY sdl_main.cpp )
endif()

add_executable(IsoMap ${PLATFORM_ENTRY} game_map.cpp game_map.h util/math.h util/math.cpp MouseListener.cpp MouseListener.h MainWindow.cpp MainWindow.h game_unit.cpp game_unit.h types.h server/Unit.cpp server/Unit.h server/UnitType.cpp server/UnitType.h server/Structure.cpp server/Structure.h server/StructureType.cpp server/StructureType.h server/Object.cpp server/Object.h server/World.cpp server/World.h server/Match.cpp server/Match.h server/types.h server/Player.cpp server/Player.h)

if ( MINGW )
    target_link_libraries(IsoMap ${VL_LIBRARIES} ${OPENGL_LIBRARIES} -static )
elseif( USE_GLUT )
    target_link_libraries(IsoMap ${VL_LIBRARIES} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} )
elseif( USE_SDL )
    target_link_libraries(IsoMap ${VL_LIBRARIES} ${OPENGL_LIBRARIES} ${SDL_LIBRARY} )
endif()