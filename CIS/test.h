#include <gtest/gtest.h>
#include <iostream>
#include <segmentation.h>
#include "mathtools.h"

TEST(mathToolsTest, maxOf3)
{
    ASSERT_EQ(9, CMaths::maximumOfThree( 3, 6, 9));
    ASSERT_EQ(6.3, CMaths::maximumOfThree( 3.3, 6.3, -9.6));
}

TEST( segmentationTest, sytemSolverTester)
{
    SparseMatrix<double> A( 3, 3);
    VectorXd b(3), x(3);
    A.coeffRef( 0, 0) = 1;
    A.coeffRef( 0, 1) = 2;
    A.coeffRef( 0, 2) = 3;
    A.coeffRef( 1, 0) = 4;
    A.coeffRef( 1, 1) = 5;
    A.coeffRef( 1, 2) = 6;
    A.coeffRef( 2, 0) = 7;
    A.coeffRef( 2, 1) = 8;
    A.coeffRef( 2, 2) = 10;
    b << 3, 3, 4;
    x << -2, 1, 1;
    for( int i = 0; i < 3; i++)
        EXPECT_EQ(x.coeff(i), CSegmentation::ComputeLinearSystem( A, b)->coeff(i));
}

