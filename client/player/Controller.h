#pragma once

#include "../types.h"

namespace isomap {
    namespace client {
        namespace player {

            class Controller {
            public:
                explicit Controller( Player* player ) :
                        m_player( player ) {
                }

                virtual ~Controller() = default;

                Controller( const Controller& ) = delete;

                Controller& operator=( const Controller& ) = delete;


                void buildStructure(
                        int32_t tileX,
                        int32_t tileY,
                        common::StructureType* structureType,
                        uint32_t orientation );

                bool canPlace(
                        int32_t worldX,
                        int32_t worldY,
                        common::StructureType* structureType,
                        uint32_t rotation ) const;

                // FIXME: should be replaced with buildqueue
                void buildUnit(
                        int32_t tileX,
                        int32_t tileY,
                        common::UnitType* unitType,
                        uint32_t orientation );

                void enqueueMessage( common::PlayerCommandMessage* msg );

                void enqueueMessage( id_t id, common::UnitCommandMessage* msg );

                virtual void update() = 0;

                virtual void onUnitCreated( Unit* unit ) = 0;

                virtual void onUnitDestroyed( Unit* unit ) = 0;

                virtual void onStructureCreated( Structure* structure ) = 0;

                virtual void onConstructionComplete( Structure* structure ) = 0;

                virtual void onStructureDestroyed( Structure* structure ) = 0;

                virtual void onUnableToConstruct( Structure* structure, Unit* unit ) = 0;

                Player* player() {
                    return m_player;
                }

                virtual void dump() const = 0;

            private:
                Player* m_player;
            };

        }
    }
}
