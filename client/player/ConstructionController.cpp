#include <cstdio>

#include "ConstructionController.h"
#include "../Unit.h"

namespace isomap {
    namespace client {
        namespace player {
            ConstructionController::ConstructionController() = default;

            ConstructionController::~ConstructionController() = default;

            void ConstructionController::update() {
                for ( auto it = m_structures.begin(); it != m_structures.end(); ) {
                    Structure* structure = *it;
                    struct {
                        bool operator()( Unit* a, Unit* b ) const {
                            return a->controller()->weight() < b->controller()->weight();
                        }
                    } weightLess;
                    std::sort( m_constructors.begin(), m_constructors.end(), weightLess );

                    for ( auto* unit : m_constructors ) {
                        if ( unit->controller()->construct( structure ) ) {
                            m_constructorAssignments[structure].insert( unit );
                            break;
                        }
                    }
                    if ( m_constructorAssignments.find( structure ) != m_constructorAssignments.end() ) {
                        it = m_structures.erase( it );
                    } else {
                        ++it;
                    }
                }
            }

            void ConstructionController::addConstructor( Unit* unit ) {
                m_constructors.push_back( unit );
            }

            void ConstructionController::removeConstructor( Unit* unit ) {
                auto it = std::find( m_constructors.begin(), m_constructors.end(), unit );
                if ( it != m_constructors.end() ) {
                    m_constructors.erase( it );
                }

                // remove all assignments
                for ( auto it = m_constructorAssignments.begin(); it != m_constructorAssignments.end(); ) {
                    it = unassign( it, unit );
                }
            }

            void ConstructionController::addStructure( Structure* structure ) {
                m_structures.insert( structure );
            }

            void ConstructionController::removeStructure( Structure* structure ) {
                m_structures.erase( structure );
                // TODO: notify constructors that unit is goneeee
                m_constructorAssignments.erase( structure );
            }

            void ConstructionController::dump() const {
                printf( "ConstructionController:\n" );
                printf( "Registered structures: %d\n", m_structures.size() );
                printf( "Registered constructors: %d\n", m_constructors.size() );
            }

            void ConstructionController::unableToConstruct( Structure* structure, Unit* unit ) {
                auto it = m_constructorAssignments.find( structure );
                if ( it != m_constructorAssignments.end() ) {
                    unassign( it, unit );
                }
            }

            ConstructionController::AssignementMap::iterator
            ConstructionController::unassign( AssignementMap::iterator it, Unit* unit ) {
                // remove unit from assignment list
                it->second.erase( unit );
                // check if structure has any assigned constructors left
                if ( it->second.empty() ) {
                    // if not, put it back in the to-do list
                    m_structures.insert( it->first );
                    return m_constructorAssignments.erase( it );
                }
                return it++;
            }

        }
    }
}
