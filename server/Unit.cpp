#include "Player.h"
#include "Terrain.h"
#include "Unit.h"
#include "UnitType.h"
#include "../util/math.h"
#include "../common/PlayerMessage.h"
#include "../common/UnitMessage.h"

namespace isomap {
    namespace server {

        Unit::Unit( Player* owner, int32_t x, int32_t y, int32_t z, common::UnitType* unitType,
                    uint32_t orientation ) :
                Object( owner ),
                m_data( {id(), unitType->id(), x * math::fix::precision, y * math::fix::precision,
                         z * math::fix::precision, orientation} ),
                m_type( unitType ) {
        }


        void Unit::processMessage( common::UnitCommandMessage* msg ) {
            switch ( msg->type() ) {
                case common::UnitCommandMessage::Move:
                    m_wayPoints = msg->wayPoints();
                    break;

                default:
                    break;
            }
        }

        common::PlayerServerMessage* Unit::update( Terrain* terrain ) {
            if ( m_wayPoints.empty() && m_data.motionState == common::Stopped ) {
                return nullptr;
            }

            common::PlayerServerMessage* msg = nullptr;
            if ( m_data.motionState == common::Stopped ) {
                m_data.motionState = common::Moving;
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
                player()->unFog( tileX(), tileY(), 20 );
            }

            if ( m_data.motionState == common::Stopped ) {
                // reached wayPoint
                if ( m_wayPoints.empty() ) {
                    delete msg;
                    msg = common::PlayerServerMessage::unitMsg( common::UnitServerMessage::stopMsg( m_data ) );
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

        int32_t Unit::speedX( int32_t speed, int32_t orientation ) const {
            return static_cast<int32_t>(speed * ::cos( (orientation * math::fPi) / 65536.0f ) );
        }

        int32_t Unit::speedY( int32_t speed, int32_t orientation ) const {
            return static_cast<int32_t>(speed * ::sin( (orientation * math::fPi) / 65536.0f ) );
        }
    }
}
