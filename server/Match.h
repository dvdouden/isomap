#pragma once

#include <map>

#include "types.h"

namespace isomap { namespace server {
    class Match {
    public:
        Match();
        ~Match();

        Match( const Match& ) = delete;
        const Match& operator=( const Match& ) = delete;

    private:
        World* m_world = nullptr;
        std::map<id_t, Object*> m_objects;
    };

} }
