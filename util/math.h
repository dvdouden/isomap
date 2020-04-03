#ifndef TESTPROJECT_MATH_H
#define TESTPROJECT_MATH_H

#include <random>

namespace math {
    class rng {
    public:
        rng( unsigned int seed );
        rng( const rng& ) = delete;
        ~rng() = default;
        const rng& operator=(const rng&) = delete;

        int operator()(int);
        int operator()(int, int);
        rng& operator>>( int& nr );
        rng& operator>>( unsigned char& nr );

    private:
        std::mt19937 engine;
    };
}

#endif //TESTPROJECT_MATH_H
