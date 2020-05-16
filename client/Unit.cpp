#include <queue>

#include "Match.h"
#include "Player.h"
#include "Terrain.h"
#include "Unit.h"
#include "../common/UnitMessage.h"
#include "../util/math.h"


namespace isomap {
    namespace client {

        Unit::Unit( Player* player, const common::UnitData& data ) :
                m_player( player ),
                m_data( data ),
                m_type( common::UnitType::get( data.typeId ) ) {
            //printf( "New unit for player [%s.%s], id [%d] %p\n", m_player->match()->player()->name().c_str(), m_player->name().c_str(), id(), this );
        }


        Unit::~Unit() = default; /*{
            printf( "Delete unit for player [%s.%s], id [%d] %p\n", m_player->match()->player()->name().c_str(), m_player->name().c_str(), id(), this );
        }*/


        void Unit::processMessage( common::UnitServerMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            switch ( msg->type() ) {
                case common::UnitServerMessage::Construct:
                case common::UnitServerMessage::Status:
                case common::UnitServerMessage::MoveTo:
                case common::UnitServerMessage::Stop:
                case common::UnitServerMessage::Done:
                case common::UnitServerMessage::Abort:
                    m_data = msg->data();
                    break;
            }
            if ( m_controller ) {
                m_controller->onMessage( msg->type() );
            }
        }

        void Unit::update() {
            switch ( m_data.state ) {
                case common::Moving: {
                    int32_t oldTileX = tileX();
                    int32_t oldTileY = tileY();

                    m_data.updateMotion();

                    // FIXME: move out of bounds check to somewhere else
                    if ( m_data.x < 0 ) {
                        m_data.x = 0;
                    } else if ( m_data.x >= m_player->terrain()->width() * math::fix::precision ) {
                        m_data.x = (m_player->terrain()->width() - 1) * math::fix::precision;
                    }
                    if ( m_data.y < 0 ) {
                        m_data.y = 0;
                    } else if ( m_data.y >= m_player->terrain()->height() * math::fix::precision ) {
                        m_data.y = (m_player->terrain()->height() - 1) * math::fix::precision;
                    }

                    // FIXME: move height calculation to somewhere else
                    m_data.z = m_player->terrain()->heightMap()[tileY() * m_player->terrain()->width() + tileX()] *
                               math::fix::precision;

                    if ( tileX() != oldTileX || tileY() != oldTileY ) {
                        m_player->terrain()->updateUnit( this, oldTileX, oldTileY );
                    }
                    break;
                }

                default:
                    break;
            }

            if ( m_controller ) {
                m_controller->update();
            }
        }

        void Unit::setVisible( const common::UnitData& data ) {
            m_data = data;
            if ( !m_visible ) {
                m_visible = true;
                if ( m_renderer ) {
                    m_renderer->setVisible();
                }
            }
        }

        void Unit::setInvisible() {
            if ( m_visible ) {
                m_visible = false;
                if ( m_renderer ) {
                    m_renderer->setInvisible();
                }
            }
        }


        void Unit::dump() const {
            printf( "client Unit [%d] (%d:%s) at %d,%d,%d (%s)\n",
                    id(),
                    m_type->id(),
                    m_type->name().c_str(),
                    tileX(),
                    tileY(),
                    tileZ(),
                    stateName()
            );
            if ( m_controller ) {
                m_controller->dump();
            }
        }

    }
}