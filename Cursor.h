#pragma once

#include <vector>

#include <vlCore/Vector4.hpp>
#include "client/types.h"

class Cursor {
public:
    void highlightTile( uint32_t x, uint32_t y, bool green );

    void highlightFootPrint( uint32_t x, uint32_t y, isomap::common::FootPrint* footPrint, bool green );

    void highlightStructure( isomap::client::Structure* structure, bool green );

    void render( isomap::client::terrain::Renderer* renderer );

private:
    uint32_t m_x;
    uint32_t m_y;
    isomap::common::FootPrint* m_footPrint;
    vl::fvec4 m_color;

    struct Tile {
        uint32_t x;
        uint32_t y;
        vl::fvec4 color;
    };

    std::vector<Tile> m_tiles;

};

