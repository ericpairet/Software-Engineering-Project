// Headers
#include "mainwindow.h"

// Libs
#include <QApplication>
#include <iostream>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include <QtMath>

//
using namespace std;
using namespace Eigen;
using namespace cv;


// Function that returns the Euclidean norm
float euclideanNorm( const int &x , const int &y ) {
    return pow( double( ( pow( double( x ) , 2 ) + pow( double ( y ) , 2 ) ) ) , 0.5 );
}


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


//
double Graph_Wights (double Iix, double Ijx, double Iiy, double Ijy)
{
    double w, Beta, sigma, DeltaIx, DeltaIy,Ix_Pow2,Iy_Pow2,magnitudeI;

    Beta = 1;
    sigma = 0.1;
    DeltaIx = Iix - Ijx;
    DeltaIy = Iiy - Ijy;
    Ix_Pow2 = qPow(DeltaIx,2);
    Iy_Pow2 = qPow(DeltaIy,2);
    magnitudeI =Ix_Pow2 + Iy_Pow2;

    w = qExp((-Beta*magnitudeI)/sigma); //Calculating pixel's weight

    return w;
}


//
double Neighbothood_Average_PEF (double x_background, double x_foreground, double xbx, double xby,
                                 double xfx, double xfy, double xix, double xiy, int B, int F, Matrix2d L, VectorXd x)
{
    double Deltaxxb, Deltaxyb,xxb_Pow2, xyb_Pow2,x_b_temp, Deltaxxf, Deltaxyf, xxf_Pow2, xyf_Pow2, x_f_temp;
    Matrix2d LaplacianC_energy;

    // Define k*
    int k1, k2, k3;

    //Calculating sum for x_background where x_background = (|xi-xb|)^2
    // ***I need to update Ixix, Ixbx .... for every position of the pixels

    for (int i=0; i <= B; i++)
    {
        Deltaxxb = xix - xbx;
        Deltaxyb = xiy - xby;
        xxb_Pow2 = qPow(Deltaxxb,2);
        xyb_Pow2 = qPow(Deltaxyb,2);
        x_b_temp = xxb_Pow2 + xyb_Pow2;
        x_background += x_b_temp;

      }

    //Calculating sum for x_foreground where x_foreground = (|xi-xf|)^2

    for (int j=x_foreground; j <= F; j++)
    {
        Deltaxxf = xix - xfx;
        Deltaxyf = xiy - xfy;
        xxf_Pow2 = qPow(Deltaxxf,2);
        xyf_Pow2 = qPow(Deltaxyf,2);
        x_f_temp = xxf_Pow2 + xyf_Pow2;
        x_foreground += x_f_temp;

    }

    LaplacianC_energy = qPow(L*x,2);

    PEF = k1* x_background+ k2* x_foreground + k3*LaplacianC_energy;

    return PEF;

}


//
double Minimizing_Energy_Functional (MatrixXd Is, MatrixXd L, VectorXd x)
{
    MatrixXd L2, Is_L, b;
    L2 = qPow(L,2);
    Is_L= Is+ L;
    b = (Is_L)*x;

    return b;
}


// Main function
int main( int argc, char** argv )
{
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
                        float wij = exp( - betta * pow( double( maximumOfThree( abs( chR( i , j ) - chR( a , b ) ),
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

    // b
    // Is
    // x



    VectorXd X( m * n );

    /*for (int j = 0 ; j < m * n / 2 ; j++ ) {          //just for test
        X(j) = 1;
    }

    for (int j = m * n / 2 ; j < m * n ; j++ ) {
        X(j) = 0;
    }*/


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
