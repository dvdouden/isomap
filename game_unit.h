
#ifndef TESTPROJECT_GAME_UNIT_H
#define TESTPROJECT_GAME_UNIT_H


#include <vlGraphics/Effect.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/RenderingAbstract.hpp>

// forward declarations
class game_map;

class game_unit {
public:
    game_unit( vl::RenderingAbstract* rendering, game_map* world );

    void setPosition( int x, int y, int z );
    void moveTo( int x, int y );
    void update();

private:
    int m_x = 0;
    int m_y = 0;
    int m_z = 0;

    int m_targetX = 0;
    int m_targetY = 0;

    vl::ref<vl::Geometry> m_geom;
    vl::ref<vl::Transform> m_transform;
    vl::ref<vl::Effect> m_effect;

    game_map* m_world;
};


#endif //TESTPROJECT_GAME_UNIT_H
