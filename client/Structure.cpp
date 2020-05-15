#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <vlCore/ResourceDatabase.hpp>

#include "Player.h"
#include "Structure.h"
#include "Terrain.h"
#include "../common/StructureMessage.h"
#include "../common/StructureType.h"
#include "../util/math.h"
#include "ModelCache.h"

namespace isomap {
    namespace client {


        Structure::~Structure() {

        }

        void Structure::processMessage( common::StructureServerMessage* msg ) {
            if ( msg == nullptr ) {
                return;
            }
            switch ( msg->type() ) {
                case common::StructureServerMessage::Status:
                    m_data = msg->data();
                    break;

                default:
                    break;
            }
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


        void Structure::setVisible( bool visible ) {
            if ( m_visible == visible ) {
                return;
            }
            m_visible = visible;
            if ( m_renderer ) {
                if ( visible ) {
                    m_renderer->setVisible();
                } else {
                    m_renderer->setInvisible();
                }
            }
        }

        void Structure::dump() const {
            printf( "client Structure [%d] (%d:%s) at %d,%d\n", id(), m_type->id(), m_type->name().c_str(), m_data.x,
                    m_data.y );
        }

    }
}