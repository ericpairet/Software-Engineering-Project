#ifndef CSEGMENTATION_H
#define CSEGMENTATION_H

#include "widgets.h"
#include <iostream>
#include <Eigen/Dense>
#include <Eigen/LU>

#include <Eigen/Cholesky>
#include <Eigen/Core>

#include <Eigen/SparseCholesky>         // just for test
#include <Eigen/SparseCore>

#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"

// only for debugging purpouses
#include <ctime>

typedef Eigen::SparseMatrix<float> SpMat; // declares a column-major sparse matrix type of double      // just for test

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
    Eigen::MatrixXf GraphLaplacianMatrix( const cv::Mat& , const float& , const float& );
    void SeedsDependentMatrices( const int& , const int& , const int& , const int& , Eigen::MatrixXf& , Eigen::VectorXf& );

signals:
    void sendImage( QPixmap );
};

#endif // CSEGMENTATION_H
