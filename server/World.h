#pragma once

#include "types.h"

namespace isomap { namespace server {

    class World {
    public:
        World( uint32_t width, uint32_t height );
        ~World();

        World( const World& ) = delete;
        const World& operator=( const World& ) = delete;

        uint32_t width() const {
            return m_width;
        }

        uint32_t height() const {
            return m_height;
        }

    private:
        uint32_t m_width;
        uint32_t m_height;

        uint8_t* m_heightMap = nullptr;
        uint8_t* m_oreMap = nullptr;
    };
} }
