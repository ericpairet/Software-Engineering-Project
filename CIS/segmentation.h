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
    CSegmentation( CToolsWidget *_t, CMonitorWidget *_m );
    ~CSegmentation();

public slots:
    void setInputImage(QImage image);
    void run() {

        //All this should be moved out from this CLASS
// *****************************************************************************************************************************************************************
        // Load image to be computed ( m x n )
        // Mat I = QPixmapToCvMat( *image);
        Mat *inputImage = new Mat(imread( tools->imagePath.toStdString().c_str() , CV_32FC3 ));
//        imshow("hey", *inputImage);
//        double min, max;
//        cv::minMaxLoc(*inputImage, &min, &max);
//        qDebug() << "Here " << min << max;
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
        int m = inputImage->rows;
        int n = inputImage->cols;

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
        GraphLaplacianMatrix( *inputImage , betta , sigma , L );

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
        imshow("Out",Y);
        //cout << "Y.cols" << Y.cols << endl;
        //cout << "Y.rows" << Y.rows << endl;
        //QPixmap dest= QPixmap((uchar*) Y.data, Y.cols, Y.rows, Y.step, QPixmap::Format_RGB888);
        QPixmap q = QPixmap::fromImage( QImage( ( unsigned char* ) Y.data , Y.cols , Y.rows, QImage::Format_RGB32));
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
    cv::Mat *inputImage;
    void GraphLaplacianMatrix( const Mat &I , const double &betta , const double &sigma , SparseMatrix<double> &L );
    void SeedsDependentMatrices( const int &xf , const int &xb , SparseMatrix<double> &Is , VectorXd &b );
    inline SparseMatrix<double> GraphLaplacianMatrixSquare( SparseMatrix<double> &L ) { return ( L * L );}
    void ComputeLinearSystem( const SparseMatrix<double> &Is_L , const VectorXd &b , VectorXd &X );
    void AssignLabels( const int &m , const int &n , const double &xf , const double &xb , const VectorXd &X , Mat_<float> &Y );
signals:
    void sendImage( QPixmap );
};

#endif // CSEGMENTATION_H
