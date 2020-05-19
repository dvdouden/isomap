//
// Created by do0874 on 19-5-20.
//
#include <vlCore/Colors.hpp>

#include "Cursor.h"
#include "client/Structure.h"
#include "client/terrain/Renderer.h"

void Cursor::highlightTile( uint32_t x, uint32_t y, bool green ) {
    m_tiles.push_back( { x, y, green ? vl::green : vl::red } );
}

void Cursor::highlightFootPrint( uint32_t x, uint32_t y, isomap::common::FootPrint* footPrint, bool green ) {
    m_footPrint = footPrint;
    m_x = x;
    m_y = y;
    m_color = green ? vl::green : vl::red;
}

void Cursor::highlightStructure( isomap::client::Structure* structure, bool green ) {
    highlightFootPrint( structure->x(), structure->y(), structure->footPrint(), green );
}

void Cursor::render( isomap::client::terrain::Renderer* renderer ) {
    for ( auto& tile : m_tiles ) {
        renderer->addHighlight( isomap::common::Area( tile.x, tile.y, 1, 1 ), tile.color );
    }

    if ( m_footPrint != nullptr ) {
        renderer->highLight( isomap::common::Area( m_x, m_y, m_footPrint ), m_color );
    }
}
