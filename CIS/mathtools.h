#ifndef MATHS_H
#define MATHS_H

#include <math.h>

class Maths
{

public:
    Maths(){}
    ~Maths(){}

    // Function that returns the maximum number of three
    int maximumOfThree( const int &x , const int &y , const int &z ) {
        int max = x;

        if ( y > max ) {
            max = y;
        }

        if ( z > max ) {
            max = z;
        }

        return max;
    }
};

#endif
