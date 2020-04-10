#include <cstring>

#include <vlCore/Say.hpp>
#include <vlGraphics/DrawArrays.hpp>
#include <vlGraphics/Effect.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <vlGraphics/Rendering.hpp>

#include "Terrain.h"
#include "../common/TerrainMessage.h"

namespace isomap {
    namespace client {

        void Terrain::processMessage( isomap::common::TerrainMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            switch ( msg->type() ) {
                case common::TerrainMessage::Create:
                    m_width = msg->width();
                    m_height = msg->height();
                    m_heightMap = new uint8_t[m_width * m_height];
                    m_oreMap = new uint8_t[m_width * m_height];
                    m_fogMap = new uint8_t[m_width * m_height];
                    ::memset( m_heightMap, 0, m_width * m_height );
                    ::memset( m_oreMap, 0, m_width * m_height );
                    ::memset( m_fogMap, 0, m_width * m_height );
                    break;

                case common::TerrainMessage::Update:
                    for ( const auto& cell : msg->cells() ) {
                        m_heightMap[cell.id] = cell.height;
                        m_oreMap[cell.id] = cell.ore;
                        m_fogMap[cell.id] = 255;
                    }
                    break;

                default:
                    break;
            }
        }

        void Terrain::initRender( vl::RenderingAbstract* rendering ) {
            m_sceneManager = new vl::SceneManagerActorTree;
            m_sceneManager->setCullingEnabled(false);
            rendering->as<vl::Rendering>()->sceneManagers()->push_back(m_sceneManager.get());
        }

        void Terrain::render() {
            uint32_t quad_count = m_width * m_height;
            vl::ref<vl::ArrayFloat3> verts = new vl::ArrayFloat3;
            vl::ref<vl::ArrayFloat3> normals = new vl::ArrayFloat3;
            vl::ref<vl::ArrayUByte4> colors = new vl::ArrayUByte4;
            verts->resize(quad_count * 4);
            normals->resize(quad_count * 4);
            colors->resize(quad_count * 4);

            auto *v = &verts->at(0);
            auto *n = &normals->at(0);
            auto *c = &colors->at(0);
            uint32_t quads = 0;
            for (int y = 0; y < m_height; ++y) {
                for (int x = 0; x < m_width; ++x) {
                    if ( m_fogMap[y * m_width + x] == 0 ) {
                        continue;
                    }

                    auto h = m_heightMap[y * m_width + x];
                    auto ore = m_oreMap[y * m_width + x];

                    vl::real hra = h * ::sqrt(2.0 / 3.0) / 2.0;
                    vl::real hrb = h * ::sqrt(2.0 / 3.0) / 2.0;
                    vl::real hrc = h * ::sqrt(2.0 / 3.0) / 2.0;
                    vl::real hrd = h * ::sqrt(2.0 / 3.0) / 2.0;
                    v[0] = vl::fvec3((vl::real) x, (vl::real) y, hra);
                    v[1] = vl::fvec3((vl::real) x + 1, (vl::real) y, hrb);
                    v[2] = vl::fvec3((vl::real) x + 1, (vl::real) y + 1, hrc);
                    v[3] = vl::fvec3((vl::real) x, (vl::real) y + 1, hrd);
                    v += 4;
                    n[0] = vl::fvec3(0, 0, 1);
                    n[1] = vl::fvec3(0, 0, 1);
                    n[2] = vl::fvec3(0, 0, 1);
                    n[3] = vl::fvec3(0, 0, 1);
                    n += 4;
                    auto col = h << 6u;
                    col += 16;
                    auto r = col;
                    auto g = 255;
                    auto b = col;
                    if (x == 0) {
                        r = 255;
                        g = 0;
                        b = 0;
                    } else if (x == m_width - 1) {
                        r = 128;
                        g = 0;
                        b = 0;
                    } else if (y == 0) {
                        r = 0;
                        g = 255;
                        b = 0;
                    } else if (y == m_height - 1) {
                        r = 0;
                        g = 128;
                        b = 0;
                    }
                    if ( ore > 0 ) {
                        r = ore;
                        g = ore;
                        b = 255;
                    }

                    c[0] = vl::ubvec4(r, g, b, 255);
                    c[1] = vl::ubvec4(r, g, b, 255);
                    c[2] = vl::ubvec4(r, g, b, 255);
                    c[3] = vl::ubvec4(r, g, b, 255);
                    c += 4;

                    ++quads;

                }
            }

            vl::ref<vl::DrawArrays> de = new vl::DrawArrays(vl::PT_QUADS, 0, (int) quads * 4);
            vl::ref<vl::Geometry> geom = new vl::Geometry;
            geom->drawCalls().push_back(de.get());
            geom->setVertexArray(verts.get());
            geom->setNormalArray(normals.get());
            geom->setColorArray(colors.get());

            vl::ref<vl::Effect> effect = new vl::Effect;
            effect->shader()->gocMaterial()->setColorMaterialEnabled(true);
            effect->shader()->enable(vl::EN_DEPTH_TEST);
            effect->shader()->enable(vl::EN_LIGHTING);
            effect->lod(0)->push_back(new vl::Shader);
            effect->shader(0, 1)->enable(vl::EN_BLEND);
            //effect->shader(0,1)->enable(vl::EN_LINE_SMOOTH);
            effect->shader(0, 1)->enable(vl::EN_DEPTH_TEST);
            effect->shader(0, 1)->enable(vl::EN_POLYGON_OFFSET_LINE);
            effect->shader(0, 1)->gocPolygonOffset()->set(-1.0f, -1.0f);
            effect->shader(0, 1)->gocPolygonMode()->set(vl::PM_LINE, vl::PM_LINE);
            effect->shader(0, 1)->gocColor()->setValue(vl::lightgreen);
            effect->shader(0, 1)->setRenderState(effect->shader()->getMaterial());
            effect->shader(0, 1)->setRenderState(effect->shader()->getLight(0), 0);

            m_sceneManager->tree()->actors()->clear();
            m_sceneManager->tree()->addActor(geom.get(), effect.get(), nullptr);
        }

    }
}