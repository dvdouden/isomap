#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include "game_map.h"
#include "game_unit.h"

namespace isomap {

    game_unit::game_unit(vl::RenderingAbstract *rendering, game_map *world) :
            m_world(world) {
        m_transform = new vl::Transform;
        rendering->as<vl::Rendering>()->transform()->addChild(m_transform.get());

        m_geom = vl::makeBox(vl::vec3(0, 0, 0), 1, 1, 1);
        m_geom->computeNormals();

        m_effect = new vl::Effect;
        m_effect->shader()->gocMaterial()->setColorMaterialEnabled(true);
        m_effect->shader()->gocMaterial()->setDiffuse(vl::crimson);
        m_effect->shader()->enable(vl::EN_DEPTH_TEST);
        m_effect->shader()->enable(vl::EN_LIGHTING);
        m_effect->lod(0)->push_back(new vl::Shader);
        m_effect->shader(0, 1)->enable(vl::EN_BLEND);
        //m_effect->shader(0,1)->enable(vl::EN_LINE_SMOOTH);
        m_effect->shader(0, 1)->enable(vl::EN_DEPTH_TEST);
        m_effect->shader(0, 1)->enable(vl::EN_POLYGON_OFFSET_LINE);
        m_effect->shader(0, 1)->gocPolygonOffset()->set(-1.0f, -1.0f);
        m_effect->shader(0, 1)->gocPolygonMode()->set(vl::PM_LINE, vl::PM_LINE);
        m_effect->shader(0, 1)->gocColor()->setValue(vl::lightgreen);
        m_effect->shader(0, 1)->setRenderState(m_effect->shader()->getMaterial());
        m_effect->shader(0, 1)->setRenderState(m_effect->shader()->getLight(0), 0);


        vl::ref<vl::SceneManagerActorTree> scene_manager = new vl::SceneManagerActorTree;
        scene_manager->setCullingEnabled(false);
        rendering->as<vl::Rendering>()->sceneManagers()->push_back(scene_manager.get());
        scene_manager->tree()->addActor(m_geom.get(), m_effect.get(), m_transform.get());
    }

    void game_unit::setPosition(int x, int y, int z) {
        m_x = x;
        m_y = y;
        m_z = z;
    }

    void game_unit::update() {
        // TODO: need to make corners and stuff
        if ( m_x < m_targetX ) {
            if ( m_y < m_targetY ) {
                m_targetOrientation = 45;
            } else if ( m_y > m_targetY ) {
                m_targetOrientation = 315;
            } else {
                m_targetOrientation = 0;
            }
        } else if ( m_x > m_targetX ) {
            if ( m_y < m_targetY ) {
                m_targetOrientation = 135;
            } else if ( m_y > m_targetY ) {
                m_targetOrientation = 225;
            } else {
                m_targetOrientation = 180;
            }
        } else {
            if ( m_y < m_targetY ) {
                m_targetOrientation = 90;
            } else if ( m_y > m_targetY ) {
                m_targetOrientation = 270;
            }
        }

        if ( m_orientation != m_targetOrientation ) {
            auto delta = m_targetOrientation - m_orientation;
            if ( delta < 0 ) {
                delta += 360;
            }
            if ( delta < 180 ) {
                m_orientation += 5;
                if ( m_orientation >= 360 ) {
                    m_orientation -= 360;
                }
            } else {
                m_orientation -= 5;
                if ( m_orientation < 0 ) {
                    m_orientation += 360;
                }
            }
        } else if ( m_x != m_targetX || m_y != m_targetY ) {
            // so this is silly; we're totally going to overshoot our target because of floating point errors.
            real speed = 0.125;
            switch ( (int)m_orientation ) {
                default:
                case   0: m_x += speed;               break;
                case  45: m_x += speed; m_y += speed; break;
                case  90:               m_y += speed; break;
                case 135: m_x -= speed; m_y += speed; break;
                case 180: m_x -= speed;               break;
                case 225: m_x -= speed; m_y -= speed; break;
                case 270:               m_y -= speed; break;
                case 315: m_x += speed; m_y -= speed; break;
            }
        }


        if (m_world->isInside(m_x, m_y)) {
            m_z = m_world->height(m_x, m_y);
        }

        vl::mat4 matrix = vl::mat4::getTranslation(m_x + 0.5, m_y + 0.5, 0.5 + m_z * ::sqrt(2.0 / 3.0) / 2.0);
        matrix *= vl::mat4::getRotation( m_orientation, 0, 0, 1 );
        m_transform->setLocalMatrix(matrix);
    }

    void game_unit::moveTo(int x, int y) {
        m_targetX = x;
        m_targetY = y;
    }

}