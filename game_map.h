#ifndef TESTPROJECT_GAME_MAP_H
#define TESTPROJECT_GAME_MAP_H

#include <vlCore/ResourceDatabase.hpp>


class game_map {
public:
    game_map( unsigned int width, unsigned int height );
    game_map( const game_map& ) = delete;
    ~game_map();
    const game_map& operator=( const game_map& ) = delete;

    void generate( unsigned int depth, unsigned int seed, unsigned char cliffs );
    unsigned char height( unsigned int x, unsigned int y ) const;
    unsigned char safe_height( unsigned int x, unsigned int y ) const;

    vl::ref<vl::ResourceDatabase> getDb() {
        return m_db;
    }

    void highlight(int x, int y);

    void setSize(unsigned int width, unsigned int height);

    bool isInside(int x, int y) const;

private:
    unsigned int m_width;
    unsigned int m_height;

    unsigned char* m_heightmap;
    unsigned char* m_corners;

    vl::ref<vl::ResourceDatabase> m_db;

    unsigned char getCorner(int x, int y, int i);

    int m_highlight_x = -1;
    int m_highlight_y = -1;
};


#endif //TESTPROJECT_GAME_MAP_H
