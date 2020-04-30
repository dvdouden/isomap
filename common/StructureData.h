#pragma once

#include "types.h"

namespace isomap {
    namespace common {

        // data shared between client and server structure objects
        struct StructureData {
            id_t id = 0;
            id_t typeId = 0;
            uint32_t x = 0;
            uint32_t y = 0;
            uint32_t z = 0;
            uint32_t orientation = 0;
            uint32_t constructionProgress = 0;
        };

    }
}

