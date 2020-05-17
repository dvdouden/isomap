#pragma once

#include "../../common/types.h"
#include "../Structure.h"

namespace isomap {
    namespace client {
        namespace structure {

            class Controller {
            public:
                explicit Controller( Structure* structure );

                virtual ~Controller();

                Controller( const Controller& ) = delete;

                Controller& operator=( const Controller& ) = delete;

                virtual void update();

                virtual void onMessage( common::StructureServerMessage::Type msgType );

                virtual void dump();

                Structure* structure() const {
                    return m_structure;
                }

            private:
                Structure* m_structure;
            };

        }
    }
}


