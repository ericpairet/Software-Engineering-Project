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

/**
 * @brief
 *
 */
class CSegmentation : public QObject {
    Q_OBJECT

public:
    /**
     * @brief
     *
     * @param _t
     * @param _m
     */
    CSegmentation( CToolsWidget *_t , CMonitorWidget *_m );
    /**
     * @brief
     *
     */
    ~CSegmentation();

public slots:
    /**
     * @brief
     *
     * @param image
     */
    void setInputImage( QImage image );
    /**
     * @brief
     *
     */
    void run();
    /**
     * @brief
     *
     * @param _val
     */
    void setBetha( int _val);

private:
    CToolsWidget *tools; /**< TODO */
    CMonitorWidget *monitor; /**< TODO */
    cv::Mat *inputImage; /**< TODO */
    QPixmap q; /**< TODO */
    double betta; /**< TODO */
    /**
     * @brief
     *
     * @param I
     * @param betta
     * @param sigma
     * @param L
     */
    void GraphLaplacianMatrix( const Mat &I , const double &betta , const double &sigma , SparseMatrix<double> &L );
    /**
     * @brief
     *
     * @param Is
     */
    void SeedsDependentMatrixIs( SparseMatrix<double> &Is );
    /**
     * @brief
     *
     * @param xf
     * @param xb
     * @param b
     * @param seed
     */
    void SeedsDependentVectorb( const int &xf , const int &xb , VectorXd &b, QString seed);
    /**
     * @brief
     *
     * @param L
     * @return SparseMatrix<double>
     */
    inline SparseMatrix<double> GraphLaplacianMatrixSquare( SparseMatrix<double> &L ) { return ( L * L ); }
    /**
     * @brief
     *
     * @param Is_L
     * @param b
     * @param X
     */
    void ComputeLinearSystem( const SparseMatrix<double> &Is_L , const VectorXd &b , VectorXd &X );
    /**
     * @brief
     *
     * @param m
     * @param n
     * @param xf
     * @param xb
     * @param X
     * @param Y
     */
    void AssignLabels( const int &m , const int &n , const double &xf , const double &xb , const VectorXd &X , Mat_<float> &Y );

signals:
    /**
     * @brief
     *
     * @param QPixmap
     * @param bool
     */
    void sendImage( QPixmap, bool);
};

#endif // CSEGMENTATION_H
