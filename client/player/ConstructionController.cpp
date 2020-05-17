#include <cstdio>

#include "ConstructionController.h"
#include "../Structure.h"
#include "../Unit.h"

namespace isomap {
    namespace client {
        namespace player {
            ConstructionController::ConstructionController() = default;

            ConstructionController::~ConstructionController() {
                for ( auto* unit : m_constructors ) {
                    unit->controller()->setWorkProvider( nullptr );
                }
            }

            void ConstructionController::update() {
                if ( !m_structures.empty() && !m_availableConstructors.empty() ) {
                    // first put all structures in a new container so we can safely iterate over them

                    // FIXME: we really don't want to perform these checks every single frame
                    // we should be able to set a timer, like "don't check this structure for the next X frames"
                    std::vector<Structure*> structures( m_structures.begin(), m_structures.end() );
                    for ( auto* structure : structures ) {
                        findUnitForStructure( structure );
                    }
                }
            }

            void ConstructionController::addConstructor( Unit* unit ) {
                printf( "CC Add unit [%d]\n", unit->id() );
                m_constructors.insert( unit );
                unit->controller()->setWorkProvider( this );
                // FIXME: ASSUMPTIONS! (what if the unit is not available?)
                unitAvailable( unit );
            }

            void ConstructionController::removeConstructor( Unit* unit ) {
                printf( "CC Remove unit [%d]\n", unit->id() );
                m_constructors.erase( unit );
                m_availableConstructors.erase( unit );
                unit->controller()->setWorkProvider( nullptr );
            }

            void ConstructionController::addStructure( Structure* structure ) {
                printf( "CC Add structure [%d]\n", structure->id() );
                m_structures.insert( structure );
                findUnitForStructure( structure );
            }

            void ConstructionController::removeStructure( Structure* structure ) {
                printf( "CC remove structure [%d]\n", structure->id() );
                m_structures.erase( structure );
            }

            void ConstructionController::unitAvailable( Unit* unit ) {
                printf( "CC Unit available [%d]\n", unit->id() );
                m_availableConstructors.insert( unit );
                findStructureForUnit( unit );
            }

            void ConstructionController::unitUnavailable( Unit* unit ) {
                printf( "CC Unit unavailable [%d]\n", unit->id() );
                m_availableConstructors.erase( unit );
            }

            void ConstructionController::unableToConstruct( Structure* structure, Unit* unit ) {
                printf( "CC Unit [%d] unable to construct [%d]\n", unit->id(), structure->id() );
                addStructure( structure );
            }

            void ConstructionController::dump() const {
                printf( "ConstructionController:\n" );
                printf( "Registered structures: %d\n", m_structures.size() );
                printf( "Registered constructors: %d\n", m_constructors.size() );
                printf( "Available constructors: %d\n", m_availableConstructors.size() );
            }

            void ConstructionController::findStructureForUnit( Unit* unit ) {
                // early abort check
                if ( m_structures.empty() ) {
                    return;
                }

                // first sort all structures based on distance to unit
                struct distanceCompare {
                    Unit* unit;

                    explicit distanceCompare( Unit* unit ) :
                            unit( unit ) { }

                    bool operator()( Structure* a, Structure* b ) const {
                        return distance( unit, a ) > distance( unit, b );
                    }
                };
                std::priority_queue<Structure*, std::vector<Structure*>, distanceCompare> structures(
                        m_structures.begin(),
                        m_structures.end(),
                        distanceCompare( unit ) );

                // try each structure, nearest one first
                while ( !structures.empty() ) {
                    auto* structure = structures.top();
                    if ( unit->controller()->construct( structure ) ) {
                        m_structures.erase( structure );
                        break;
                    } else {
                        structures.pop();
                    }
                }
            }

            bool ConstructionController::findUnitForStructure( Structure* structure ) {
                // early abort check
                if ( m_availableConstructors.empty() ) {
                    return false;
                }

                // first sort all units based on distance to structure
                struct distanceCompare {
                    Structure* structure;

                    explicit distanceCompare( Structure* structure ) :
                            structure( structure ) { }

                    bool operator()( Unit* a, Unit* b ) const {
                        return distance( a, structure ) > distance( b, structure );
                    }
                };
                std::priority_queue<Unit*, std::vector<Unit*>, distanceCompare> units(
                        m_availableConstructors.begin(),
                        m_availableConstructors.end(),
                        distanceCompare( structure ) );

                // try each unit, nearest one first
                while ( !units.empty() ) {
                    auto* unit = units.top();
                    if ( unit->controller()->construct( structure ) ) {
                        m_structures.erase( structure );
                        return true;
                    } else {
                        units.pop();
                    }
                }
                return false;
            }

            uint32_t ConstructionController::distance( Unit* unit, Structure* structure ) {
                int32_t dX = unit->tileX() - structure->x();
                int32_t dY = unit->tileY() - structure->y();
                return dX * dX + dY * dY;
            }

        }
    }
}
