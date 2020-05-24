#include "Match.h"
#include "Player.h"
#include "Structure.h"
#include "Terrain.h"
#include "Unit.h"
#include "../util/math.h"
#include "../common/PlayerMessage.h"
#include "../common/UnitMessage.h"

namespace isomap {
    namespace server {

        Unit::Unit( Player* owner, int32_t x, int32_t y, int32_t z, common::UnitType* unitType,
                    uint32_t orientation ) :
                Object( owner ),
                m_data( {id(),
                         unitType->id(),
                         0,
                         x * math::fix::precision + math::fix::halfPrecision,
                         y * math::fix::precision + math::fix::halfPrecision,
                         z * math::fix::precision,
                         orientation} ),
                m_type( unitType ) {
            m_data.payload = unitType->initialPayload();
        }


        void Unit::processMessage( common::UnitCommandMessage* msg ) {
            //printf( "Server Unit %d process client msg of type %s\n", id(), msg->typeName() );
            switch ( msg->type() ) {
                case common::UnitCommandMessage::Move:
                    // FIXME: stop doing what you're doing!
                    m_wayPoints = msg->wayPoints();
                    break;

                case common::UnitCommandMessage::Construct: {
                    if ( !m_type->canConstruct() ) {
                        printf( "[%d] Received Construct command but unit has no construct abilities!\n", id() );
                        player()->match()->enqueueMessage(
                                this,
                                common::PlayerServerMessage::unitMsg(
                                        common::UnitServerMessage::abortMsg( m_data ) ) );
                        break;
                    }
                    // FIXME: check if we're on a tile boundary
                    auto* structure = player()->getStructure( msg->id() );
                    if ( structure != nullptr && structure->isAdjacentTo( tileX(), tileY() ) ) {
                        m_data.setState( common::Constructing );
                        m_data.structureId = msg->id();
                        player()->match()->enqueueMessage(
                                this,
                                common::PlayerServerMessage::unitMsg(
                                        common::UnitServerMessage::constructMsg( m_data ) ) );
                    } else {
                        // FIXME: should probably send a message back
                        if ( structure == nullptr ) {
                            printf( "Unit %d unable to construct structure %d, doesn't exist\n", id(), msg->id() );
                            player()->match()->enqueueMessage(
                                    this,
                                    common::PlayerServerMessage::unitMsg(
                                            common::UnitServerMessage::abortMsg( m_data ) ) );
                        } else {
                            printf( "Unit %d unable to construct structure %d, not adjacent\n", id(), msg->id() );
                            player()->match()->enqueueMessage(
                                    this,
                                    common::PlayerServerMessage::unitMsg(
                                            common::UnitServerMessage::abortMsg( m_data ) ) );
                        }
                    }
                    break;
                }

                case common::UnitCommandMessage::Harvest: {
                    if ( !m_type->canHarvest() ) {
                        printf( "[%d] Received Harvest command but unit has no harvesting abilities!\n", id() );
                        player()->match()->enqueueMessage(
                                this,
                                common::PlayerServerMessage::unitMsg(
                                        common::UnitServerMessage::abortMsg( m_data ) ) );
                        break;
                    }
                    // FIXME: check if we're on a tile boundary
                    m_data.setState( common::Harvesting );
                    if ( player()->terrain()->ore( tileX(), tileY() ) == 0 ) {
                        m_data.setState( common::Idle );
                        player()->match()->enqueueMessage(
                                this,
                                common::PlayerServerMessage::unitMsg(
                                        common::UnitServerMessage::doneMsg( m_data ) ) );
                        break;
                    }

                    player()->match()->enqueueMessage(
                            this,
                            common::PlayerServerMessage::unitMsg(
                                    common::UnitServerMessage::harvestMsg( m_data ) ) );
                    break;
                }

                case common::UnitCommandMessage::Unload: {
                    if ( m_type->maxPayload() == 0 ) {
                        printf( "[%d] Received Unload command but unit has no payload!\n", id() );
                        player()->match()->enqueueMessage(
                                this,
                                common::PlayerServerMessage::unitMsg(
                                        common::UnitServerMessage::abortMsg( m_data ) ) );
                        break;
                    }
                    // FIXME: check if we're on a tile boundary
                    // FIXME: check if we're on a docking tile
                    m_data.setState( common::Unloading );
                    if ( m_data.payload == 0 ) {
                        m_data.setState( common::Idle );
                        player()->match()->enqueueMessage(
                                this,
                                common::PlayerServerMessage::unitMsg(
                                        common::UnitServerMessage::doneMsg( m_data ) ) );
                        break;
                    }

                    player()->match()->enqueueMessage(
                            this,
                            common::PlayerServerMessage::unitMsg(
                                    common::UnitServerMessage::unloadMsg( m_data ) ) );
                    break;
                }
                case common::UnitCommandMessage::Load: {
                    if ( m_type->maxPayload() == 0 ) {
                        printf( "[%d] Received Load command but unit has no payload!\n", id() );
                        player()->match()->enqueueMessage(
                                this,
                                common::PlayerServerMessage::unitMsg(
                                        common::UnitServerMessage::abortMsg( m_data ) ) );
                        break;
                    }
                    // FIXME: check if we're on a tile boundary
                    // FIXME: check if we're on a docking tile
                    m_data.setState( common::Loading );
                    if ( m_data.payload == m_type->maxPayload() ) {
                        m_data.setState( common::Idle );
                        player()->match()->enqueueMessage(
                                this,
                                common::PlayerServerMessage::unitMsg(
                                        common::UnitServerMessage::doneMsg( m_data ) ) );
                        break;
                    }

                    player()->match()->enqueueMessage(
                            this,
                            common::PlayerServerMessage::unitMsg(
                                    common::UnitServerMessage::loadMsg( m_data ) ) );
                    break;
                }
            }
        }

