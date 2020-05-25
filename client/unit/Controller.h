#pragma once

#include <vector>

#include "../types.h"
#include "../Unit.h"

namespace isomap {
    namespace client {
        namespace unit {

            class Controller {
            public:
                explicit Controller( Unit* unit );

                virtual ~Controller();

                Controller( const Controller& ) = delete;

                Controller& operator=( const Controller& ) = delete;

                bool moveTo( const PathCondition& pathCondition );

                virtual bool construct( Structure* structure );

                virtual bool harvest();

                virtual bool unload();

                virtual bool load();

                virtual void update();

                virtual void onMessage( common::UnitServerMessage::Type msgType );

                virtual void dump();

                Unit* unit() const {
                    return m_unit;
                }

                void setWorkProvider( WorkProvider* workProvider ) {
                    m_workProvider = workProvider;
                }

                WorkProvider* workProvider() const {
                    return m_workProvider;
                }

                void assignStructure( id_t structureId ) {
                    m_assignedStructureId = structureId;
                }

                id_t assignedStructureId() const {
                    return m_assignedStructureId;
                }

                Structure* assignedStructure();

            private:
                void onDone();

                void onMove();

                void onAbort();

                void updateWayPoints();

                std::vector<common::WayPoint> findPath( const PathCondition& pathCondition ) const;

                Unit* m_unit;
                std::vector<common::WayPoint> m_wayPoints;
                WorkProvider* m_workProvider = nullptr;

                id_t m_assignedStructureId = 0;
            };
        }

    }
}
