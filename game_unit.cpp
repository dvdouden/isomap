#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include "game_map.h"
#include "game_unit.h"

game_unit::game_unit( vl::RenderingAbstract* rendering, game_map* world ) :
m_world( world )
{
    m_transform = new vl::Transform;
    rendering->as<vl::Rendering>()->transform()->addChild( m_transform.get() );

    m_geom = vl::makeBox( vl::vec3( 0.5, 0.5, 0.5), 1, 1, 1 );
    m_geom->computeNormals();

    m_effect = new vl::Effect;
    m_effect->shader()->gocMaterial()->setColorMaterialEnabled(true);
    m_effect->shader()->gocMaterial()->setDiffuse( vl::crimson );
    m_effect->shader()->enable(vl::EN_DEPTH_TEST);
    m_effect->shader()->enable(vl::EN_LIGHTING);
    m_effect->lod(0)->push_back( new vl::Shader );
    m_effect->shader(0,1)->enable(vl::EN_BLEND);
    //m_effect->shader(0,1)->enable(vl::EN_LINE_SMOOTH);
    m_effect->shader(0,1)->enable(vl::EN_DEPTH_TEST);
    m_effect->shader(0,1)->enable(vl::EN_POLYGON_OFFSET_LINE);
    m_effect->shader(0,1)->gocPolygonOffset()->set(-1.0f, -1.0f);
    m_effect->shader(0,1)->gocPolygonMode()->set(vl::PM_LINE, vl::PM_LINE);
    m_effect->shader(0,1)->gocColor()->setValue(vl::lightgreen);
    m_effect->shader(0,1)->setRenderState( m_effect->shader()->getMaterial() );
    m_effect->shader(0,1)->setRenderState( m_effect->shader()->getLight(0), 0 );


    vl::ref<vl::SceneManagerActorTree> scene_manager = new vl::SceneManagerActorTree;
    scene_manager->setCullingEnabled( false );
    rendering->as<vl::Rendering>()->sceneManagers()->push_back(scene_manager.get());
    scene_manager->tree()->addActor( m_geom.get(), m_effect.get(), m_transform.get()  );
}

void game_unit::setPosition(int x, int y, int z) {
    m_x = x;
    m_y = y;
    m_z = z;
}

void game_unit::update() {
    if ( m_x < m_targetX ) {
        m_x++;
    } else if ( m_x > m_targetX ) {
        m_x--;
    }
    if ( m_y < m_targetY ) {
        m_y++;
    } else if ( m_y > m_targetY ) {
        m_y--;
    }

    if ( m_world->isInside( m_x, m_y ) ) {
        m_z = m_world->height(m_x, m_y);
    }

    vl::mat4 matrix = vl::mat4::getTranslation( m_x, m_y, m_z * ::sqrt(2.0 / 3.0 ) / 2.0);
    m_transform->setLocalMatrix( matrix );
}

void game_unit::moveTo( int x, int y ) {
    m_targetX = x;
    m_targetY = y;
}