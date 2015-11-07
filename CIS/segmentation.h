#ifndef CSEGMENTATION_H
#define CSEGMENTATION_H

#include "widgets.h"
#include <iostream>
#include <Eigen/Dense>
#include <Eigen/LU>

#include <Eigen/Cholesky>
#include <Eigen/Core>

#include <Eigen/Cholesky>
#include <Eigen/SparseCholesky>         // just for test
#include <Eigen/SparseCore>

#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"

// only for debugging purpouses
#include <ctime>

typedef Eigen::SparseMatrix<double> SpMat; // declares a column-major sparse matrix type of double      // just for test

class CSegmentation : public QObject
{
    Q_OBJECT
public:
    CSegmentation(CToolsWidget *_t, CMonitorWidget *_m);
    ~CSegmentation();

public slots:
    void run();

private:
    CToolsWidget *tools;
    CMonitorWidget *monitor;
    void GraphLaplacianMatrix( const cv::Mat& , const double& , const double& , Eigen::MatrixXd& );
    void SeedsDependentMatrices( const int& , const int& , const int& , const int& , Eigen::MatrixXd& , Eigen::VectorXd& );
    void GraphLaplacianMatrixSquare( const int& , const int& , Eigen::MatrixXd& );
    void ComputeLinearSystem( const int& , const int& , const Eigen::MatrixXd& , const Eigen::VectorXd& , Eigen::VectorXd& );
    void AssignLabels( const int& , const int& , const int& , const int& , const Eigen::VectorXd& , cv::Mat_<double>& );

signals:
    void sendImage( QPixmap );
};

#endif // CSEGMENTATION_H
