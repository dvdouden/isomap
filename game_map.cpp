#include <vlCore/Say.hpp>
#include <vlGraphics/Actor.hpp>
#include <vlGraphics/DrawArrays.hpp>
#include <vlGraphics/Effect.hpp>
#include <vlGraphics/Geometry.hpp>

#include "game_map.h"
#include "util/math.h"

namespace isomap {

    game_map::game_map(unsigned int width, unsigned int height) :
            m_width(width),
            m_height(height) {

        m_heightmap = new unsigned char[m_width * m_height];
        m_corners = new unsigned char[m_width * m_height * 4];
        m_oremap = new unsigned char[m_width * m_height];
        m_pathmap = new unsigned char[m_width * m_height];
        m_fogmap = new unsigned char[m_width * m_height];
        m_fowmap = new unsigned char[m_width * m_height];


        memset( m_fogmap, 0, m_width * m_height);
        memset( m_fowmap, 0, m_width * m_height);
    }

    game_map::~game_map() {
        delete[] m_heightmap;
        delete[] m_corners;
        delete[] m_oremap;
        delete[] m_pathmap;
        delete[] m_fogmap;
        delete[] m_fowmap;
    }


    unsigned char clamp(int a) {
        return a > 255 ? 255 : (a < 0 ? 0 : a);
    }

    void diamond(
            unsigned char *data,
            unsigned int width,
            unsigned int height,
            unsigned int scale,
            int depth,
            math::rng &rnd) {
        // diamond
        unsigned int hs = scale / 2;
        for (auto y = hs; y < height; y += scale) {
            for (auto x = hs; x < width; x += scale) {
                int i0 = data[(y - hs) * width + (x - hs)];
                int i1 = data[(y - hs) * width + (x + hs)];
                int i2 = data[(y + hs) * width + (x - hs)];
                int i3 = data[(y + hs) * width + (x + hs)];
                int avg = (i0 + i1 + i2 + i3) / 4;
                data[y * width + x] = clamp(avg + rnd(-depth, depth));
            }
        }
    }


    void square(
            unsigned char *data,
            unsigned int width,
            unsigned int height,
            unsigned int scale,
            int depth,
            math::rng &rnd) {
        // diamond
        for (unsigned int y = 0; y < height; y += scale) {
            for (unsigned int x = 0; x < width; x += scale) {
                if (x < width - 1) {
                    unsigned int x1 = x + scale / 2;
                    // square the one on the right
                    int avg = data[y * width + x] + data[y * width + x + scale];
                    int cnt = 2;
                    if (y > 0) {
                        avg += data[(y - scale / 2) * width + x1];
                        cnt++;
                    }
                    if (y < height - 1) {
                        avg += data[(y + scale / 2) * width + x1];
                        cnt++;
                    }
                    avg /= cnt;
                    data[y * width + x1] = clamp(avg + rnd(-depth, depth));
                }
                if (y < height - 1) {
                    unsigned int y1 = y + scale / 2;
                    // square the one below
                    int avg = data[y * width + x] + data[(y + scale) * width + x];
                    int cnt = 2;
                    if (x > 0) {
                        avg += data[y1 * width + x - scale / 2];
                        cnt++;
                    }
                    if (x < width - 1) {
                        avg += data[y1 * width + x + scale / 2];
                        cnt++;
                    }
                    avg /= cnt;
                    data[y1 * width + x] = clamp(avg + rnd(-depth, depth));
                }
            }
        }
    }


    unsigned char *squareDiamond(unsigned int width, unsigned int height, unsigned int scale, math::rng &rnd) {
        auto *tmp = new unsigned char[width * height];
        for (int i = 0; i < width * height; ++i) {
            tmp[i] = 0;
        }

        // initial corner values
        for (unsigned int y = 0; y < height; y += scale) {
            for (unsigned int x = 0; x < width; x += scale) {
                tmp[y * width + x] = rnd(256);
            }
        }

        unsigned int magnitude = 128;
        while (scale > 1) {
            diamond(tmp, width, height, scale, (int) magnitude, rnd);
            square(tmp, width, height, scale, (int) magnitude, rnd);
            scale >>= 1u;
            magnitude >>= 1u;
        }
        return tmp;
    }


