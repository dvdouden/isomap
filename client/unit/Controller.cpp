#include <vlCore/Colors.hpp>

#include "Controller.h"
#include "PathCondition.h"
#include "../player/Controller.h"
#include "../Structure.h"
#include "../Player.h"
#include "../Terrain.h"
#include "WayPointRetryPathCondition.h"

namespace isomap {
    namespace client {
        namespace unit {

            Controller::Controller( Unit* unit ) :
                    m_unit( unit ) {

            }

            Controller::~Controller() = default;

            bool Controller::construct( Structure* structure ) {
                //printf( "Unit %d construct %d\n", id(), structure->id() );
                if ( !m_unit->type()->canConstruct() ) {
                    printf( "[%d] Construct command given to unit without construction abilities!\n",
                            m_unit->id() );
                    return false;
                }
                if ( structure->player() != m_unit->player() ) {
                    printf( "[%d] Construct command given to unit for structure %d of other player!\n",
                            m_unit->id(),
                            structure->id() );
                    return false;
                }
                if ( structure->constructionCompleted() ) {
                    printf( "[%d] Construct command given to unit for completed structure %d!\n",
                            m_unit->id(),
                            structure->id() );
                    return false;
                }
                if ( !m_unit->isAdjacentTo( structure ) ) {
                    printf( "[%d] Construct command given to unit not adjacent to structure %d (%d.%d, %d.%d)!\n",
                            m_unit->id(),
                            structure->id(),
                            m_unit->tileX(), m_unit->subTileX(),
                            m_unit->tileY(), m_unit->subTileY() );
                    return false;
                }
                m_unit->player()->controller()->enqueueMessage( m_unit->id(),
                                                                common::UnitCommandMessage::constructMsg(
                                                                        structure->id() ) );
                return true;
            }

            bool Controller::harvest() {
                //printf( "Unit %d harvest\n", id() );
                if ( !m_unit->type()->canHarvest() ) {
                    printf( "[%d] Harvest command given to unit without harvesting abilities!\n",
                            m_unit->id() );
                    return false;
                }
                if ( m_unit->payload() >= m_unit->type()->maxPayload() ) {
                    printf( "[%d] Harvest command given to unit with full payload!\n",
                            m_unit->id() );
                    return false;
                }
                if ( m_unit->player()->terrain()->ore( m_unit->tileX(), m_unit->tileY() ) == 0 ) {
                    printf( "[%d] Harvest command given to unit but no ore at tile!\n",
                            m_unit->id() );
                    return false;
                }

                m_unit->player()->controller()->enqueueMessage( m_unit->id(),
                                                                common::UnitCommandMessage::harvestMsg() );
                return true;
            }

            bool Controller::unload() {
                //printf( "Unit %d unload\n", id() );
                if ( m_unit->type()->maxPayload() == 0 ) {
                    printf( "[%d] Unload command given to unit without payload!\n",
                            m_unit->id() );
                    return false;
                }
                auto* structure = m_unit->player()->terrain()->getConstructedStructureAt( m_unit->tileX(),
                                                                                          m_unit->tileY() );
                if ( structure == nullptr ) {
                    return false;
                }
                if ( structure->type()->id() != m_unit->type()->dockStructureType() ) {
                    return false;
                }
                if ( !structure->dockingTileAt( m_unit->tileX(), m_unit->tileY() ) ) {
                    return false;
                }

                m_unit->player()->controller()->enqueueMessage( m_unit->id(),
                                                                common::UnitCommandMessage::unloadMsg() );
                return true;
            }

            bool Controller::load() {
                //printf( "Unit %d load\n", id() );
                if ( m_unit->type()->maxPayload() == 0 ) {
                    printf( "[%d] Load command given to unit without payload!\n",
                            m_unit->id() );
                    return false;
                }
                auto* structure = m_unit->player()->terrain()->getConstructedStructureAt( m_unit->tileX(),
                                                                                          m_unit->tileY() );
                if ( structure == nullptr ) {
                    return false;
                }
                if ( structure->type()->id() != m_unit->type()->dockStructureType() ) {
                    return false;
                }
                if ( !structure->dockingTileAt( m_unit->tileX(), m_unit->tileY() ) ) {
                    return false;
                }

                m_unit->player()->controller()->enqueueMessage( m_unit->id(),
                                                                common::UnitCommandMessage::loadMsg() );
                return true;
            }

