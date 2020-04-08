#include "Match.h"
#include "Object.h"
#include "World.h"

namespace isomap { namespace server {

    Match::Match() {

    }

    Match::~Match() {
        delete m_world;
        for ( auto obj : m_objects ) {
            delete obj.second;
        }
    }


} }
