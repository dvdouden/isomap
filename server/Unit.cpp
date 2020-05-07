#include "Player.h"
#include "Terrain.h"
#include "Unit.h"
#include "UnitType.h"
#include "../util/math.h"
#include "../common/PlayerMessage.h"
#include "../common/UnitMessage.h"

namespace isomap {
    namespace server {

        void Unit::processMessage( common::UnitCommandMessage* msg ) {
            switch ( msg->type() ) {
                case common::UnitCommandMessage::Move:
                    m_wayPoints.clear();
                    for ( const auto& wayPoint : msg->wayPoints() ) {
                        // TODO: fix orientation
                        m_wayPoints.push_back( {wayPoint.x, wayPoint.y, 0} );
                    }
                    break;

                default:
                    break;
            }
        }

        common::UnitServerMessage* Unit::statusMessage() {
            return common::UnitServerMessage::statusMsg( m_data );
        }

        common::PlayerServerMessage* Unit::update( Terrain* terrain ) {
            // should probably do something with states
            if ( m_wayPoints.empty() ) {
                return nullptr;
            }

            // let's keep things simple for now...
            WayPoint& wayPoint = m_wayPoints.back();
            if ( m_data.x != wayPoint.x ) {
                if ( m_data.x < wayPoint.x ) {
                    ++m_data.x;
                } else {
                    --m_data.x;
                }
            }
            if ( m_data.y != wayPoint.y ) {
                if ( m_data.y < wayPoint.y ) {
                    ++m_data.y;
                } else {
                    --m_data.y;
                }
            }
            if ( m_data.x < 0 ) {
                m_data.x = 0;
            } else if ( m_data.x >= terrain->width() ) {
                m_data.x = terrain->width() - 1;
            }
            if ( m_data.y < 0 ) {
                m_data.y = 0;
            } else if ( m_data.y >= terrain->height() ) {
                m_data.y = terrain->height() - 1;
            }
            m_data.z = terrain->heightMap()[m_data.y * terrain->width() + m_data.x];
            player()->unFog( m_data.x, m_data.y, 20 );
            if ( m_data.x == wayPoint.x && m_data.y == wayPoint.y ) {
                m_wayPoints.pop_back();
                return common::PlayerServerMessage::unitMsg( statusMessage() );
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

        int32_t Unit::speedX( int32_t speed, int32_t orientation ) const {
            return static_cast<int32_t>(speed * ::cos( (orientation * math::fPi) / 65536.0f ) );
        }

        int32_t Unit::speedY( int32_t speed, int32_t orientation ) const {
            return static_cast<int32_t>(speed * ::sin( (orientation * math::fPi) / 65536.0f ) );
        }
    }
}
