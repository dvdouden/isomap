#pragma once

#include "types.h"

namespace isomap { namespace server {
    class Player {
    public:
        Player();
        ~Player();

        Player( const Player& ) = delete;
        const Player& operator=( const Player& ) = delete;

    private:
        uint8_t* m_fogMap = nullptr;
        uint8_t* m_fowMap = nullptr;
    };

} }