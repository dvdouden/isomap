#ifndef TESTPROJECT_MATH_H
#define TESTPROJECT_MATH_H

#include <random>

namespace math {
    class rng {
    public:
        rng( unsigned int seed );

        rng( const rng& ) = delete;

        ~rng() = default;

        const rng& operator=( const rng& ) = delete;

        int operator()( int );

        int operator()( int, int );

        rng& operator>>( int& nr );

        rng& operator>>( unsigned char& nr );

    private:
        std::mt19937 engine;
    };


    //! Greek Pi constant using \p double precision.
    const double dPi = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093845;
    //! Constant to convert degree into radian using \p double precision.
    const double dDEG_TO_RAD = dPi / 180.0;
    //! Constant to convert radian into degree using \p double precision.
    const double dRAD_TO_DEG = 180.0 / dPi;


    //! Greek Pi constant using \p float precision.
    const float fPi = (float)3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093845;
    //! Constant to convert degree into radian using \p float precision.
    const float fDEG_TO_RAD = float( dPi / 180.0 );
    //! Constant to convert radian into degree using \p float precision.
    const float fRAD_TO_DEG = float( 180.0 / dPi );

    namespace fix {
        int64_t mul( int32_t lhs, int32_t rhs );

        int32_t div( int32_t lhs, int32_t rhs );

        int64_t div( int64_t lhs, int32_t rhs );

        const uint32_t precisionBits = 12u;
        const int32_t precision = 1u << precisionBits;
        const int32_t precisionMask = precision - 1;
        const float fPrecision = precision;
    }
}

#endif //TESTPROJECT_MATH_H
