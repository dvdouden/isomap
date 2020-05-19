#pragma once

#include "PathCondition.h"
#include "../Structure.h"
#include "../Terrain.h"

namespace isomap {
    namespace client {
        namespace unit {

            class StructureDockingTilePathCondition : public PathCondition {
            public:
                explicit StructureDockingTilePathCondition( Structure* structure ) :
                        m_structure( structure ) { }

                bool hasReached( uint32_t idx ) const override {
                    return m_structure->dockingTileAt( idx );
                }

            private:
                Structure* m_structure;
            };

            class StructureTypeDockingTilePathCondition : public PathCondition {
            public:
                StructureTypeDockingTilePathCondition(
                        id_t structureTypeId,
                        Terrain* terrain ) :
                        m_structureTypeId( structureTypeId ),
                        m_terrain( terrain ) { }

                bool hasReached( uint32_t idx ) const override {
                    uint32_t x = idx % m_terrain->width();
                    uint32_t y = idx / m_terrain->width();
                    auto* structure = m_terrain->getConstructedStructureAt( x, y );
                    if ( structure == nullptr ) {
                        return false;
                    }
                    if ( structure->data().typeId != m_structureTypeId ) {
                        return false;
                    }
                    return structure->dockingTileAt( x, y );
                }

            private:
                id_t m_structureTypeId;
                Terrain* m_terrain;
            };


        }

    }
}

