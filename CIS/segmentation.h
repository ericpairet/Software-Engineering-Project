#ifndef CSEGMENTATION_H
#define CSEGMENTATION_H

#include "widgets.h"

#include <ctime>
#include <iostream>

#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/Sparse>
#include <Eigen/SparseCore>
#include <Eigen/SparseCholesky>

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"

using namespace std;
using namespace Eigen;
using namespace cv;

class CSegmentation : public QObject
{
    Q_OBJECT

public:

    CSegmentation( CToolsWidget *_t, CMonitorWidget *_m ) : QObject() {
        tools = _t;
        monitor = _m;
    }
    ~CSegmentation() {}

public slots:

    void run() {

        //All this should be moved out from this CLASS
// *****************************************************************************************************************************************************************
        // Load image to be computed ( m x n )
        // Mat I = QPixmapToCvMat( *image);
        Mat I = imread( tools->imagePath.toStdString().c_str() , CV_32FC3 );
        //I = img2grey(I);

        // Take m and n image size
        //int m = I.rows;
        //int n = I.cols;

        // Check if there is an image
        /*if ( ( m * n ) == 0 ) {
            cout << "No image" << endl;
            return;
        }*/

        // Initialize tunning constants
        double betta = 0.005;
        double sigma = 0.1;
// *****************************************************************************************************************************************************************

        // Save segmentation start time
        const clock_t t_start = clock();

        // Take m and n image size
        int m = I.rows;
        int n = I.cols;

        // Initialize background and foreground labels
        double xb = 1;
        double xf = 0;

        // Check xb > xf                                            // if xb and xf are hardcoded this condition can be removed
        if ( xb < xf ) {
            cout << "Background level must be greater than the foreground one" << endl;
            return;
        }

        // Compute the Graph Laplacian Matrix (L)
        SparseMatrix<double> L( m * n , m * n );
        L.reserve( VectorXi::Constant( m * n , 8 ) );
        GraphLaplacianMatrix( I , betta , sigma , L );

        // Compute seeds dependent matrices (Is , b)
        SparseMatrix<double> Is( m * n , m * n );
        VectorXd b( m * n );
        b = VectorXd::Zero( m * n );
        SeedsDependentMatrices( xf , xb , Is , b );

        // Compute the Graph Laplacian Matrix square
        SparseMatrix<double> L2( m * n , m * n );
        L2.reserve( VectorXi::Constant( m * n , 8 ) );
        L2 = GraphLaplacianMatrixSquare( L );

        const clock_t begin_time2 = clock();
        // Solve linear system
        VectorXd X( m * n );
        ComputeLinearSystem( Is + L , b , X );
        cout << "Ax = b took " << float( clock () - begin_time2 ) /  CLOCKS_PER_SEC << endl;

        // Assign labels to x
        Mat_<float> Y = Mat_<float>::zeros( m , n );
        AssignLabels( m , n , xf , xb , X , Y );

        // Print time tooken for segmentation process
        cout << "Segmentation process took " << float( clock () - t_start ) /  CLOCKS_PER_SEC << endl;

        //All this should be moved out from this CLASS
// *****************************************************************************************************************************************************************
        // Show the segmented image
        //namedWindow( "SI" , CV_WINDOW_NORMAL );
        imshow( "SI" , Y );
        //cout << "Y.cols" << Y.cols << endl;
        //cout << "Y.rows" << Y.rows << endl;
        //QPixmap dest= QPixmap((uchar*) Y.data, Y.cols, Y.rows, Y.step, QPixmap::Format_RGB888);
        QPixmap q = QPixmap::fromImage( QImage( ( unsigned char* ) Y.data , Y.cols , Y.rows, /*QImage::Format_Grayscale8)); */ QImage::Format_RGB32));
        //q = q.scaledToWidth(1);
        // label->setPixmap( q );
        //cout << "q.width" << q.width << endl;
        //cout << "q.height" << q.height << endl;
        emit sendImage(q);
// *****************************************************************************************************************************************************************
    }

private:
    CToolsWidget *tools;
    CMonitorWidget *monitor;