            void Controller::dump() {
                printf( "Controller:\n" );
                printf( "\tWaypoints: %lu\n", m_wayPoints.size() );
            }

            void Controller::onMessage( common::UnitServerMessage::Type msgType ) {
                switch ( msgType ) {
                    case common::UnitServerMessage::Construct:
                    case common::UnitServerMessage::Harvest:
                    case common::UnitServerMessage::Status:
                    case common::UnitServerMessage::Stop:
                    case common::UnitServerMessage::Unload:
                    case common::UnitServerMessage::Load:
                        break;
                    case common::UnitServerMessage::MoveTo:
                        onMove();
                        break;
                    case common::UnitServerMessage::Done:
                        onDone();
                        break;
                    case common::UnitServerMessage::Abort:
                        onAbort();
                        break;
                }
            }

            void Controller::update() {
                switch ( m_unit->state() ) {
                    case common::Harvesting:
                        if ( !m_unit->payloadFull() ) {
                            m_unit->data().payload++;
                        }
                        break;

                    case common::Unloading:
                        // FIXME: depends on structure type
                        m_unit->data().payload -= m_unit->player()->incCredits( 1 );
                        break;

                    case common::Loading:
                        // FIXME: depends on structure type
                        m_unit->data().payload += m_unit->player()->decCredits( 1 );
                        break;

                    default:
                        break;
                }

                for ( auto& wayPoint : m_wayPoints ) {
                    m_unit->player()->terrain()->renderer()->addHighlight( wayPoint.x, wayPoint.y, vl::fuchsia );
                }
            }

            bool Controller::moveTo( const PathCondition& pathCondition ) {
                m_wayPoints = findPath( pathCondition );
                if ( !m_wayPoints.empty() ) {
                    m_unit->player()->controller()->enqueueMessage( m_unit->id(),
                                                                    common::UnitCommandMessage::moveMsg(
                                                                            m_wayPoints ) );
                    return true;
                }
                return false;
            }

            Structure* Controller::assignedStructure() {
                if ( m_assignedStructureId == 0 ) {
                    return nullptr;
                }
                Structure* structure = m_unit->player()->getStructure( m_assignedStructureId );
                if ( structure == nullptr ) {
                    m_assignedStructureId = 0;
                }
                return structure;
            }

            void Controller::onMove() {
                updateWayPoints();
            }

            void Controller::onDone() {
                if ( unit()->lastState() == common::Moving ) {
                    updateWayPoints();
                }
            }

            void Controller::onAbort() {
                if ( unit()->lastState() == common::Moving ) {
                    printf( "Unit stopped moving at %d, %d while moving to %d, %d\n",
                            unit()->data().x, unit()->data().y, unit()->data().wayPoint.x, unit()->data().wayPoint.y );
                    if ( !unit()->player()->terrain()->data().hasPath( unit()->tileX(), unit()->tileY(),
                                                                       unit()->orientation() ) ) {
                        printf( "Due to landscape or structure change!\n" );
                    } else if ( unit()->player()->terrain()->data().reservedByUnit( unit()->tileX(),
                                                                                    unit()->tileY() ) ) {
                        printf( "Reserved by unit\n" );
                    }
                    std::vector<common::WayPoint> wayPoints = findAlternativePath( m_wayPoints );
                    if ( !wayPoints.empty() ) {
                        m_wayPoints = wayPoints;
                        m_unit->player()->controller()->enqueueMessage( m_unit->id(),
                                                                        common::UnitCommandMessage::moveMsg(
                                                                                m_wayPoints ) );
                    }
                }
            }

            void Controller::updateWayPoints() {
                if ( !m_wayPoints.empty() &&
                     m_wayPoints.back().x == m_unit->tileX() &&
                     m_wayPoints.back().y == m_unit->tileY() ) {
                    m_wayPoints.pop_back();
                }
            }

