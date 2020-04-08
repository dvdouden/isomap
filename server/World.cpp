#include "World.h"

namespace isomap { namespace server {

    World::World(uint32_t width, uint32_t height) :
    m_width( width ),
    m_height( height )
    {
        m_heightMap = new uint8_t[m_width * m_height];
        m_oreMap = new uint8_t[m_width * m_height];
    }

    World::~World() {
        delete[] m_heightMap;
        delete[] m_oreMap;
    }
} }
