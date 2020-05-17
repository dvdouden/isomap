#include "Player.h"
#include "Structure.h"
#include "Terrain.h"
#include "Unit.h"
#include "../common/PlayerMessage.h"
#include "../common/StructureMessage.h"


namespace isomap {
    namespace server {

        Structure::Structure(
                Player* owner,
                uint32_t x,
                uint32_t y,
                uint32_t z,
                common::StructureType* structureType,
                uint32_t orientation ) :
                Object( owner ),
                m_data( {id(), structureType->id(), x, y, z, orientation, 0} ),
                m_type( structureType ) {

        }

        common::PlayerServerMessage* Structure::update( Terrain* world ) {
            if ( m_dirty ) {
                m_dirty = false;
                if ( constructionCompleted() ) {
                    return common::PlayerServerMessage::structureMsg( completedMessage() );
                } else {
                    return common::PlayerServerMessage::structureMsg( statusMessage() );
                }
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

        void Structure::constructionTick() {
            if ( m_data.constructionProgress < 100 ) {
                m_dirty = true;
                m_data.constructionProgress++;
                if ( m_data.constructionProgress == 100 ) {
                    if ( m_type->includedUnitType() != nullptr ) {
                        player()->registerNewUnit( spawnUnit() );
                    }
                }
            }
        }

        common::StructureServerMessage* Structure::completedMessage() {
            return common::StructureServerMessage::completedMsg( m_data );
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

        Unit* Structure::spawnUnit() const {
            common::UnitType* unitType = m_type->includedUnitType();
            if ( unitType == nullptr ) {
                printf( "[%d] Spawn called on structure that doesn't include unit!\n", id() );
                return nullptr;
            }

            // find spawn point
            for ( uint32_t y = 0; y < footPrint()->height(); ++y ) {
                for ( uint32_t x = 0; x < footPrint()->width(); ++x ) {
                    if ( footPrint()->get( x, y ) & common::occupancy::bitSpawnPoint ) {
                        return new Unit(
                                player(),
                                m_data.x + x,
                                m_data.y + y,
                                m_data.z,
                                unitType,
                                m_data.orientation * 2 );
                    }
                }
            }
            printf( "[%d] Structure has included unit type but no spawn point!\n", id() );
            return nullptr;
        }

        void Structure::dump() {
            printf( "Structure [%d] (%d:%s) at %d,%d\n", id(), m_type->id(), m_type->name().c_str(), m_data.x,
                    m_data.y );
        }

    }
}
