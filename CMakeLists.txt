cmake_minimum_required(VERSION 3.14.2)
project(IsoMap)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")
find_package(OpenGL REQUIRED)

#set( USE_GLUT )
set(USE_SDL ON)


if (MINGW)

elseif (USE_GLUT)
    find_package(GLUT REQUIRED)
elseif (USE_SDL)
    find_package(SDL REQUIRED)
endif ()

set(VL_BASE_COMPONENTS VLMain VLX VLGraphics VLCore VLVG VLVolume VLMolecule FreeType JPG PNG TIFF ZLib)


if (MINGW)
    find_package(VL COMPONENTS ${VL_BASE_COMPONENTS} VLWin32 REQUIRED)
elseif (USE_GLUT)
    find_package(VL COMPONENTS ${VL_BASE_COMPONENTS} VLGLUT REQUIRED)
elseif (USE_SDL)
    find_package(VL COMPONENTS ${VL_BASE_COMPONENTS} VLSDL REQUIRED)
endif ()
add_definitions(-DVL_STATIC_LINKING)


# add VL include directories
include_directories(${VL_INCLUDE_DIRS})
if (MINGW)
    set(PLATFORM_ENTRY win32_main.cpp)
elseif (USE_GLUT)
    include_directories(${GLUT_INCLUDE_DIR})
    set(PLATFORM_ENTRY glut_main.cpp)
elseif (USE_SDL)
    include_directories(${SDL_INCLUDE_DIR})
    set(PLATFORM_ENTRY sdl_main.cpp)
endif ()

add_executable(
        IsoMap
        ${PLATFORM_ENTRY}
        MainWindow.cpp
        MainWindow.h
        MouseListener.cpp
        MouseListener.h
        client/Match.cpp
        client/Match.h
        client/ModelCache.cpp
        client/ModelCache.h
        client/Player.cpp
        client/Player.h
        client/Structure.cpp
        client/Structure.h
        client/Terrain.cpp
        client/Terrain.h
        client/Unit.cpp
        client/Unit.h
        client/match/Renderer.cpp
        client/match/Renderer.h
        client/player/AutonomousUnitsAI.cpp
        client/player/AutonomousUnitsAI.h
        client/player/Controller.h
        client/player/ConstructionController.cpp
        client/player/ConstructionController.h
        client/player/Renderer.cpp
        client/player/Renderer.h
        client/structure/Controller.cpp
        client/structure/Controller.h
        client/structure/Renderer.cpp
        client/structure/Renderer.h
        client/terrain/Renderer.cpp
        client/terrain/Renderer.h
        client/unit/ConstructorController.cpp
        client/unit/ConstructorController.h
        client/unit/Controller.h
        client/unit/Controller.cpp
        client/unit/Renderer.cpp
        client/unit/Renderer.h
        common/types.h
        common/FootPrint.cpp
        common/FootPrint.h
        common/MatchMessage.cpp
        common/MatchMessage.h
        common/PlayerMessage.cpp
        common/PlayerMessage.h
        common/StructureData.h
        common/StructureMessage.cpp
        common/StructureMessage.h
        common/StructureType.cpp
        common/StructureType.h
        common/TerrainData.cpp
        common/TerrainData.h
        common/TerrainMessage.cpp
        common/TerrainMessage.h
        common/UnitData.h
        common/UnitData.cpp
        common/UnitMessage.cpp
        common/UnitMessage.h
        common/UnitType.cpp
        common/UnitType.h
        server/Match.cpp
        server/Match.h
        server/Object.cpp
        server/Object.h
        server/Player.cpp
        server/Player.h
        server/Structure.cpp
        server/Structure.h
        server/Terrain.cpp
        server/Terrain.h
        server/TerrainGenerator.cpp
        server/TerrainGenerator.h
        server/Unit.cpp
        server/Unit.h
        util/math.h
        util/math.cpp)

if (MINGW)
    target_link_libraries(IsoMap ${VL_LIBRARIES} ${OPENGL_LIBRARIES} -static)
elseif (USE_GLUT)
    target_link_libraries(IsoMap ${VL_LIBRARIES} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
elseif (USE_SDL)
    target_link_libraries(IsoMap ${VL_LIBRARIES} ${OPENGL_LIBRARIES} ${SDL_LIBRARY})
endif ()