        common::PlayerServerMessage* Unit::update( Terrain* terrain ) {

            common::PlayerServerMessage* msg = nullptr;

            switch ( m_data.state ) {
                case common::Constructing: {
                    auto* structure = player()->getStructure( m_data.structureId );
                    if ( structure == nullptr || structure->constructionCompleted() ) {
                        m_data.setState( common::Idle );
                        return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::doneMsg( m_data ) );
                    } else {
                        if ( m_data.payload > 0 ) {
                            structure->constructionTick();
                            m_data.payload--;
                        } else {
                            return common::PlayerServerMessage::unitMsg(
                                    common::UnitServerMessage::abortMsg( m_data ) );
                        }

                        if ( structure->constructionCompleted() ) {
                            return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::doneMsg( m_data ) );
                        }

                        if ( m_data.payload % 64 == 0 ) {
                            //printf( "Updating payload %d\n", m_data.payload );
                            return common::PlayerServerMessage::unitMsg(
                                    common::UnitServerMessage::statusMsg( m_data ) );
                        }
                        return nullptr;
                    }
                }

                case common::Harvesting: {
                    if ( m_data.payload == m_type->maxPayload() ) {
                        //printf( "Harvester full, payload is %d\n", m_data.payload );
                        m_data.setState( common::Idle );
                        return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::doneMsg( m_data ) );
                    }
                    uint8_t oreTile = player()->terrain()->harvest( tileX(), tileY() );
                    if ( oreTile > 0 ) {
                        m_data.payload++;
                        //printf( "Harvesting from tile %d, %d, ore amount was %d\n", tileX(), tileY(), oreTile );
                    }
                    if ( oreTile <= 1 || m_data.payload == m_type->maxPayload() ) {
                        //printf( "Harvesting done, payload is now %d\n", m_data.payload );
                        m_data.setState( common::Idle );
                        if ( oreTile == 1 ) {
                            player()->terrain()->markCellDirty( tileX(), tileY() );
                        }
                        return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::doneMsg( m_data ) );
                    }
                    if ( m_data.payload % 64 == 0 ) {
                        //printf( "Updating payload %d\n", m_data.payload );
                        return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::statusMsg( m_data ) );
                    }
                    return nullptr;
                }

                case common::Unloading: {
                    if ( m_data.payload == 0 ) {
                        //printf( "Unit empty, payload is %d\n", m_data.payload );
                        m_data.setState( common::Idle );
                        return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::doneMsg( m_data ) );
                    }
                    // FIXME: Check if construction is complete!
                    auto* structure = terrain->getConstructedStructureAt( tileX(), tileY() );
                    if ( structure == nullptr ) {
                        printf( "Unit not at structure, abort unloading\n" );
                        m_data.setState( common::Idle );
                        return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::abortMsg( m_data ) );
                    }
                    if ( structure->type()->id() != m_type->dockStructureType() ) {
                        printf( "Unit at wrong structure type %d, abort unloading\n", structure->type()->id() );
                        m_data.setState( common::Idle );
                        return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::abortMsg( m_data ) );
                    }
                    if ( (terrain->occupancy( tileX(), tileY() ) & common::occupancy::bitDockingPoint) == 0 ) {
                        printf( "Unit at not at docking point, abort unloading\n" );
                        m_data.setState( common::Idle );
                        return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::abortMsg( m_data ) );
                    }
                    // FIXME: should differ based on structure type
                    m_data.payload -= player()->incCredits( 1 );

                    if ( m_data.payload == 0 ) {
                        //printf( "Unit empty, payload is %d\n", m_data.payload );
                        m_data.setState( common::Idle );
                        player()->markDirty();
                        return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::doneMsg( m_data ) );
                    }
                    if ( m_data.payload % 64 == 0 ) {
                        //printf( "Updating payload %d\n", m_data.payload );
                        return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::statusMsg( m_data ) );
                    }
                    return nullptr;
                }

                case common::Loading: {
                    if ( m_data.payload == m_type->maxPayload() ) {
                        //printf( "Unit loaded, payload is %d\n", m_data.payload );
                        m_data.setState( common::Idle );
                        return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::doneMsg( m_data ) );
                    }
                    // FIXME: Check if construction is complete!
                    auto* structure = terrain->getConstructedStructureAt( tileX(), tileY() );
                    if ( structure == nullptr ) {
                        printf( "Unit not at structure, abort loading\n" );
                        m_data.setState( common::Idle );
                        return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::abortMsg( m_data ) );
                    }
                    if ( structure->type()->id() != m_type->dockStructureType() ) {
                        printf( "Unit at wrong structure type %d, abort loading\n", structure->type()->id() );
                        m_data.setState( common::Idle );
                        return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::abortMsg( m_data ) );
                    }
                    if ( (terrain->occupancy( tileX(), tileY() ) & common::occupancy::bitDockingPoint) == 0 ) {
                        printf( "Unit at not at docking point, abort loading\n" );
                        m_data.setState( common::Idle );
                        return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::abortMsg( m_data ) );
                    }
                    // FIXME: should differ based on structure type
                    m_data.payload += player()->decCredits( 1 );

                    if ( m_data.payload == m_type->maxPayload() ) {
                        //printf( "Unit loaded, payload is %d\n", m_data.payload );
                        m_data.setState( common::Idle );
                        player()->markDirty();
                        return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::doneMsg( m_data ) );
                    }
                    if ( m_data.payload % 64 == 0 ) {
                        //printf( "Updating payload %d\n", m_data.payload );
                        return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::statusMsg( m_data ) );
                    }
                    return nullptr;
                }

                case common::Idle: {
                    if ( m_wayPoints.empty() ) {
                        return nullptr;
                    }

                    m_data.setState( common::Moving );
                    m_data.wayPoint = m_wayPoints.back();
                    m_data.wayPoint.x = (m_data.wayPoint.x * math::fix::precision) + math::fix::halfPrecision;
                    m_data.wayPoint.y = (m_data.wayPoint.y * math::fix::precision) + math::fix::halfPrecision;
                    m_wayPoints.pop_back();
                    msg = common::PlayerServerMessage::unitMsg( common::UnitServerMessage::moveToMsg( m_data ) );
                    // FALL THROUGH!
                }

                case common::Moving: {
                    int32_t oldTileX = tileX();
                    int32_t oldTileY = tileY();

                    if ( !m_data.updateMotion( terrain->data() ) ) {
                        m_data.setState( common::Idle );
                        delete msg;
                        m_wayPoints.clear();
                        return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::abortMsg( m_data ) );
                    }

                    if ( tileY() != oldTileY || tileX() != oldTileX ) {
                        player()->unFog( tileX(), tileY(), 10 );
                        player()->match()->updateSubscriptions( this );
                        terrain->updateUnit( this, oldTileX, oldTileY );
                    }

                    if ( m_data.state == common::Idle ) {
                        // reached wayPoint
                        if ( m_wayPoints.empty() ) {
                            delete msg;
                            msg = common::PlayerServerMessage::unitMsg( common::UnitServerMessage::doneMsg( m_data ) );
                        }
                    }
                    return msg;
                }
            }
            return nullptr;


            /*if ( getSubTileX() != 0 || getSubTileY() != 0 ) {
                // not at a tile boundary, keep moving
                int32_t maxSpeed = m_type->maxSpeed( UnitType::Land );
                int32_t acceleration = m_type->acceleration( UnitType::Land );
                int32_t deceleration = m_type->deceleration( UnitType::Land );

                // check if we need to update the speed
                if ( m_wayPoints.size() == 1 ) {
                    // last way point, we may have to decelerate
                    int32_t diffX = m_wayPoints.back().x - getX();
                    int32_t diffY = m_wayPoints.back().y - getY();
                    int64_t distanceSquared = math::fix::mul(diffX, diffX) + math::fix::mul(diffY, diffY);

                    // x = (-v^2) / (2*a)
                    // fixed point math, yay
                    int64_t stoppingDistance = math::fix::mul(m_speed, m_speed);
                    stoppingDistance = math::fix::div(stoppingDistance, 2 * deceleration);
                    if (distanceSquared <= math::fix::mul(stoppingDistance, stoppingDistance)) {
                        m_motion = Decelerating;
                    }
                }

                if ( m_motion != Decelerating && m_speed < maxSpeed ) {
                    m_motion = Accelerating;
                }

                switch ( m_motion ) {
                    case Decelerating:
                        m_speed -= deceleration;
                        if (m_speed < 0) {
                            m_speed = 0;
                            m_motion = Stopped;
                        }
                        break;

                    case Accelerating:
                        m_speed += acceleration;
                        if (m_speed > maxSpeed) {
                            m_speed = maxSpeed;
                            m_motion = Constant;
                        }
                        break;

                    default:
                        break;
                }

                if ( m_motion != Stopped ) {
                    m_x += speedX( m_speed, m_orientation );
                    m_y += speedY( m_speed, m_orientation );
                }
            }*/
        }

        void Unit::destroy() {
            player()->destroyUnit( this );
        }

        int32_t Unit::speedX( int32_t speed, int32_t orientation ) const {
            return static_cast<int32_t>(speed * ::cosf( (orientation * math::fPi) / 65536.0f ));
        }

        int32_t Unit::speedY( int32_t speed, int32_t orientation ) const {
            return static_cast<int32_t>(speed * ::sinf( (orientation * math::fPi) / 65536.0f ));
        }

        void Unit::dump() {
            printf( "Unit [%d] (%d:%s) at %d,%d,%d (payload %d/%d)\n", id(), m_type->id(), m_type->name().c_str(),
                    tileX(), tileY(),
                    tileZ(), m_data.payload, m_type->maxPayload() );
        }
    }
}
