#include "UnitData.h"
#include "../util/math.h"
#include "TerrainData.h"

namespace isomap {
    namespace common {
        bool UnitData::updateMotion( TerrainData& terrain ) {
            if ( state == Idle ) {
                return true;
            }
            int32_t dX = wayPoint.x - x;
            int32_t dY = wayPoint.y - y;

            orientation = getOrientation( dX, dY );
            uint32_t oldTileX = tileX();
            uint32_t oldTileY = tileY();
            if ( onCenterOfTile() ) {
                // about to move to the next tile

                // first check if there's a path to the next tile from the current tile
                if ( !terrain.hasPath( tileX(), tileY(), orientation ) ) {
                    return false;
                }
                // secondly check if tile is actually available
                int32_t mX, mY;
                getMotion( mX, mY, orientation, 1 );
                if ( terrain.impassable( tileX() + mX, tileY() + mY ) ) {
                    return false;
                }
                terrain.reserveUnit( tileX() + mX, tileY() + mY );
            }

            getMotion( dX, dY, orientation, math::fix::precision / 16 );
            x += dX;
            y += dY;

            if ( x < 0 ) {
                x = 0;
            } else if ( x >= terrain.mapWidth * math::fix::precision ) {
                x = (terrain.mapWidth - 1) * math::fix::precision;
            }
            if ( y < 0 ) {
                y = 0;
            } else if ( y >= terrain.mapHeight * math::fix::precision ) {
                y = (terrain.mapHeight - 1) * math::fix::precision;
            }

            if ( tileX() != oldTileX || tileY() != oldTileY ) {
                terrain.unreserveUnit( oldTileX, oldTileY );
            }

            z = terrain.heightAt( x, y );

            if ( x == wayPoint.x && y == wayPoint.y ) {
                setState( common::Idle );
            }
            return true;
        }

        uint32_t UnitData::getOrientation( int32_t dX, int32_t dY ) {
            if ( dY > 0 ) {
                if ( dX > 0 ) {
                    return 1;
                } else if ( dX == 0 ) {
                    return 0;
                } else {
                    return 7;
                }
            } else if ( dY == 0 ) {
                if ( dX > 0 ) {
                    return 2;
                } else if ( dX == 0 ) {
                    return 8; // yeah, I don't know either...
                } else {
                    return 6;
                }
            } else {
                if ( dX > 0 ) {
                    return 3;
                } else if ( dX == 0 ) {
                    return 4;
                } else {
                    return 5;
                }
            }
        }

        void UnitData::getMotion( int32_t& dX, int32_t& dY, uint32_t orientation, int32_t speed ) {
            switch ( orientation ) {
                case 0:
                    dX = 0;
                    dY = speed;
                    break;
                case 1:
                    dX = speed;
                    dY = speed;
                    break;
                case 2:
                    dX = speed;
                    dY = 0;
                    break;
                case 3:
                    dX = speed;
                    dY = -speed;
                    break;
                case 4:
                    dX = 0;
                    dY = -speed;
                    break;
                case 5:
                    dX = -speed;
                    dY = -speed;
                    break;
                case 6:
                    dX = -speed;
                    dY = 0;
                    break;
                case 7:
                    dX = -speed;
                    dY = speed;
                    break;
                default:
                    dX = 0;
                    dY = 0;
                    break;
            }
        }

        const char* stateToString( UnitState state ) {
            switch ( state ) {
                case Idle:
                    return "idle";
                case Moving:
                    return "moving";
                case Constructing:
                    return "constructing";
                case Harvesting:
                    return "harvesting";
                case Unloading:
                    return "unloading";
                case Loading:
                    return "loading";
            }
            return "unknown";
        }

        const char* UnitData::stateName() const {
            return stateToString( state );
        }

        const char* UnitData::lastStateName() const {
            return stateToString( lastState );
        }
    }
}
