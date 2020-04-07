#ifndef TESTPROJECT_GAME_MAP_H
#define TESTPROJECT_GAME_MAP_H

#include <vlCore/ResourceDatabase.hpp>

#include "types.h"
#include "util/math.h"

namespace isomap {

    class game_map {
    public:
        game_map(unsigned int width, unsigned int height);

        game_map(const game_map &) = delete;

        ~game_map();

        const game_map &operator=(const game_map &) = delete;

        void generate(unsigned int depth,
                unsigned int seed,
                unsigned char cliffs,
                unsigned char oreAmount,
                unsigned char oreDensity);

        unsigned char height(unsigned int x, unsigned int y) const;

        unsigned char safe_height(unsigned int x, unsigned int y) const;

        vl::ref<vl::ResourceDatabase> getDb() {
            return m_db;
        }

        void highlight(int x, int y);

        void setSize(unsigned int width, unsigned int height);

        bool isInside(int x, int y) const;

        unsigned int width() const {
            return m_width;
        }

        unsigned int height() const {
            return m_height;
        }

        unsigned char canReach( int x, int y ) const {
            if ( x < 0 || x >= m_width || y < 0 || y >= m_height ) {
                return 0;
            }
            return m_pathmap[y * m_width + x];
        }

        void unfog( int x, int y, int radius );

        void update();

    private:
        unsigned int m_width;
        unsigned int m_height;

        unsigned char* m_heightmap = nullptr;
        unsigned char* m_corners = nullptr;
        unsigned char* m_oremap = nullptr;
        unsigned char* m_pathmap = nullptr;
        unsigned char* m_fogmap = nullptr;
        unsigned char* m_fowmap = nullptr;

        vl::ref<vl::ResourceDatabase> m_db;

        unsigned char getCorner(int x, int y, int i);

        int m_highlight_x = -1;
        int m_highlight_y = -1;

        void updatePathMap();

        void generateOreMap(unsigned int width, unsigned int height, unsigned int scale, math::rng &rng,
                            unsigned char amount,
                            unsigned char density);
    };

}

#endif //TESTPROJECT_GAME_MAP_H
