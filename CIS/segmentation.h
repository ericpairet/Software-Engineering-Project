#ifndef CSEGMENTATION_H
#define CSEGMENTATION_H

#include "widgets.h"
#include <iostream>
#include <Eigen/Dense>
#include <Eigen/LU>

//#include <Eigen/Cholesky>
#include <Eigen/Core>

#include <Eigen/SparseCholesky>
#include <Eigen/SparseCore>

#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"

// only for debugging purpouses                             //***
#include <ctime>

//typedef Eigen::SparseMatrix<double> SpMat; // declares a column-major sparse matrix type of double

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
    void GraphLaplacianMatrix( const cv::Mat& , const double& , const double& , Eigen::SparseMatrix<double>& );
    void SeedsDependentMatrices( const int& , const int& , const int& , const int& , Eigen::SparseMatrix<double>& , Eigen::VectorXd& );
    void GraphLaplacianMatrixSquare( const int& , const int& , Eigen::SparseMatrix<double>& );
    void ComputeLinearSystem( const int& , const int& , const Eigen::SparseMatrix<double>& , const Eigen::VectorXd& , Eigen::VectorXd& );
    void AssignLabels( const int& , const int& , const int& , const int& , const Eigen::VectorXd& , cv::Mat_<double>& );

signals:
    void sendImage( QPixmap );
};

#endif // CSEGMENTATION_H
