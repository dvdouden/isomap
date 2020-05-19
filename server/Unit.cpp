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
                m_data( {id(), unitType->id(), 0, x * math::fix::precision, y * math::fix::precision,
                         z * math::fix::precision, orientation} ),
                m_type( unitType ) {
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
                    if ( !m_type->canHarvest() ) {
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
            }
        }

        common::PlayerServerMessage* Unit::update( Terrain* terrain ) {
            if ( m_wayPoints.empty() && m_data.state == common::Idle ) {
                return nullptr;
            }

            if ( m_data.state == common::Constructing ) {
                auto* structure = player()->getStructure( m_data.structureId );
                if ( structure == nullptr || structure->constructionCompleted() ) {
                    m_data.setState( common::Idle );
                    return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::doneMsg( m_data ) );
                } else {
                    structure->constructionTick();
                    return nullptr;
                }
            } else if ( m_data.state == common::Harvesting ) {
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
                    return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::harvestMsg( m_data ) );
                }
                return nullptr;
            } else if ( m_data.state == common::Unloading ) {
                if ( m_data.payload == 0 ) {
                    //printf( "Harvester empty, payload is %d\n", m_data.payload );
                    m_data.setState( common::Idle );
                    return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::doneMsg( m_data ) );
                }
                auto* structure = terrain->getConstructedStructureAt( tileX(), tileY() );
                if ( structure == nullptr ) {
                    printf( "Harvester not at structure, abort unloading\n" );
                    m_data.setState( common::Idle );
                    return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::abortMsg( m_data ) );
                }
                if ( structure->type()->id() != m_type->dockStructureType() ) {
                    printf( "Harvester at wrong structure type %d, abort unloading\n", structure->type()->id() );
                    m_data.setState( common::Idle );
                    return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::abortMsg( m_data ) );
                }
                if ( (terrain->occupancy( tileX(), tileY() ) & common::occupancy::bitDockingPoint) == 0 ) {
                    printf( "Harvester at not at docking point, abort unloading\n" );
                    m_data.setState( common::Idle );
                    return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::abortMsg( m_data ) );
                }
                m_data.payload -= player()->incCredits( 1 );

                if ( m_data.payload == 0 ) {
                    //printf( "Harvester empty, payload is %d\n", m_data.payload );
                    m_data.setState( common::Idle );
                    player()->markDirty();
                    return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::doneMsg( m_data ) );
                }
                if ( m_data.payload % 64 == 0 ) {
                    //printf( "Updating payload %d\n", m_data.payload );
                    return common::PlayerServerMessage::unitMsg( common::UnitServerMessage::harvestMsg( m_data ) );
                }
                return nullptr;
            }

            common::PlayerServerMessage* msg = nullptr;
            if ( m_data.state == common::Idle ) {
                m_data.setState( common::Moving );
                m_data.wayPoint = m_wayPoints.back();
                m_wayPoints.pop_back();
                msg = common::PlayerServerMessage::unitMsg( common::UnitServerMessage::moveToMsg( m_data ) );
            }

            int32_t oldTileX = tileX();
            int32_t oldTileY = tileY();

            // let's keep things simple for now...
            m_data.updateMotion();

            // FIXME: move out of bounds check to somewhere else
            if ( m_data.x < 0 ) {
                m_data.x = 0;
            } else if ( m_data.x >= terrain->width() * math::fix::precision ) {
                m_data.x = (terrain->width() - 1) * math::fix::precision;
            }
            if ( m_data.y < 0 ) {
                m_data.y = 0;
            } else if ( m_data.y >= terrain->height() * math::fix::precision ) {
                m_data.y = (terrain->height() - 1) * math::fix::precision;
            }

            // FIXME: move height calculation to somewhere else
            m_data.z = terrain->heightMap()[tileY() * terrain->width() + tileX()] * math::fix::precision;
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