    void GraphLaplacianMatrix( const Mat &I , const double &betta , const double &sigma , SparseMatrix<double> &L ) {
        // Take m and n image size
        int m = I.rows;
        int n = I.cols;

        // Get the R, G, B channels in BGR order
        vector<Mat> channels(3);
        split(I,channels);
        Mat_<double> chR = channels[2];
        Mat_<double> chG = channels[1];
        Mat_<double> chB = channels[0];

        // Initialize adjency matrix W with zeros
        SparseMatrix<double> W( m * n , m * n );
        W.reserve( VectorXi::Constant( m * n , 8 ) );                                             // 8 for normal neighborhood

        // Initialize diagonal matrix D with zeros
        SparseMatrix<double> D( m * n , m * n );
        D.reserve( VectorXi::Constant( m * n , 1 ) );

        // For each row                                                                           // --> TODO: Optimize the code changing the way to compute D
        for ( int i = 0 ; i < m ; i++ ) {                                                         //           (avoid compute twice the same W -> uncomment condition in below if)
            // For each column
            for ( int j = 0 ; j < n ; j ++ ) {

                // Variable to store the sum of dii
                double tmp = 0;

                // For each row of the neighborhood
                for ( int a = i - 1 ; a <= i + 1 ; a++ ) {
                    // For each column of the neighborhood
                    for ( int b = j - 1 ; b <= j + 1 ; b++ ) {
                        // Avoid compute edge with itself, values out the matrix and recompute edges
                        if ( !( ( a == i ) and ( b == j ) ) and ( a >= 0 ) and ( a < m ) and ( b >= 0 ) and ( b < n ) ) { //and ( W( i * n + j , a * n + b ) == 0 ) ) {

                            // Compute wij ( Pi is defined by i and j, Pj is defined by a and b )
                            double wij = exp( - betta * pow( CMaths::maximumOfThree( abs( chR( i , j ) - chR( a , b ) ),
                                                                                     abs( chG( i , j ) - chG( a , b ) ),
                                                                                     abs( chB( i , j ) - chB( a , b ) ) ) , 2 ) / sigma ) + 10e-6;


                            // Store the result at W in wij and wji
                            W.coeffRef( i * n + j , a * n + b ) = wij;
                            W.coeffRef( a * n + b , i * n + j ) = wij;

                            // Compute dii
                            tmp += wij;
                        }
                    }
                }
                // Save final dii
                D.coeffRef( i * n + j , i * n + j ) = tmp;
            }
        }

        // Compute L matrix                                                                       // --> TODO: Try to do in the same loop as D and W?
        L = D - W;
    }

    void SeedsDependentMatrices( const int &xf , const int &xb , SparseMatrix<double> &Is , VectorXd &b ) {
        // Compute Is matrix and b vector                                                         // --> TODO: Apply OR gate? It can avoid go through all the seeds
        for( QSet< QPair< int, int> >::Iterator it = monitor->fgSeeds.begin(); it != monitor->fgSeeds.end(); ++it) {
            Is.coeffRef( it->first + it->second * monitor->image->width() , it->first + it->second * monitor->image->width() ) = 1;
            b( it->first + it->second * monitor->image->width() ) = xf;
        }
        for( QSet< QPair< int, int> >::Iterator it = monitor->bgSeeds.begin(); it != monitor->bgSeeds.end(); ++it) {
            Is.coeffRef( it->first + it->second * monitor->image->width() , it->first + it->second * monitor->image->width() ) = 1;
            b( it->first + it->second * monitor->image->width() ) = xb;
        }
    }

    SparseMatrix<double> GraphLaplacianMatrixSquare( SparseMatrix<double> &L ) {
        return ( L * L );
    }

    void ComputeLinearSystem( const SparseMatrix<double> &Is_L , const VectorXd &b , VectorXd &X ) {
        SimplicialCholesky< SparseMatrix<double> > chol( Is_L );
        X = chol.solve( b );
    }

    void AssignLabels( const int &m , const int &n , const double &xf , const double &xb , const VectorXd &X , Mat_<float> &Y ) {
        // Threshold to assign labels
        double threshold = ( ( xb + xf ) / 2 );

        // Variable used to reshape from vector to matrix
        int r = 0;

        // Assign labels, reshape from vector to matrix and change from eigen to opencv
        for ( int i = 0 ; i < m * n ; i++ ) {

            if ( ( i - r * n ) == n ) {
                r++;
            }

            if ( X( i ) >= threshold ) {
                Y( r , i - r * n ) = float(xb);
            }
            else {                                                                          // Depends on the value of xf this can be avoided
                Y( r , i - r * n ) = float(xf);
            }
        }
    }

signals:

    void sendImage( QPixmap );
};

#endif // CSEGMENTATION_H
