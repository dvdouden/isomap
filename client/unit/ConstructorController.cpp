#include "ConstructorController.h"
#include "../Player.h"
#include "../Structure.h"
#include "../Unit.h"
#include "WorkProvider.h"
#include "AdjacentToStructurePathCondition.h"
#include "DockingTilePathCondition.h"

namespace isomap {
    namespace client {
        namespace unit {

            ConstructorController::ConstructorController( Unit* unit ) :
                    Controller( unit ) {

            }

            ConstructorController::~ConstructorController() = default;

            bool ConstructorController::construct( Structure* structure ) {
                if ( unit()->payloadEmpty() ) {
                    return false;
                }
                if ( unit()->isAdjacentTo( structure ) ) {
                    if ( Controller::construct( structure ) ) {
                        setStructure( structure );
                        return true;
                    }
                } else {
                    if ( Controller::moveTo( AdjacentToStructurePathCondition( structure ) ) ) {
                        setStructure( structure );
                        return true;
                    }
                }
                return false;
            }

            bool ConstructorController::load() {
                m_constructing = false;
                Structure* constructionYard = assignedStructure();
                if ( constructionYard != nullptr ) {
                    // check if we're on the docking tile
                    if ( constructionYard->dockingTileAt( unit()->tileX(), unit()->tileY() ) ) {
                        // if so, start unloading
                        return Controller::unload();
                    } else {
                        // if not, move to the construction yard
                        return Controller::moveTo( StructureDockingTilePathCondition( constructionYard ) );
                    }
                }
                // no assigned construction yard, check if we're on one by chance...
                if ( Controller::load() ) {
                    return true;
                }

                return Controller::moveTo(
                        StructureTypeDockingTilePathCondition(
                                unit()->type()->dockStructureType(),
                                unit()->player()->terrain() ) );
            }

            void ConstructorController::onMessage( common::UnitServerMessage::Type msgType ) {
                switch ( msgType ) {
                    case common::UnitServerMessage::Construct:
                    case common::UnitServerMessage::Harvest:
                    case common::UnitServerMessage::Stop:
                    case common::UnitServerMessage::Status:
                    case common::UnitServerMessage::MoveTo:
                    case common::UnitServerMessage::Unload:
                    case common::UnitServerMessage::Load:
                        break;

                    case common::UnitServerMessage::Done:
                        onDone();
                        break;

                    case common::UnitServerMessage::Abort:
                        onAbort();
                        break;
                }
                Controller::onMessage( msgType );
            }

            void ConstructorController::onDone() {
                switch ( unit()->lastState() ) {
                    case common::Moving: // reached target
                        //printf( "Reached target, start constructing\n" );
                        if ( m_constructing ) {
                            construct();
                        } else {
                            load();
                        }
                        break;

                    case common::Constructing:
                        //printf( "Construction complete\n" );
                        m_currentStructure = nullptr;
                        m_currentStructureId = 0;
                        if ( unit()->payloadEmpty() ) {
                            load();
                        } else if ( workProvider() != nullptr ) {
                            workProvider()->unitAvailable( unit() );
                        }
                        break;

                    case common::Loading:
                        //printf( "Loading complete\n" );
                        if ( workProvider() != nullptr ) {
                            workProvider()->unitAvailable( unit() );
                        }
                        break;

                    default:
                        // ignore
                        break;
                }
            }

            void ConstructorController::onAbort() {
                switch ( unit()->lastState() ) {
                    case common::Moving:
                        // failed to reach target, retry
                        printf( "[%d] Move command given to unit but unable to reach structure, retry!\n",
                                unit()->id() );
                        moveTo();
                        break;

                    case common::Constructing:
                        // failed to construct target? Retry
                        printf( "[%d] Construct command given to unit but unable to construct structure, retry!\n",
                                unit()->id() );
                        construct();
                        break;

                    case common::Loading:
                        // failed to load resources? Retry
                        printf( "[%d] Load command given to unit but unable to load, retry!\n",
                                unit()->id() );
                        load();
                        break;

                    default:
                        // ignore
                        break;
                }
            }

            void ConstructorController::update() {
                Controller::update();
            }

            void ConstructorController::moveTo() {
                if ( m_constructing ) {
                    m_currentStructure = unit()->player()->getStructure( m_currentStructureId );
                    if ( m_currentStructure == nullptr || !Controller::moveTo(
                            AdjacentToStructurePathCondition( m_currentStructure ) ) ) {
                        printf( "[%d] Construct command given to unit but unable to reach structure %d!\n",
                                unit()->id(),
                                m_currentStructureId );
                        fail();
                    }
                } else {
                    load();
                }
            }

            void ConstructorController::construct() {
                m_currentStructure = unit()->player()->getStructure( m_currentStructureId );
                if ( m_currentStructure == nullptr || !Controller::construct( m_currentStructure ) ) {
                    printf( "[%d] Construct command given to unit but unable to construct structure %d!\n",
                            unit()->id(),
                            m_currentStructureId );
                    fail();
                }
            }

            void ConstructorController::fail() {
                m_currentStructure = nullptr;
                // notify player that we're unable to construct structure
                unit()->player()->controller()->onUnableToConstruct(
                        unit()->player()->getStructure( m_currentStructureId ),
                        unit() );
                if ( unit()->payloadEmpty() ) {
                    load();
                } else if ( workProvider() ) {
                    workProvider()->unitAvailable( unit() );
                }
            }

            void ConstructorController::setStructure( Structure* structure ) {
                if ( m_currentStructure != nullptr ) {
                    unit()->player()->controller()->onUnableToConstruct(
                            unit()->player()->getStructure( m_currentStructureId ),
                            unit() );
                }
                m_constructing = true;
                m_currentStructure = structure;
                m_currentStructureId = m_currentStructure->id();
                if ( workProvider() != nullptr ) {
                    workProvider()->unitUnavailable( unit() );
                }
            }

            void ConstructorController::dump() {
                Controller::dump();
                printf( "Current structure id: %d\n", m_currentStructureId );
                printf( "Current structure: %p\n", m_currentStructure );
                printf( "Current state: %s\n", m_constructing ? "Construction" : "Loading" );
            }

        }
    }
}