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

class CSegmentation : public QObject {
    Q_OBJECT

public:
    CSegmentation( CToolsWidget *_t , CMonitorWidget *_m );
    ~CSegmentation();

public slots:
    void setInputImage( QImage image );
    void run();

private:
    CToolsWidget *tools;
    CMonitorWidget *monitor;
    cv::Mat *inputImage;
    void GraphLaplacianMatrix( const Mat &I , const double &betta , const double &sigma , SparseMatrix<double> &L );
    void SeedsDependentMatrices( const int &xf , const int &xb , SparseMatrix<double> &Is , VectorXd &b );
    inline SparseMatrix<double> GraphLaplacianMatrixSquare( SparseMatrix<double> &L ) { return ( L * L ); }
    void ComputeLinearSystem( const SparseMatrix<double> &Is_L , const VectorXd &b , VectorXd &X );
    void AssignLabels( const int &m , const int &n , const double &xf , const double &xb , const VectorXd &X , Mat_<float> &Y );

signals:
    void sendImage( QPixmap );
};

#endif // CSEGMENTATION_H
