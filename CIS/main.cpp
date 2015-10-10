// Headers
#include "mathtools.h"

// Libs
#include <iostream>
#include <Eigen/Dense>
#include <Eigen/LU>
#include <opencv2/opencv.hpp>

//
using namespace std;
using namespace Eigen;
using namespace cv;


// Main function
int main( int argc, char** argv )
{
    Maths math;

    // Load image to be computed (mxn)
    Mat I = imread( "/Users/ericpairet/Desktop/equations/i_test_red.jpg" );

    // Take m and n image size
    int m = I.rows;
    int n = I.cols;

    // Check if there is an image
    if ( ( m * n ) == 0 ) {
        cout << "No image" << endl;
        return -1;
    }

    // Initialize background and foreground labels (do not change the type!)
    float xb = 1;
    float xf = 0;

    // Check xb > xf
    if ( xb < xf ) {
        cout << "Background level must be greater than the foreground one" << endl;
        return -1;
    }

    // Initialize tunning constants
    float betta = 1.0;

    // Get the R, G, B channels in BGR order
    vector<Mat> channels(3);
    split(I,channels);
    Mat_<int> chR = channels[2];
    Mat_<int> chG = channels[1];
    Mat_<int> chB = channels[0];

    //**************************
    //
    // Compute matrix W and D
    //
    //**************************

    // Initialize adjency matrix W with zeros
    MatrixXf W( m * n , m * n );
    W = MatrixXf::Zero( m * n , m * n );

    // Initialize diagonal matrix D with zeros
    MatrixXf D( m * n , m * n );
    D = MatrixXf::Zero( m * n , m * n );

    // For each row
    for ( int i = 0 ; i < m ; i++ ) {
        // For each column
        for ( int j = 0 ; j < n ; j ++ ) {

            // Variable to store the sum of dii
            float tmp = 0;

            // For each row of the neighborhood
            for ( int a = i - 1 ; a <= i + 1 ; a++ ) {
                // For each column of the neighborhood
                for ( int b = j - 1 ; b <= j + 1 ; b++ ) {
                    // Avoid compute edge with itself, values out the matrix and recompute edges
                    if ( !( ( a == i ) and ( b == j ) ) and ( a >= 0 ) and ( a < m ) and ( b >= 0 ) and ( b < n ) and ( W( i * n + j , a * n + b ) == 0 ) ) {

                        // Compute wij ( Pi is defined by i and j, Pj is defined by a and b )
                        float wij = exp( - betta * pow( double( math.maximumOfThree( abs( chR( i , j ) - chR( a , b ) ),
                                                                                abs( chG( i , j ) - chG( a , b ) ),
                                                                                abs( chB( i , j ) - chB( a , b ) ) ) ) , 2 ) / 0.1 ) + 10e-6;

                        // Store the result at W in wij and wji
                        W( i * n + j , a * n + b ) = wij;
                        W( a * n + b , i * n + j ) = wij;

                        // Compute dii
                        tmp += wij;
                    }
                }
            }
            // Save final dii
            D( i * n + j , i * n + j ) = tmp;
        }
    }

    //**************************
    //
    // Compute matrix L
    //
    //**************************

    MatrixXf L( m * n , m * n );
    L = D - W;

    //**************************
    //
    // Solve linear system
    //   (Is + L^2)x = b
    //
    //**************************
/*
    //
    MatrixXf Is( m * n , m * n );
    Is = MatrixXf::Identity( m * n , m * n );       // just for test (change with the GUI)

    MatrixXf Is_L( m * n , m * n );
    Is_L = Is + L * L;

    //VectorXf b( m * n );
    float b = 4;                        // just for test (change with the GUI)

    VectorXf X( m * n );
    X = Is_L.inverse() * b;
*/
    VectorXd X( m * n );                // double? something wrong with floats
    //**************************
    //
    // Assign labels to x
    //    [ m x n , 1 ]
    //
    //**************************

    // Threshold to assign labels
    float threshold = ( ( xb + xf ) / 2 );

    // Variable to store the segmented image
    cv::Mat_<float> Y = Mat_<float>::zeros( m , n );        // float?

    // Variable used to reshape from vector to matrix
    int r = 0;

    // Assign labels, reshape from vector to matrix and change from eigen to opencv
    for ( int i = 0 ; i < m * n ; i++ ) {

        if ( ( i - r * n ) == n ) {
            r++;
        }

        if ( X( i ) >= threshold ) {
            Y( r , i - r * n ) = xb;
        }
    }


    //**************************
    //
    // Show the segmented image
    //
    //**************************

    // Show the segmented image
    //namedWindow( "SI" , CV_WINDOW_NORMAL );
    imshow( "SI" , Y );
    waitKey();

    return 0;
}
