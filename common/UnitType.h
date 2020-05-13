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

            static UnitType* get( id_t id );

            static void load();

            static void clear();

        private:
            UnitType( id_t id, std::string name );

            id_t m_id;
            std::string m_name;

        };

    }
}
