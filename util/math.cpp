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
        std::uniform_int_distribution<int> uni( 0, max );
        return uni( engine );
    }

    int rng::operator()( int min, int max ) {
        std::uniform_int_distribution<int> uni( min, max );
        return uni( engine );
    }

    namespace fix {
        int64_t mul( int32_t lhs, int32_t rhs ) {
            // shift <precisionBits> bits right, but use a divide to shut Clang-tidy up
            return static_cast<int32_t>((int64_t( lhs ) * int64_t( rhs )) / precision );
        }

        int32_t div( int32_t lhs, int32_t rhs ) {
            return static_cast<int32_t>(int64_t( lhs * precision ) / rhs);
        }

        int64_t div( int64_t lhs, int32_t rhs ) {
            return static_cast<int32_t>((lhs * precision) / rhs);
        }
    }

}
