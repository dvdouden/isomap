#include "math.h"

namespace math {

    rng::rng( unsigned int seed ) :
            engine( seed ) { }

    rng& rng::operator>>( int& nr ) {
        nr = engine();
        return *this;
    }

    rng& rng::operator>>( unsigned char& nr ) {
        nr = engine();
        return *this;
    }

    int rng::operator()( int max ) {
        return engine() % max;
    }

    int rng::operator()( int min, int max ) {
        return min + (engine() % (max - min));
    }

    namespace fix {
        int64_t mul( int32_t lhs, int32_t rhs ) {
            // shift 16 right, but use a divide to shut Clang-tidy up
            return static_cast<int32_t>((int64_t( lhs ) * int64_t( rhs )) / 65536 );
        }

        int32_t div( int32_t lhs, int32_t rhs ) {
            return static_cast<int32_t>(int64_t( lhs * 65536 ) / rhs);
        }

        int64_t div( int64_t lhs, int32_t rhs ) {
            return static_cast<int32_t>((lhs * 65536) / rhs);
        }
    }

}
