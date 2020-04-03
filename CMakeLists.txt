cmake_minimum_required(VERSION 3.15)
project(TestProject)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")
find_package(OpenGL REQUIRED)
if ( MINGW )

else()
    find_package(GLUT REQUIRED)
endif()
find_package(VL COMPONENTS VLMain VLX VLGraphics VLCore VLVG VLVolume VLMolecule VLWin32 FreeType JPG PNG TIFF ZLib REQUIRED)
add_definitions(-DVL_STATIC_LINKING)


# add VL include directories
include_directories(${VL_INCLUDE_DIRS})
if ( MINGW )

else()
    include_directories(${GLUT_INCLUDE_DIR})
endif()

add_executable(TestProject win32_main.cpp game_map.cpp game_map.h util/math.h util/math.cpp)

if ( MINGW )
    message( "hello: ${VL_LIBRARIES}" )
    target_link_libraries(TestProject ${VL_LIBRARIES} ${OPENGL_LIBRARIES} -static )
else()
    target_link_libraries(TestProject ${VL_LIBRARIES} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} -static )
endif()