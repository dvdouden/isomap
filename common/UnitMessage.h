#pragma once


#include <vector>
#include "types.h"
#include "UnitData.h"

namespace isomap {
    namespace common {

        class UnitCommandMessage {
        public:
            enum Type {
                Move,
            };

            explicit UnitCommandMessage( Type type ) :
                    m_type( type ) { }

            Type type() const {
                return m_type;
            }

            const std::vector<WayPoint>& wayPoints() const {
                return m_wayPoints;
            }

            static UnitCommandMessage* moveMsg( std::vector<WayPoint>& wayPoints );

        private:
            Type m_type;
            std::vector<WayPoint> m_wayPoints;
        };

        class UnitServerMessage {
        public:
            enum Type {
                Create,
                Status,
                MoveTo,
                Stop,
            };

            explicit UnitServerMessage( Type type ) :
                    m_type( type ) { }

            UnitServerMessage( const UnitServerMessage& ) = default;

            Type type() const {
                return m_type;
            }

            const UnitData& data() {
                return m_data;
            }

            static UnitServerMessage* statusMsg( const UnitData& data );

            static UnitServerMessage* moveToMsg( const UnitData& data );

            static UnitServerMessage* stopMsg( const UnitData& data );

        private:
            Type m_type;

            UnitData m_data;

        };
    }
}

