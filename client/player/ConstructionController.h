#pragma once

#include <map>
#include <set>
#include <vector>

#include "../types.h"
#include "../unit/WorkProvider.h"

namespace isomap {
    namespace client {
        namespace player {

            class ConstructionController : public unit::WorkProvider {
            public:
                ConstructionController();

                ~ConstructionController();

                ConstructionController( const ConstructionController& ) = delete;

                ConstructionController& operator=( const ConstructionController& ) = delete;

                void update();

                void addConstructor( Unit* unit );

                void removeConstructor( Unit* unit );

                void addStructure( Structure* structure );

                void removeStructure( Structure* structure );

                void unableToConstruct( Structure* structure, Unit* unit );

                void unitAvailable( Unit* unit ) override;

                void unitUnavailable( Unit* unit ) override;

                void dump() const;

            private:
                void findStructureForUnit( Unit* unit );

                bool findUnitForStructure( Structure* structure );

                static uint32_t distance( Unit* unit, Structure* structure );

                std::set<Unit*> m_constructors;
                std::set<Unit*> m_availableConstructors;
                std::set<Structure*> m_structures;
            };

        }
    }
}