            std::vector<common::WayPoint> Controller::findPath( const PathCondition& pathCondition ) const {
                auto* terrain = m_unit->player()->terrain();

                uint32_t width = terrain->width();
                uint32_t height = terrain->height();

                // create buffer for A* algorithm
                struct node {
                    uint32_t value = 0;
                    uint32_t from = 0;

                    bool operator>( const node& rhs ) const {
                        return value > rhs.value;
                    }
                };
                std::vector<node> nodeMap( width * height );

                std::priority_queue<node, std::vector<node>, std::greater<>> todo;

                uint32_t startIdx = m_unit->tileY() * width + m_unit->tileX();
                uint32_t targetIdx = 0;

                todo.push( {1, startIdx} );
                bool found = false;
                while ( !todo.empty() ) {
                    node tile = todo.top();
                    todo.pop();
                    node value = nodeMap[tile.from];
                    if ( pathCondition.hasReached( tile.from ) ) {
                        //printf( "Found a route!\n" );
                        found = true;
                        targetIdx = tile.from;
                        break;
                    }
                    uint8_t canReach = terrain->pathMap()[tile.from];
                    //printf( "[%2d,%2d] %02X\n", tile_x, tile_y, canReach );
                    uint8_t slopeBits = terrain->slopeMap()[tile.from] % 16u;

                    // for now we're going to move in every direction, as long as we haven't traveled there yet.
                    if ( canReach & common::path::bitDownLeft ) {
                        //printf( "down left\n" );
                        uint32_t idx = tile.from - width - 1;
                        if ( nodeMap[idx].value == 0 && pathCondition.canReach( tile.from, idx ) ) {
                            nodeMap[idx].value = value.value + 15 + common::slopeAngle( slopeBits, 5 );
                            nodeMap[idx].from = tile.from;
                            if ( nodeMap[idx].value < pathCondition.maxValue() ) {
                                todo.push( {nodeMap[idx].value, idx} );
                            }
                        }
                    }
                    if ( canReach & common::path::bitDown ) {
                        //printf( "down\n" );
                        uint32_t idx = tile.from - width;
                        if ( nodeMap[idx].value == 0 && pathCondition.canReach( tile.from, idx ) ) {
                            nodeMap[idx].value = value.value + 10 + common::slopeAngle( slopeBits, 4 );
                            nodeMap[idx].from = tile.from;
                            if ( nodeMap[idx].value < pathCondition.maxValue() ) {
                                todo.push( {nodeMap[idx].value, idx} );
                            }
                        }
                    }
                    if ( canReach & common::path::bitDownRight ) {
                        //printf( "down right\n" );
                        uint32_t idx = tile.from - width + 1;
                        if ( nodeMap[idx].value == 0 && pathCondition.canReach( tile.from, idx ) ) {
                            nodeMap[idx].value = value.value + 15 + common::slopeAngle( slopeBits, 3 );
                            nodeMap[idx].from = tile.from;
                            if ( nodeMap[idx].value < pathCondition.maxValue() ) {
                                todo.push( {nodeMap[idx].value, idx} );
                            }
                        }
                    }
                    if ( canReach & common::path::bitRight ) {
                        //printf( "right\n" );
                        uint32_t idx = tile.from + 1;
                        if ( nodeMap[idx].value == 0 && pathCondition.canReach( tile.from, idx ) ) {
                            nodeMap[idx].value = value.value + 10 + common::slopeAngle( slopeBits, 2 );
                            nodeMap[idx].from = tile.from;
                            if ( nodeMap[idx].value < pathCondition.maxValue() ) {
                                todo.push( {nodeMap[idx].value, idx} );
                            }
                        }
                    }
                    if ( canReach & common::path::bitUpRight ) {
                        //printf( "right up\n" );
                        uint32_t idx = tile.from + width + 1;
                        if ( nodeMap[idx].value == 0 && pathCondition.canReach( tile.from, idx ) ) {
                            nodeMap[idx].value = value.value + 15 + common::slopeAngle( slopeBits, 1 );
                            nodeMap[idx].from = tile.from;
                            if ( nodeMap[idx].value < pathCondition.maxValue() ) {
                                todo.push( {nodeMap[idx].value, idx} );
                            }
                        }
                    }
                    if ( canReach & common::path::bitUp ) {
                        //printf( "up\n" );
                        uint32_t idx = tile.from + width;
                        if ( nodeMap[idx].value == 0 && pathCondition.canReach( tile.from, idx ) ) {
                            nodeMap[idx].value = value.value + 10 + common::slopeAngle( slopeBits, 0 );
                            nodeMap[idx].from = tile.from;
                            if ( nodeMap[idx].value < pathCondition.maxValue() ) {
                                todo.push( {nodeMap[idx].value, idx} );
                            }
                        }
                    }
                    if ( canReach & common::path::bitUpLeft ) {
                        //printf( "up left\n" );
                        uint32_t idx = tile.from + width - 1;
                        if ( nodeMap[idx].value == 0 && pathCondition.canReach( tile.from, idx ) ) {
                            nodeMap[idx].value = value.value + 15 + common::slopeAngle( slopeBits, 7 );
                            nodeMap[idx].from = tile.from;
                            if ( nodeMap[idx].value < pathCondition.maxValue() ) {
                                todo.push( {nodeMap[idx].value, idx} );
                            }
                        }
                    }
                    if ( canReach & common::path::bitLeft ) {
                        //printf( "left\n" );
                        uint32_t idx = tile.from - 1;
                        if ( nodeMap[idx].value == 0 && pathCondition.canReach( tile.from, idx ) ) {
                            nodeMap[idx].value = value.value + 10 + common::slopeAngle( slopeBits, 6 );
                            nodeMap[idx].from = tile.from;
                            if ( nodeMap[idx].value < pathCondition.maxValue() ) {
                                todo.push( {nodeMap[idx].value, idx} );
                            }
                        }
                    }
                }

                if ( !found ) {
                    printf( "No route!\n" );
                    return std::vector<common::WayPoint>();
                }
                printf( "Found a route!\n" );
                std::vector<common::WayPoint> wayPoints;
                uint8_t lastOrientation = 8;
                while ( targetIdx != startIdx ) {
                    auto wayPointX = (int32_t)(targetIdx % width);
                    auto wayPointY = (int32_t)(targetIdx / width);
                    //printf( "[%d]: %d %d\n", nodeMap[targetIdx].value, wayPointX, wayPointY );
                    targetIdx = nodeMap[targetIdx].from;

                    auto tile_x = (int32_t)(targetIdx % width);
                    auto tile_y = (int32_t)(targetIdx / width);
                    uint8_t orientation = common::UnitData::getOrientation( wayPointX - tile_x,
                                                                            wayPointY - tile_y );
                    if ( wayPoints.empty() || orientation != lastOrientation ) {
                        wayPoints.push_back( {wayPointX, wayPointY} );
                        lastOrientation = orientation;
                    }
                }
                return wayPoints;
            }

