#pragma once

#include <string>

#include "types.h"

namespace isomap {
    namespace common {

        class UnitType {
        public:
            ~UnitType() = default;

            UnitType( const UnitType& ) = delete;

            UnitType& operator=( const UnitType& ) = delete;

            id_t id() const {
                return m_id;
            }

            const std::string& name() const {
                return m_name;
            }

            bool canConstruct() const {
                return m_canConstruct;
            }

            bool canHarvest() const {
                return m_canHarvest;
            }

            void setPayload( uint32_t initialPayload, uint32_t maxPayload ) {
                m_initialPayload = initialPayload;
                m_maxPayload = maxPayload;
            }

            uint32_t initialPayload() const {
                return m_initialPayload;
            }

            uint32_t maxPayload() const {
                return m_maxPayload;
            }

            void setDockStructureType( id_t dockStructureTypeId );

            id_t dockStructureType() const {
                return m_dockStructureId;
            }

            static UnitType* get( id_t id );

            static void load();

            static void clear();


        private:
            UnitType( id_t id, std::string name, bool canConstruct, bool canHarvest );

            id_t m_id;
            std::string m_name;
            bool m_canConstruct;
            bool m_canHarvest;
            uint32_t m_initialPayload = 0;
            uint32_t m_maxPayload = 0;
            id_t m_dockStructureId = 0;

        };

    }
}
