#include <vlCore/Say.hpp>
#include <vlGraphics/DrawArrays.hpp>
#include <vlGraphics/Effect.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/Rendering.hpp>

#include "Renderer.h"

namespace isomap {
    namespace client {
        namespace terrain {

            Renderer::Renderer( Terrain* terrain ) :
                    m_terrain( terrain ) {

            }

            Renderer::~Renderer() = default;

            void Renderer::init( vl::RenderingAbstract* rendering ) {
                m_sceneManager = new vl::SceneManagerActorTree;
                m_sceneManager->setCullingEnabled( false );
                rendering->as<vl::Rendering>()->sceneManagers()->push_back( m_sceneManager.get() );
            }

            void Renderer::render() {
                auto width = m_terrain->width();
                auto height = m_terrain->height();
                uint32_t quad_count = width * height * 3; // worst case scenario: all quads have 2 cliffs
                vl::ref<vl::ArrayFloat3> verts = new vl::ArrayFloat3;
                vl::ref<vl::ArrayFloat3> normals = new vl::ArrayFloat3;
                vl::ref<vl::ArrayUByte4> colors = new vl::ArrayUByte4;
                verts->resize( quad_count * 4 );
                normals->resize( quad_count * 4 );
                colors->resize( quad_count * 4 );

                auto* v = &verts->at( 0 );
                auto* n = &normals->at( 0 );
                auto* c = &colors->at( 0 );
                uint32_t quads = 0;
                for ( int y = 0; y < m_terrain->height(); ++y ) {
                    for ( int x = 0; x < m_terrain->width(); ++x ) {
                        uint32_t idx = y * m_terrain->width() + x;
                        uint8_t fog = m_terrain->fogMap()[idx];
                        if ( fog == 0 ) {
                            continue;
                        }

                        uint8_t h = m_terrain->heightMap()[idx];
                        uint8_t slope = m_terrain->slopeMap()[idx];
                        uint8_t ore = m_terrain->oreMap()[idx];

                        uint8_t ha = h - (slope & 0b0000'0001u);
                        slope >>= 1u;
                        uint8_t hb = h - (slope & 0b0000'0001u);
                        slope >>= 1u;
                        uint8_t hc = h - (slope & 0b0000'0001u);
                        slope >>= 1u;
                        uint8_t hd = h - (slope & 0b0000'0001u);
                        slope <<= 3u;

                        vl::real hra = ha * ::sqrt( 2.0 / 3.0 ) / 2.0;
                        vl::real hrb = hb * ::sqrt( 2.0 / 3.0 ) / 2.0;
                        vl::real hrc = hc * ::sqrt( 2.0 / 3.0 ) / 2.0;
                        vl::real hrd = hd * ::sqrt( 2.0 / 3.0 ) / 2.0;
                        v[0] = vl::fvec3( (vl::real)x, (vl::real)y, hra );
                        v[1] = vl::fvec3( (vl::real)x + 1, (vl::real)y, hrb );
                        v[2] = vl::fvec3( (vl::real)x + 1, (vl::real)y + 1, hrc );
                        v[3] = vl::fvec3( (vl::real)x, (vl::real)y + 1, hrd );
                        v += 4;
                        n[0] = vl::fvec3( 0, 0, 1 );
                        n[1] = vl::fvec3( 0, 0, 1 );
                        n[2] = vl::fvec3( 0, 0, 1 );
                        n[3] = vl::fvec3( 0, 0, 1 );
                        n += 4;
                        uint8_t col = h << 4u;
                        col += 8;
                        uint8_t r = col;
                        uint8_t g = col;
                        uint8_t b = col;
                        if ( h >= 4 ) {
                            // ground
                            g = 255;
                        } else {
                            // water
                            b = 255;
                        }
                        if ( x == 0 ) {
                            r = 255;
                            g = 0;
                            b = 0;
                        } else if ( x == width - 1 ) {
                            r = 128;
                            g = 0;
                            b = 0;
                        } else if ( y == 0 ) {
                            r = 0;
                            g = 255;
                            b = 0;
                        } else if ( y == height - 1 ) {
                            r = 0;
                            g = 128;
                            b = 0;
                        }
                        if ( ore > 0 ) {
                            r = 255;
                            g = ore;
                            b = ore;
                        }

                        if ( m_renderFog ) {
                            r = ((r * fog) + (r / 4u * (255 - fog))) >> 8u;
                            g = ((g * fog) + (g / 4u * (255 - fog))) >> 8u;
                            b = ((b * fog) + (b / 4u * (255 - fog))) >> 8u;
                        } else {
                            r /= 2;
                            g /= 2;
                            b /= 2;
                        }

                        if ( m_renderOccupancy ) {
                            if ( m_terrain->occupancyMap()[idx] == 0 ) {
                                // free
                                r = 0;
                                g = 128;
                                b = 0;
                            } else if ( m_terrain->occupancyMap()[idx] == 1 ) {
                                // occupied
                                r = 128;
                                g = 0;
                                b = 0;
                            } else if ( m_terrain->occupancyMap()[idx] == 2 ) {
                                // reserved
                                r = 128;
                                g = 128;
                                b = 0;
                            } else if ( m_terrain->occupancyMap()[idx] == 3 ) {
                                // reserved and occupied? :/
                                r = 128;
                                g = 64;
                                b = 0;
                            }
                        }

                        if ( m_renderHighlight && m_highlightArea.contains( x, y ) ) {
                            r = m_highlightColor.r() * 255;
                            g = m_highlightColor.g() * 255;
                            b = m_highlightColor.b() * 255;
                        }
                        for ( const auto& area : m_highLightAreas ) {
                            if ( area.first.contains( x, y ) ) {
                                r = area.second.r() * 255;
                                g = area.second.g() * 255;
                                b = area.second.b() * 255;
                                break;
                            }
                        }

                        c[0] = vl::ubvec4( r, g, b, 255 );
                        c[1] = vl::ubvec4( r, g, b, 255 );
                        c[2] = vl::ubvec4( r, g, b, 255 );
                        c[3] = vl::ubvec4( r, g, b, 255 );
                        c += 4;

                        ++quads;

                        if ( slope & 0b0001'0000u ) {
                            auto c03 = m_terrain->getCornerSafe( x, y - 1, 3 );
                            auto c02 = m_terrain->getCornerSafe( x, y - 1, 2 );
                            vl::real hc03 = c03 * ::sqrt( 2.0 / 3.0 ) / 2.0;
                            vl::real hc02 = c02 * ::sqrt( 2.0 / 3.0 ) / 2.0;
                            // TODO: figure out if we need a quad or a tri...
                            v[0] = vl::fvec3( (vl::real)x, (vl::real)y, hra );
                            v[1] = vl::fvec3( (vl::real)x, (vl::real)y, hc03 );
                            v[2] = vl::fvec3( (vl::real)x + 1, (vl::real)y, hc02 );
                            v[3] = vl::fvec3( (vl::real)x + 1, (vl::real)y, hrb );
                            v += 4;
                            n[0] = vl::fvec3( 0, -1, 0 );
                            n[1] = vl::fvec3( 0, -1, 0 );
                            n[2] = vl::fvec3( 0, -1, 0 );
                            n[3] = vl::fvec3( 0, -1, 0 );
                            n += 4;
                            c[0] = vl::ubvec4( r, g, b, 255 );
                            c[1] = vl::ubvec4( r, g, b, 255 );
                            c[2] = vl::ubvec4( r, g, b, 255 );
                            c[3] = vl::ubvec4( r, g, b, 255 );
                            c += 4;
                            ++quads;
                        }
                        if ( slope & 0b0010'0000u ) {
                            auto c10 = m_terrain->getCornerSafe( x + 1, y, 0 );
                            auto c13 = m_terrain->getCornerSafe( x + 1, y, 3 );
                            vl::real hc10 = c10 * ::sqrt( 2.0 / 3.0 ) / 2.0;
                            vl::real hc13 = c13 * ::sqrt( 2.0 / 3.0 ) / 2.0;
                            // TODO: figure out if we need a quad or a tri...
                            v[0] = vl::fvec3( (vl::real)x + 1, (vl::real)y, hrb );
                            v[1] = vl::fvec3( (vl::real)x + 1, (vl::real)y, hc10 );
                            v[2] = vl::fvec3( (vl::real)x + 1, (vl::real)y + 1, hc13 );
                            v[3] = vl::fvec3( (vl::real)x + 1, (vl::real)y + 1, hrc );
                            v += 4;
                            n[0] = vl::fvec3( 1, 0, 0 );
                            n[1] = vl::fvec3( 1, 0, 0 );
                            n[2] = vl::fvec3( 1, 0, 0 );
                            n[3] = vl::fvec3( 1, 0, 0 );
                            n += 4;
                            c[0] = vl::ubvec4( r, g, b, 255 );
                            c[1] = vl::ubvec4( r, g, b, 255 );
                            c[2] = vl::ubvec4( r, g, b, 255 );
                            c[3] = vl::ubvec4( r, g, b, 255 );
                            c += 4;
                            ++quads;
                        }
                        if ( slope & 0b0100'0000u ) {
                            auto c21 = m_terrain->getCornerSafe( x, y + 1, 1 );
                            auto c20 = m_terrain->getCornerSafe( x, y + 1, 0 );
                            vl::real hc21 = c21 * ::sqrt( 2.0 / 3.0 ) / 2.0;
                            vl::real hc20 = c20 * ::sqrt( 2.0 / 3.0 ) / 2.0;

                            // TODO: figure out if we need a quad or a tri...
                            v[0] = vl::fvec3( (vl::real)x + 1, (vl::real)y + 1, hrc );
                            v[1] = vl::fvec3( (vl::real)x + 1, (vl::real)y + 1, hc21 );
                            v[2] = vl::fvec3( (vl::real)x, (vl::real)y + 1, hc20 );
                            v[3] = vl::fvec3( (vl::real)x, (vl::real)y + 1, hrd );
                            v += 4;
                            n[0] = vl::fvec3( 0, 1, 0 );
                            n[1] = vl::fvec3( 0, 1, 0 );
                            n[2] = vl::fvec3( 0, 1, 0 );
                            n[3] = vl::fvec3( 0, 1, 0 );
                            n += 4;
                            c[0] = vl::ubvec4( r, g, b, 255 );
                            c[1] = vl::ubvec4( r, g, b, 255 );
                            c[2] = vl::ubvec4( r, g, b, 255 );
                            c[3] = vl::ubvec4( r, g, b, 255 );
                            c += 4;
                            ++quads;
                        }
                        if ( slope & 0b1000'0000u ) {
                            auto c32 = m_terrain->getCornerSafe( x - 1, y, 2 );
                            auto c31 = m_terrain->getCornerSafe( x - 1, y, 1 );
                            // TODO: figure out if we need a quad or a tri...
                            vl::real hc32 = c32 * ::sqrt( 2.0 / 3.0 ) / 2.0;
                            vl::real hc31 = c31 * ::sqrt( 2.0 / 3.0 ) / 2.0;

                            v[0] = vl::fvec3( (vl::real)x, (vl::real)y + 1, hrd );
                            v[1] = vl::fvec3( (vl::real)x, (vl::real)y + 1, hc32 );
                            v[2] = vl::fvec3( (vl::real)x, (vl::real)y, hc31 );
                            v[3] = vl::fvec3( (vl::real)x, (vl::real)y, hra );
                            v += 4;
                            n[0] = vl::fvec3( -1, 0, 0 );
                            n[1] = vl::fvec3( -1, 0, 0 );
                            n[2] = vl::fvec3( -1, 0, 0 );
                            n[3] = vl::fvec3( -1, 0, 0 );
                            n += 4;
                            c[0] = vl::ubvec4( r, g, b, 255 );
                            c[1] = vl::ubvec4( r, g, b, 255 );
                            c[2] = vl::ubvec4( r, g, b, 255 );
                            c[3] = vl::ubvec4( r, g, b, 255 );
                            c += 4;
                            ++quads;
                        }
                    }
                }

                vl::ref<vl::DrawArrays> de = new vl::DrawArrays( vl::PT_QUADS, 0, (int)quads * 4 );
                vl::ref<vl::Geometry> geom = new vl::Geometry;
                geom->drawCalls().push_back( de.get() );
                geom->setVertexArray( verts.get() );
                geom->setNormalArray( normals.get() );
                geom->setColorArray( colors.get() );

                vl::ref<vl::Effect> effect = new vl::Effect;
                effect->shader()->gocMaterial()->setColorMaterialEnabled( true );
                effect->shader()->enable( vl::EN_DEPTH_TEST );
                effect->shader()->enable( vl::EN_LIGHTING );
                effect->lod( 0 )->push_back( new vl::Shader );
                effect->shader( 0, 1 )->enable( vl::EN_BLEND );
                //effect->shader(0,1)->enable(vl::EN_LINE_SMOOTH);
                effect->shader( 0, 1 )->enable( vl::EN_DEPTH_TEST );
                effect->shader( 0, 1 )->enable( vl::EN_POLYGON_OFFSET_LINE );
                effect->shader( 0, 1 )->gocPolygonOffset()->set( -1.0f, -1.0f );
                effect->shader( 0, 1 )->gocPolygonMode()->set( vl::PM_LINE, vl::PM_LINE );
                effect->shader( 0, 1 )->gocColor()->setValue( vl::lightgreen );
                effect->shader( 0, 1 )->setRenderState( effect->shader()->getMaterial() );
                effect->shader( 0, 1 )->setRenderState( effect->shader()->getLight( 0 ), 0 );

                m_sceneManager->tree()->actors()->clear();
                m_sceneManager->tree()->addActor( geom.get(), effect.get(), nullptr );
            }


            void Renderer::disable() {
                if ( m_sceneManager ) {
                    m_sceneManager->setEnableMask( 0 );
                }
            }

            void Renderer::enable() {
                if ( m_sceneManager ) {
                    m_sceneManager->setEnableMask( 0xFFFFFFFF );
                }
            }

        }
    }
}