            std::vector<common::WayPoint> Controller::findAlternativePath( const common::WayPoint& wayPoint, const common::WayPoint& prevWayPoint ) const {
                uint32_t orientation = common::UnitData::getOrientation( wayPoint.x - prevWayPoint.x, wayPoint.y - prevWayPoint.y );
                int32_t dX, dY;
                common::UnitData::getMotion( dX, dY, orientation, 1 );

                int32_t x = prevWayPoint.x + dX;
                int32_t y = prevWayPoint.y + dY;

                for ( ; ; ) {
                    common::WayPoint w = { x, y };
                    std::vector<common::WayPoint> result = findPath(
                            WayPointRetryPathCondition(
                                    w,
                                    unit()->player()->terrain()->data(),
                                    300 ) );
                    if ( !result.empty() ) {
                        return result;
                    }
                    if ( w == wayPoint ) {
                        return std::vector<common::WayPoint>();
                    }
                    x += dX;
                    y += dY;
                }
            }

            std::vector<common::WayPoint>
            Controller::findAlternativePath( const std::vector<common::WayPoint>& wayPoints ) const {
                std::vector<common::WayPoint> oldRoute = wayPoints;
                common::WayPoint wp { m_unit->tileX(), m_unit->tileY() };

                while ( !oldRoute.empty() ) {
                    std::vector<common::WayPoint> newRoute = findAlternativePath( oldRoute.back(), wp );
                    if ( !newRoute.empty() ) {
                        if ( newRoute.front() == oldRoute.back() ) {
                            oldRoute.pop_back();
                        }
                        oldRoute.insert( oldRoute.end(), newRoute.begin(), newRoute.end() );
                        return oldRoute;
                    }
                    wp = oldRoute.back();
                    oldRoute.pop_back();
                }
                return std::vector<common::WayPoint>();
            }

        }
    }
}