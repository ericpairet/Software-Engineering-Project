#ifndef MATHS_H
#define MATHS_H

#include <math.h>

/**
 * @brief Math class : and small calss created to contain all the usefull mathematics
 * function might be used in the project
 *
 */
class CMaths
{

public:
    /**
     * @brief Cuntrator : math class cinstructor
     *
     */
    CMaths(){}
    /**
     * @brief Destactor : math class destractor
     *
     */
    ~CMaths(){}

    // Function that returns the minimum number of three
    //static inline double minimumOfThree( const double &a, const double &b, const double &c){return a<b?a<c?a:c:b<c?b:c;}
    // static inline int minimumOfThree( const int &a, const int &b, const int &c){return a<b?a<c?a:c:b<c?b:c;}
    // Function that returns the maximum number of three
    /**
     * @brief Double Max : this function fids the maximum of 3 double values
     *
     * @param a : First value
     * @param b : Second Value
     * @param c : Third value
     * @return double : max( a, b, c)
     */
    static inline double maximumOfThree( const double &a, const double &b, const double &c){return a>b?a>c?a:c:b>c?b:c;}
    /**
     * @brief Integer Max : this function fids the maximum of 3 integer values
     *
     * @param a : First value
     * @param b : Second Value
     * @param c : Third value
     * @return int : max( a, b, c)
     */
    static inline int maximumOfThree( const int &a, const int &b, const int &c){return a>b?a>c?a:c:b>c?b:c;}
};

#endif
