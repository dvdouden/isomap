#pragma once

#include "../types.h"

namespace isomap {
    namespace client {
        namespace unit {

            class WorkProvider {
            public:
                virtual void unitAvailable( Unit* unit ) = 0;

                virtual void unitUnavailable( Unit* unit ) = 0;
            };
        }

    }
}
