#pragma once

#include <map>

#include "../common/types.h"

namespace isomap { namespace server {
    class UnitType {
    public:

        bool canFly() const {
            return m_speedParams.find( Air ) != m_speedParams.end();
        }

        bool canHover() const {
            auto flyParams = m_speedParams.find( Air );
            return flyParams != m_speedParams.cend() && (*flyParams).second.minSpeed == 0;
        }

        bool canTurnInPlace() const {
            return (*m_speedParams.cbegin()).second.turningRadius == 0;
        }

        bool canFloat() const {
            return m_speedParams.find( Water ) != m_speedParams.cend();
        }
        bool canSubmerge() const {
            return m_speedParams.find( Submerged ) != m_speedParams.cend();
        }

        enum Surface {
            Air,
            Land,
            Water,
            Submerged,
            Snow,
            Tarmac,
            Grass,
        };

        int32_t minSpeed( Surface surface ) const;
        int32_t maxSpeed( Surface surface ) const;
        int32_t acceleration( Surface surface ) const;
        int32_t deceleration( Surface surface ) const;
        int32_t turningRadius( Surface surface ) const;
        int32_t turningSpeed( Surface surface ) const;

        struct speedParameters {
            int32_t minSpeed = 0;
            int32_t maxSpeed = 0;
            int32_t acceleration = 0;
            int32_t deceleration = 0;
            int32_t turningRadius = 0;
            int32_t turningSpeed = 0;
        };


    private:
        std::map<Surface, speedParameters> m_speedParams;

    };
} }

