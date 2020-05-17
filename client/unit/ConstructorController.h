#pragma once

#include <queue>

#include "../../common/types.h"
#include "Controller.h"

namespace isomap {
    namespace client {
        namespace unit {

            class ConstructorController : public Controller {
            public:
                explicit ConstructorController( Unit* unit );

                ~ConstructorController() override;

                ConstructorController( const ConstructorController& ) = delete;

                ConstructorController& operator=( const ConstructorController& ) = delete;

                bool construct( Structure* structure ) override;

                void onMessage( common::UnitServerMessage::Type msgType ) override;

                void update() override;

                int32_t weight() const override;

                void dump() override;

            private:
                void onDone();

                void onAbort();

                void moveTo();

                void construct();

                void fail();

                Structure* m_currentStructure = nullptr;
                id_t m_currentStructureId = 0;
                std::queue<id_t> m_structureQueue;
            };
        }
    }
}
