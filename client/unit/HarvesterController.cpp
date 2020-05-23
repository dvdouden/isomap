#include "HarvesterController.h"
#include "../Player.h"
#include "../Structure.h"
#include "../Unit.h"
#include "DockingTilePathCondition.h"
#include "OreTilePathCondition.h"

namespace isomap {
    namespace client {
        namespace unit {

            HarvesterController::HarvesterController( Unit* unit ) :
                    Controller( unit ) {

            }

            HarvesterController::~HarvesterController() = default;

            bool HarvesterController::harvest() {
                m_harvesting = true;
                if ( Controller::harvest() ) {
                    return true;
                }
                return Controller::moveTo( OreTilePathCondition( unit()->player()->terrain()->oreMap() ) );
            }

            bool HarvesterController::unload() {
                m_harvesting = false;
                Structure* refinery = assignedStructure();
                if ( refinery != nullptr ) {
                    // check if we're on the docking tile
                    if ( refinery->dockingTileAt( unit()->tileX(), unit()->tileY() ) ) {
                        // if so, start unloading
                        return Controller::unload();
                    } else {
                        // if not, move to the refinery
                        return Controller::moveTo( StructureDockingTilePathCondition( refinery ) );
                    }
                }
                // no assigned refinery, check if we're on one by chance...
                if ( Controller::unload() ) {
                    return true;
                }

                return Controller::moveTo(
                        StructureTypeDockingTilePathCondition(
                                unit()->type()->dockStructureType(),
                                unit()->player()->terrain() ) );
            }

            void HarvesterController::onMessage( common::UnitServerMessage::Type msgType ) {
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

            void HarvesterController::onDone() {
                switch ( unit()->lastState() ) {
                    case common::Moving: // reached target
                        if ( m_harvesting ) {
                            printf( "Reached target, start harvesting\n" );
                            harvest();
                        } else {
                            printf( "Reached target, start unloading\n" );
                            unload();
                        }
                        break;

                    case common::Harvesting:
                        printf( "Harvesting complete\n" );
                        if ( unit()->payloadFull() ) {
                            printf( "start unloading\n" );
                            unload();
                        } else {
                            printf( "move to next ore tile!\n" );
                            if ( !harvest() ) {
                                printf( "None found, unload!\n" );
                                unload();
                            }
                        }
                        break;

                    case common::Unloading:
                        harvest();
                        break;

                    default:
                        // ignore
                        break;
                }
            }

            void HarvesterController::onAbort() {
                switch ( unit()->lastState() ) {
                    case common::Moving:
                        printf( "[%d] Failed to reach target ore tile, retry!\n",
                                unit()->id() );
                        if ( m_harvesting ) {
                            if ( !harvest() ) {
                                printf( "[%d] Failed again, unload!\n", unit()->id() );
                                unload();
                            }
                        } else {
                            printf( "[%d] Failed to reach refinery for unload, retry!\n",
                                    unit()->id() );
                            unload();
                        }
                        break;

                    case common::Harvesting:
                        printf( "[%d] Failed to harvest, retry!\n", unit()->id() );
                        if ( !harvest() ) {
                            printf( "[%d] Failed again, unload!\n", unit()->id() );
                            unload();
                        }
                        break;

                    case common::Unloading:
                        // failed to construct target? Retry
                        printf( "[%d] Failed to unload, retry!\n", unit()->id() );
                        unload();
                        break;

                    default:
                        // ignore
                        break;
                }
            }

            void HarvesterController::update() {
                Controller::update();
            }


            void HarvesterController::dump() {
                Controller::dump();
                printf( "Harvesting: %s\n", m_harvesting ? "yes" : "no" );
            }

        }
    }
}