#ifndef MATHS_H
#define MATHS_H

#include <math.h>

class CMaths
{

public:
    CMaths(){}
    ~CMaths(){}

    // Function that returns the minimum number of three
    //static inline double minimumOfThree( const double &a, const double &b, const double &c){return a<b?a<c?a:c:b<c?b:c;}
    // static inline int minimumOfThree( const int &a, const int &b, const int &c){return a<b?a<c?a:c:b<c?b:c;}
    // Function that returns the maximum number of three
    static inline double maximumOfThree( const double &a, const double &b, const double &c){return a>b?a>c?a:c:b>c?b:c;}
    static inline int maximumOfThree( const int &a, const int &b, const int &c){return a>b?a>c?a:c:b>c?b:c;}
};

#endif
