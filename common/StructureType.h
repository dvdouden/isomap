#pragma once

#include <string>

#include "types.h"
#include "FootPrint.h"

namespace isomap {
    namespace common {
        class StructureType {
        public:
            ~StructureType();

            StructureType( const StructureType& ) = delete;

            StructureType& operator=( const StructureType& ) = delete;

            id_t id() const {
                return m_id;
            }

            const std::string& name() const {
                return m_name;
            }

            uint32_t width( uint32_t orientation ) const {
                return m_footPrint[orientation % 4u]->width();
            }

            uint32_t height( uint32_t orientation ) const {
                return m_footPrint[orientation % 4u]->height();
            }

            FootPrint* footPrint( uint32_t orientation ) const {
                return m_footPrint[orientation % 4u];
            }

            uint8_t footPrint( uint32_t orientation, uint32_t x, uint32_t y ) const {
                return m_footPrint[orientation % 4u]->get( x, y );
            }

            UnitType* includedUnitType() const;

            uint32_t creditStorage() const {
                return m_creditStorage;
            }

            void setCreditStorage( uint32_t creditStorage ) {
                m_creditStorage = creditStorage;
            }

            static StructureType* get( id_t id );

            static void load();

            static void clear();


        private:
            StructureType( id_t id, std::string name, FootPrint* footPrint, id_t spawnUnitId = 0 );

            id_t m_id;
            id_t m_spawnUnitId = 0;
            FootPrint* m_footPrint[4];
            std::string m_name;
            uint32_t m_creditStorage = 0;
        };
    }
}