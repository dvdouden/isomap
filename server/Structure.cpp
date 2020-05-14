#include "Player.h"
#include "Structure.h"
#include "Terrain.h"
#include "../common/PlayerMessage.h"
#include "../common/StructureMessage.h"


namespace isomap {
    namespace server {

        common::PlayerServerMessage* Structure::update( Terrain* world ) {
            if ( m_dirty ) {
                m_dirty = false;
                return common::PlayerServerMessage::structureMsg( statusMessage() );
            }
            return nullptr;
        }


        void Structure::processMessage( common::StructureCommandMessage* msg ) {
            switch ( msg->type() ) {
                case common::StructureCommandMessage::Move:
                    break;

                default:
                    break;
            }
        }

        common::StructureServerMessage* Structure::statusMessage() {
            return common::StructureServerMessage::statusMsg( m_data );
        }


        bool Structure::occupies( uint32_t x, uint32_t y ) const {
            if ( x >= m_data.x + m_type->width( m_data.orientation ) ) {
                return false; // structure right of point
            }
            if ( x < m_data.x ) {
                return false; // structure left of point
            }
            if ( y >= m_data.y + m_type->height( m_data.orientation ) ) {
                return false; // structure below point
            }
            if ( y < m_data.y ) {
                return false; // structure above point
            }
            return footPrint()->get( x - m_data.x, y - m_data.y ) != 0;
        }


        bool Structure::isAdjacentTo( uint32_t x, uint32_t y ) const {
            return
                    (x > 0 && occupies( x - 1, y )) ||
                    (x < player()->terrain()->width() - 1 && occupies( x + 1, y )) ||
                    (y > 0 && occupies( x, y - 1 )) ||
                    (y < player()->terrain()->height() - 1 && occupies( x, y + 1 ));
        }

        void Structure::destroy() {
            player()->destroyStructure( this );
        }

        void Structure::dump() {
            printf( "Structure [%d] (%d:%s) at %d,%d\n", id(), m_type->id(), m_type->name().c_str(), m_data.x,
                    m_data.y );
        }

    }
}
