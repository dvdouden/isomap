#pragma once

#include <map>
#include <set>
#include <vector>

#include "../../common/types.h"

namespace isomap {
    namespace client {
        namespace player {

            class ConstructionController {
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

                void dump() const;

            private:
                typedef std::map<Structure*, std::set<Unit*>> AssignementMap;

                AssignementMap::iterator unassign( AssignementMap::iterator it, Unit* unit );

                std::vector<Unit*> m_constructors;
                std::set<Structure*> m_structures;
                AssignementMap m_constructorAssignments;
            };

        }
    }
}
