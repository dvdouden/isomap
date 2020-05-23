#pragma once

#include "../types.h"
#include "Controller.h"

namespace isomap {
    namespace client {
        namespace unit {

            class HarvesterController : public Controller {
            public:
                explicit HarvesterController( Unit* unit );

                ~HarvesterController() override;

                HarvesterController( const HarvesterController& ) = delete;

                HarvesterController& operator=( const HarvesterController& ) = delete;

                bool harvest() override;

                bool unload() override;

                void onMessage( common::UnitServerMessage::Type msgType ) override;

                void update() override;

                void dump() override;

            private:
                void onDone();

                void onAbort();

                bool m_harvesting = true;

            };
        }
    }
}
