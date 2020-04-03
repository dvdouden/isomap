#include "math.h"

namespace math {

    rng::rng( unsigned int seed ) :
    engine( seed ) {}

    rng& rng::operator>>( int& nr ) {
        nr = engine();
        return *this;
    }

    rng& rng::operator>>( unsigned char& nr ) {
        nr = engine();
        return *this;
    }

    int rng::operator()(int max) {
        return engine() % max;
    }

    int rng::operator()(int min, int max) {
        return min + (engine() % (max - min));
    }

}