    unsigned char game_map::getCorner(int x, int y, int i) {
        if (x < 0) {
            x = 0;
            if (i == 1) {
                i = 0;
            } else if (i == 2) {
                i = 3;
            }
        } else if (x >= m_width) {
            x = (int) m_width - 1;
            if (i == 0) {
                i = 1;
            } else if (i == 3) {
                i = 2;
            }
        }
        if (y < 0) {
            y = 0;
            if (i == 2) {
                i = 1;
            } else if (i == 3) {
                i = 0;
            }
        } else if (y >= m_height) {
            y = (int) m_height - 1;
            if (i == 0) {
                i = 3;
            } else if (i == 1) {
                i = 2;
            }
        }
        return m_corners[(y * m_width + x) * 4 + i];
    }


// TODO: move to map generator
    void game_map::generate(
            unsigned int depth,
            unsigned int seed,
            unsigned char cliffAmount,
            unsigned char oreAmount,
            unsigned char oreDensity) {
        math::rng rnd(seed);
        // use diamond-square algorithm
        // scale up to a multiple of 2^depth + 1
        unsigned int scale = 1u << depth;
        unsigned int width = ((m_width + (scale - 1)) / scale) * scale + 1;
        unsigned int height = ((m_height + (scale - 1)) / scale) * scale + 1;
        unsigned char* height_map = squareDiamond(width, height, scale, rnd);
        unsigned char* smooth_map = squareDiamond(width, height, scale / 2, rnd);
        unsigned char *scr_h = height_map;
        unsigned char *scr_s = smooth_map;
        for (int i = 0; i < width * height; ++i) {
            if (*scr_s >= cliffAmount) {
                *scr_h >>= 6u;
            } else {
                if (*scr_h >= 128) {
                    *scr_h = 3;
                } else {
                    *scr_h = 0;
                }
            }
            ++scr_h;
            ++scr_s;
        }

        // copy data to actual map
        for (int y = 0; y < m_height; ++y) {
            memcpy(m_heightmap + (y * m_width), height_map + (y * width), m_width);
        }

        delete[] height_map;
        delete[] smooth_map;

        generateOreMap( width, height, scale, rnd, oreAmount, oreDensity );

        // smooth out the map, get rid of peaks and holes
        for (int p = 0; p < 4; ++p) {
            for (int y = 0; y < m_height; ++y) {
                for (int x = 0; x < m_width; ++x) {
                    auto h = m_heightmap[y * m_width + x];
                    auto h2 = safe_height(x, y - 1);
                    auto h4 = safe_height(x + 1, y);
                    auto h6 = safe_height(x, y + 1);
                    auto h8 = safe_height(x - 1, y);

                    if (h == 0 && h2 >= 2 && h4 >= 2 && h6 >= 2 && h8 >= 2) {
                        m_heightmap[y * m_width + x] = 3;
                    }
                    if (h == 3 && h2 <= 1 && h4 <= 1 && h6 <= 1 && h8 <= 1) {
                        m_heightmap[y * m_width + x] = 0;
                    }
                }
            }
        }


        // smooth out the map, get rid of bumps that would always end up flat anyway
        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width; ++x) {
                auto h = m_heightmap[y * m_width + x];
                auto h1 = safe_height(x - 1, y - 1);
                auto h2 = safe_height(x, y - 1);
                auto h3 = safe_height(x + 1, y - 1);
                auto h4 = safe_height(x + 1, y);
                auto h5 = safe_height(x + 1, y + 1);
                auto h6 = safe_height(x, y + 1);
                auto h7 = safe_height(x - 1, y + 1);
                auto h8 = safe_height(x - 1, y);

                auto ha = h;
                if ((h8 < h || h1 < h || h2 < h) && !(h8 < h - 1 || h1 < h - 1 || h2 < h - 1)) {
                    ha = h - 1;
                }

                auto hb = h;
                if ((h2 < h || h3 < h || h4 < h) && !(h2 < h - 1 || h3 < h - 1 || h4 < h - 1)) {
                    hb = h - 1;
                }

                auto hc = h;
                if ((h4 < h || h5 < h || h6 < h) && !(h4 < h - 1 || h5 < h - 1 || h6 < h - 1)) {
                    hc = h - 1;
                }

                auto hd = h;
                if ((h6 < h || h7 < h || h8 < h) && !(h6 < h - 1 || h7 < h - 1 || h8 < h - 1)) {
                    hd = h - 1;
                }

                if (ha == hb && hc == hd && ha == hc) {
                    m_heightmap[y * m_width + x] = ha;
                }
            }
        }

        // now calculate the corners of each quad
        unsigned char *scr_c = m_corners;
        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width; ++x) {
                auto h = m_heightmap[y * m_width + x];
                auto h1 = safe_height(x - 1, y - 1);
                auto h2 = safe_height(x, y - 1);
                auto h3 = safe_height(x + 1, y - 1);
                auto h4 = safe_height(x + 1, y);
                auto h5 = safe_height(x + 1, y + 1);
                auto h6 = safe_height(x, y + 1);
                auto h7 = safe_height(x - 1, y + 1);
                auto h8 = safe_height(x - 1, y);

                auto ha = h;
                if ((h8 < h || h1 < h || h2 < h) && !(h8 < h - 1 || h1 < h - 1 || h2 < h - 1)) {
                    ha = h - 1;
                }

                auto hb = h;
                if ((h2 < h || h3 < h || h4 < h) && !(h2 < h - 1 || h3 < h - 1 || h4 < h - 1)) {
                    hb = h - 1;
                }

                auto hc = h;
                if ((h4 < h || h5 < h || h6 < h) && !(h4 < h - 1 || h5 < h - 1 || h6 < h - 1)) {
                    hc = h - 1;
                }

                auto hd = h;
                if ((h6 < h || h7 < h || h8 < h) && !(h6 < h - 1 || h7 < h - 1 || h8 < h - 1)) {
                    hd = h - 1;
                }
                scr_c[0] = ha;
                scr_c[1] = hb;
                scr_c[2] = hc;
                scr_c[3] = hd;
                scr_c += 4;
            }
        }

        // calculate the number of cliff quads
        int cliffs = 0;
        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width; ++x) {
                // get quad corners
                auto c0 = getCorner(x, y, 0);
                auto c1 = getCorner(x, y, 1);
                auto c2 = getCorner(x, y, 2);
                auto c3 = getCorner(x, y, 3);

                // get adjacent corners
                auto c03 = getCorner(x, y - 1, 3);
                auto c02 = getCorner(x, y - 1, 2);

                auto c10 = getCorner(x + 1, y, 0);
                auto c13 = getCorner(x + 1, y, 3);

                auto c21 = getCorner(x, y + 1, 1);
                auto c20 = getCorner(x, y + 1, 0);

                auto c32 = getCorner(x - 1, y, 2);
                auto c31 = getCorner(x - 1, y, 1);

                if (c0 > c03 || c1 > c02) {
                    ++cliffs;
                }
                if (c1 > c10 || c2 > c13) {
                    ++cliffs;
                }
                if (c2 > c21 || c3 > c20) {
                    ++cliffs;
                }
                if (c3 > c32 || c0 > c31) {
                    ++cliffs;
                }
            }
        }

        updatePathMap();


        unsigned int quad_count = m_width * m_height + 6 + cliffs;

        vl::ref<vl::ArrayFloat3> verts = new vl::ArrayFloat3;
        vl::ref<vl::ArrayFloat3> normals = new vl::ArrayFloat3;
        vl::ref<vl::ArrayUByte4> colors = new vl::ArrayUByte4;
        verts->resize(quad_count * 4);
        normals->resize(quad_count * 4);
        colors->resize(quad_count * 4);

        auto *v = &verts->at(0);
        auto *n = &normals->at(0);
        auto *c = &colors->at(0);
        scr_c = m_corners;
        unsigned int quads = 0;
        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width; ++x) {

                auto h = m_heightmap[y * m_width + x];
                auto ore = m_oremap[y * m_width + x];
                auto ha = scr_c[0];
                auto hb = scr_c[1];
                auto hc = scr_c[2];
                auto hd = scr_c[3];
                scr_c += 4;

                auto fog = m_fogmap[y * m_width + x];
                if ( !fog ) {
                    continue;
                }
                auto fow = m_fowmap[y * m_width + x];

                vl::real hra = ha * ::sqrt(2.0 / 3.0) / 2.0;
                vl::real hrb = hb * ::sqrt(2.0 / 3.0) / 2.0;
                vl::real hrc = hc * ::sqrt(2.0 / 3.0) / 2.0;
                vl::real hrd = hd * ::sqrt(2.0 / 3.0) / 2.0;
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
                if (x == m_highlight_x && y == m_highlight_y) {
                    r = 255;
                    g = 0;
                    b = 255;
                }

                r = ((r * fow) + (r / 4 * (255 - fow))) >> 8;
                g = ((g * fow) + (g / 4 * (255 - fow))) >> 8;
                b = ((b * fow) + (b / 4 * (255 - fow))) >> 8;


                c[0] = vl::ubvec4(r, g, b, 255);
                c[1] = vl::ubvec4(r, g, b, 255);
                c[2] = vl::ubvec4(r, g, b, 255);
                c[3] = vl::ubvec4(r, g, b, 255);
                c += 4;

                ++quads;


                // get adjacent corners
                auto c03 = getCorner(x, y - 1, 3);
                auto c02 = getCorner(x, y - 1, 2);

                auto c10 = getCorner(x + 1, y, 0);
                auto c13 = getCorner(x + 1, y, 3);

                auto c21 = getCorner(x, y + 1, 1);
                auto c20 = getCorner(x, y + 1, 0);

                auto c32 = getCorner(x - 1, y, 2);
                auto c31 = getCorner(x - 1, y, 1);


                vl::real hc03 = c03 * ::sqrt(2.0 / 3.0) / 2.0;
                vl::real hc02 = c02 * ::sqrt(2.0 / 3.0) / 2.0;
                vl::real hc10 = c10 * ::sqrt(2.0 / 3.0) / 2.0;
                vl::real hc13 = c13 * ::sqrt(2.0 / 3.0) / 2.0;
                vl::real hc21 = c21 * ::sqrt(2.0 / 3.0) / 2.0;
                vl::real hc20 = c20 * ::sqrt(2.0 / 3.0) / 2.0;
                vl::real hc32 = c32 * ::sqrt(2.0 / 3.0) / 2.0;
                vl::real hc31 = c31 * ::sqrt(2.0 / 3.0) / 2.0;

                if (ha > c03 || hb > c02) {
                    // TODO: figure out if we need a quad or a tri...
                    v[0] = vl::fvec3((vl::real) x, (vl::real) y, hra);
                    v[1] = vl::fvec3((vl::real) x, (vl::real) y, hc03);
                    v[2] = vl::fvec3((vl::real) x + 1, (vl::real) y, hc02);
                    v[3] = vl::fvec3((vl::real) x + 1, (vl::real) y, hrb);
                    v += 4;
                    n[0] = vl::fvec3(0, -1, 0);
                    n[1] = vl::fvec3(0, -1, 0);
                    n[2] = vl::fvec3(0, -1, 0);
                    n[3] = vl::fvec3(0, -1, 0);
                    n += 4;
                    c[0] = vl::ubvec4(r, g, b, 255);
                    c[1] = vl::ubvec4(r, g, b, 255);
                    c[2] = vl::ubvec4(r, g, b, 255);
                    c[3] = vl::ubvec4(r, g, b, 255);
                    c += 4;
                    ++quads;
                }
                if (hb > c10 || hc > c13) {
                    // TODO: figure out if we need a quad or a tri...
                    v[0] = vl::fvec3((vl::real) x + 1, (vl::real) y, hrb);
                    v[1] = vl::fvec3((vl::real) x + 1, (vl::real) y, hc10);
                    v[2] = vl::fvec3((vl::real) x + 1, (vl::real) y + 1, hc13);
                    v[3] = vl::fvec3((vl::real) x + 1, (vl::real) y + 1, hrc);
                    v += 4;
                    n[0] = vl::fvec3(1, 0, 0);
                    n[1] = vl::fvec3(1, 0, 0);
                    n[2] = vl::fvec3(1, 0, 0);
                    n[3] = vl::fvec3(1, 0, 0);
                    n += 4;
                    c[0] = vl::ubvec4(r, g, b, 255);
                    c[1] = vl::ubvec4(r, g, b, 255);
                    c[2] = vl::ubvec4(r, g, b, 255);
                    c[3] = vl::ubvec4(r, g, b, 255);
                    c += 4;
                    ++quads;
                }
                if (hc > c21 || hd > c20) {
                    // TODO: figure out if we need a quad or a tri...
                    v[0] = vl::fvec3((vl::real) x + 1, (vl::real) y + 1, hrc);
                    v[1] = vl::fvec3((vl::real) x + 1, (vl::real) y + 1, hc21);
                    v[2] = vl::fvec3((vl::real) x, (vl::real) y + 1, hc20);
                    v[3] = vl::fvec3((vl::real) x, (vl::real) y + 1, hrd);
                    v += 4;
                    n[0] = vl::fvec3(0, 1, 0);
                    n[1] = vl::fvec3(0, 1, 0);
                    n[2] = vl::fvec3(0, 1, 0);
                    n[3] = vl::fvec3(0, 1, 0);
                    n += 4;
                    c[0] = vl::ubvec4(r, g, b, 255);
                    c[1] = vl::ubvec4(r, g, b, 255);
                    c[2] = vl::ubvec4(r, g, b, 255);
                    c[3] = vl::ubvec4(r, g, b, 255);
                    c += 4;
                    ++quads;
                }
                if (hd > c32 || ha > c31) {
                    // TODO: figure out if we need a quad or a tri...
                    v[0] = vl::fvec3((vl::real) x, (vl::real) y + 1, hrd);
                    v[1] = vl::fvec3((vl::real) x, (vl::real) y + 1, hc32);
                    v[2] = vl::fvec3((vl::real) x, (vl::real) y, hc31);
                    v[3] = vl::fvec3((vl::real) x, (vl::real) y, hra);
                    v += 4;
                    n[0] = vl::fvec3(-1, 0, 0);
                    n[1] = vl::fvec3(-1, 0, 0);
                    n[2] = vl::fvec3(-1, 0, 0);
                    n[3] = vl::fvec3(-1, 0, 0);
                    n += 4;
                    c[0] = vl::ubvec4(r, g, b, 255);
                    c[1] = vl::ubvec4(r, g, b, 255);
                    c[2] = vl::ubvec4(r, g, b, 255);
                    c[3] = vl::ubvec4(r, g, b, 255);
                    c += 4;
                    ++quads;
                }
            }
        }

        // X axis
        v[0] = vl::fvec3(0, 0, 0);
        v[1] = vl::fvec3(5, 0, 0);
        v[2] = vl::fvec3(5, 1, 0);
        v[3] = vl::fvec3(0, 1, 0);
        v += 4;
        n[0] = vl::fvec3(0, 0, 1);
        n[1] = vl::fvec3(0, 0, 1);
        n[2] = vl::fvec3(0, 0, 1);
        n[3] = vl::fvec3(0, 0, 1);
        n += 4;
        c[0] = vl::ubvec4(255, 0, 0, 255);
        c[1] = vl::ubvec4(255, 0, 0, 255);
        c[2] = vl::ubvec4(255, 0, 0, 255);
        c[3] = vl::ubvec4(255, 0, 0, 255);
        c += 4;

        // X axis
        v[0] = vl::fvec3(0, 0, 0);
        v[1] = vl::fvec3(-5, 0, 0);
        v[2] = vl::fvec3(-5, -1, 0);
        v[3] = vl::fvec3(0, -1, 0);
        v += 4;
        n[0] = vl::fvec3(0, 0, -1);
        n[1] = vl::fvec3(0, 0, -1);
        n[2] = vl::fvec3(0, 0, -1);
        n[3] = vl::fvec3(0, 0, -1);
        n += 4;
        c[0] = vl::ubvec4(128, 0, 0, 255);
        c[1] = vl::ubvec4(128, 0, 0, 255);
        c[2] = vl::ubvec4(128, 0, 0, 255);
        c[3] = vl::ubvec4(128, 0, 0, 255);
        c += 4;


        // Y axis
        v[0] = vl::fvec3(0, 0, 0);
        v[1] = vl::fvec3(0, 5, 0);
        v[2] = vl::fvec3(0, 5, 1);
        v[3] = vl::fvec3(0, 0, 1);
        v += 4;
        n[0] = vl::fvec3(1, 0, 0);
        n[1] = vl::fvec3(1, 0, 0);
        n[2] = vl::fvec3(1, 0, 0);
        n[3] = vl::fvec3(1, 0, 0);
        n += 4;
        c[0] = vl::ubvec4(0, 255, 0, 255);
        c[1] = vl::ubvec4(0, 255, 0, 255);
        c[2] = vl::ubvec4(0, 255, 0, 255);
        c[3] = vl::ubvec4(0, 255, 0, 255);
        c += 4;

        // Y axis
        v[0] = vl::fvec3(0, 0, 0);
        v[1] = vl::fvec3(0, -5, 0);
        v[2] = vl::fvec3(0, -5, -1);
        v[3] = vl::fvec3(0, 0, -1);
        v += 4;
        n[0] = vl::fvec3(-1, 0, 0);
        n[1] = vl::fvec3(-1, 0, 0);
        n[2] = vl::fvec3(-1, 0, 0);
        n[3] = vl::fvec3(-1, 0, 0);
        n += 4;
        c[0] = vl::ubvec4(0, 128, 0, 255);
        c[1] = vl::ubvec4(0, 128, 0, 255);
        c[2] = vl::ubvec4(0, 128, 0, 255);
        c[3] = vl::ubvec4(0, 128, 0, 255);
        c += 4;


        // Z axis
        v[0] = vl::fvec3(0, 0, 0);
        v[1] = vl::fvec3(0, 0, 5);
        v[2] = vl::fvec3(1, 0, 5);
        v[3] = vl::fvec3(1, 0, 0);
        v += 4;
        n[0] = vl::fvec3(0, 1, 0);
        n[1] = vl::fvec3(0, 1, 0);
        n[2] = vl::fvec3(0, 1, 0);
        n[3] = vl::fvec3(0, 1, 0);
        n += 4;
        c[0] = vl::ubvec4(0, 0, 255, 255);
        c[1] = vl::ubvec4(0, 0, 255, 255);
        c[2] = vl::ubvec4(0, 0, 255, 255);
        c[3] = vl::ubvec4(0, 0, 255, 255);
        c += 4;

        // Z axis
        v[0] = vl::fvec3(0, 0, 0);
        v[1] = vl::fvec3(0, 0, -5);
        v[2] = vl::fvec3(-1, 0, -5);
        v[3] = vl::fvec3(-1, 0, 0);

        n[0] = vl::fvec3(0, -1, 0);
        n[1] = vl::fvec3(0, -1, 0);
        n[2] = vl::fvec3(0, -1, 0);
        n[3] = vl::fvec3(0, -1, 0);

        c[0] = vl::ubvec4(0, 0, 128, 255);
        c[1] = vl::ubvec4(0, 0, 128, 255);
        c[2] = vl::ubvec4(0, 0, 128, 255);
        c[3] = vl::ubvec4(0, 0, 128, 255);

        quads += 6;


        vl::ref<vl::DrawArrays> de = new vl::DrawArrays(vl::PT_QUADS, 0, (int) quads * 4);
        vl::ref<vl::Geometry> geom = new vl::Geometry;
        geom->drawCalls().push_back(de.get());
        geom->setVertexArray(verts.get());
        geom->setNormalArray(normals.get());
        geom->setColorArray(colors.get());

        m_db = new vl::ResourceDatabase;
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
        m_db->resources().emplace_back(geom);
        m_db->resources().push_back(new vl::Actor(geom.get(), effect.get(), nullptr));
        m_db->resources().emplace_back(effect.get());
    }

    unsigned char game_map::height(unsigned int x, unsigned int y) const {
        return m_heightmap[y * m_width + x];
    }


    unsigned char game_map::safe_height(unsigned int x, unsigned int y) const {
        if (y < 0) y = 0;
        if (y >= m_height) y = m_height - 1;
        if (x < 0) x = 0;
        if (x >= m_width) x = m_width - 1;
        return m_heightmap[y * m_width + x];
    }

    void game_map::highlight(int x, int y) {
        m_highlight_x = x;
        m_highlight_y = y;
    }

    void game_map::unfog( int x, int y, int radius ) {
        int cnt = 0;
        int radiusSquared = radius * radius;
        for ( int tile_y = y - radius; tile_y <= y + radius; ++tile_y ) {
            if ( tile_y < 0 || tile_y >= m_height ) {
                continue;
            }

            int deltay = (tile_y - y) * (tile_y - y);
            for ( int tile_x = x - radius; tile_x <= x + radius; ++tile_x ) {

                if ( tile_x < 0 || tile_x >= m_width ) {
                    continue;
                }
                ++cnt;
                int deltax = (tile_x - x) * (tile_x - x);
                if ( deltax + deltay <= radiusSquared ) {
                    m_fogmap[ tile_y * m_width + tile_x ] = 1;
                    m_fowmap[ tile_y * m_width + tile_x ] = 255;
                }
            }
        }
    }

    void game_map::setSize(unsigned int width, unsigned int height) {
        if ( width == m_width && height == m_height ) {
            return;
        }
        if (width * height > m_width * m_height) {
            // don't bother resizing the buffers when the new buffers would be smaller...
            delete[] m_heightmap;
            delete[] m_corners;
            delete[] m_oremap;
            delete[] m_pathmap;
            delete[] m_fogmap;
            delete[] m_fowmap;
            m_heightmap = new unsigned char[width * height];
            m_corners = new unsigned char[width * height * 4];
            m_oremap = new unsigned char[width * height];
            m_pathmap = new unsigned char[width * height];
            m_fogmap = new unsigned char[m_width * m_height];
            m_fowmap = new unsigned char[m_width * m_height];

        }
        m_width = width;
        m_height = height;
        memset( m_fogmap, 0, m_width * m_height);
        memset( m_fowmap, 0, m_width * m_height);
    }

    void game_map::update() {
        auto* scratch = m_fowmap;
        for ( int y = 0; y < m_height; ++y ) {
            for ( int x = 0; x < m_width; ++x ) {
                if ( *scratch > 0 ) {
                    *scratch -= 1;
                }
                ++scratch;
            }
        }
    }

    bool game_map::isInside(int x, int y) const {
        return x >= 0 && x < m_width && y >= 0 && y < m_height;
    }

    void game_map::updatePathMap() {
        // update pathmap
        unsigned char* scr_p = m_pathmap;
        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width; ++x) {
                unsigned char canReach = 0;
                auto h = m_heightmap[y * m_width + x];

                if ( x > 0 && y > 0 ) {
                    auto h0 = safe_height( x - 1, y - 1 );
                    if ( h0 == h || h0 - h == 1 || h - h0 == 1 ) {
                        canReach |= 0b0000'0001u;
                    }
                }
                if ( y > 0 ) {
                    auto h1 = safe_height( x, y - 1 );
                    if ( h1 == h || h1 - h == 1 || h - h1 == 1 ) {
                        canReach |= 0b0000'0010u;
                    }
                }
                if ( x < m_width - 1 && y > 0 ) {
                    auto h2 = safe_height( x + 1, y - 1 );
                    if ( h2 == h || h2 - h == 1 || h - h2 == 1 ) {
                        canReach |= 0b0000'0100u;
                    }
                }
                if ( x < m_width - 1 ) {
                    auto h3 = safe_height( x + 1, y );
                    if ( h3 == h || h3 - h == 1 || h - h3 == 1 ) {
                        canReach |= 0b0000'1000u;
                    }
                }
                if ( x < m_width - 1 && y < m_height - 1 ) {
                    auto h4 = safe_height( x + 1, y + 1 );
                    if ( h4 == h || h4 - h == 1 || h - h4 == 1 ) {
                        canReach |= 0b0001'0000u;
                    }
                }
                if ( y < m_height - 1 ) {
                    auto h5 = safe_height( x, y + 1 );
                    if ( h5 == h || h5 - h == 1 || h - 54 == 1 ) {
                        canReach |= 0b0010'0000u;
                    }
                }
                if ( x > 0 && y < m_height - 1 ) {
                    auto h6 = safe_height( x - 1, y + 1 );
                    if ( h6 == h || h6 - h == 1 || h - h6 == 1 ) {
                        canReach |= 0b0100'0000u;
                    }
                }
                if ( x > 0 ) {
                    auto h7 = safe_height( x - 1, y );
                    if ( h7 == h || h7 - h == 1 || h - h7 == 1 ) {
                        canReach |= 0b1000'0000u;
                    }
                }

                *scr_p = canReach;
                ++scr_p;
            }
        }
    }

    void game_map::generateOreMap(
            unsigned int width,
            unsigned int height,
            unsigned int scale,
            math::rng &rnd,
            unsigned char oreAmount,
            unsigned char oreDensity) {

        // create ore map
        unsigned char* ore_map = squareDiamond(width, height, scale, rnd);
        unsigned char *scr_o = ore_map;
        for (int i = 0; i < width * height; ++i) {
            if (*scr_o >= oreAmount) {
                *scr_o = 0;
            } else {
                // tiles with 0 have highest concentration
                // density runs from 0 (0%, all tiles minimum) to 255 (200% half tiles max)
                unsigned int amount = (oreAmount - *scr_o);
                // linear interpolation from (oreDensity * 2) to 1
                amount = ((oreDensity * 2) * amount) / oreAmount;
                if ( amount > 255 ) {
                    // clamp at 255
                    amount = 255;
                } else if ( amount == 0 ) {
                    // ensure a minimum of 1
                    amount = 1;
                }
                *scr_o = amount;
            }
            ++scr_o;
        }

        // copy data to actual oremap
        for (int y = 0; y < m_height; ++y) {
            memcpy(m_oremap + (y * m_width), ore_map + (y * width), m_width);
        }

        delete[] ore_map;
    }

}
