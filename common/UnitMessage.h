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
                Construct,
                Harvest,
            };

            explicit UnitCommandMessage( Type type ) :
                    m_type( type ) { }

            const char* typeName() const;

            Type type() const {
                return m_type;
            }

            const std::vector<WayPoint>& wayPoints() const {
                return m_wayPoints;
            }

            id_t id() const {
                return m_id;
            }

            static UnitCommandMessage* moveMsg( const std::vector<WayPoint>& wayPoints );

            static UnitCommandMessage* constructMsg( id_t structureId );

            static UnitCommandMessage* harvestMsg();

        private:
            Type m_type;
            std::vector<WayPoint> m_wayPoints;
            id_t m_id = 0;
        };

        class UnitServerMessage {
        public:
            enum Type {
                Construct,
                Harvest,
                Status,
                MoveTo,
                Stop,
                Done,
                Abort,
            };

            explicit UnitServerMessage( Type type ) :
                    m_type( type ) { }

            UnitServerMessage( const UnitServerMessage& ) = default;

            const char* typeName() const;

            Type type() const {
                return m_type;
            }

            const UnitData& data() {
                return m_data;
            }

            static UnitServerMessage* constructMsg( const UnitData& data );

            static UnitServerMessage* harvestMsg( const UnitData& data );

            static UnitServerMessage* statusMsg( const UnitData& data );

            static UnitServerMessage* moveToMsg( const UnitData& data );

            static UnitServerMessage* stopMsg( const UnitData& data );

            static UnitServerMessage* doneMsg( const UnitData& data );

            static UnitServerMessage* abortMsg( const UnitData& data );


        private:
            Type m_type;

            UnitData m_data;

        };
    }
}


