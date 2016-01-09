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
#include <unsupported/Eigen/SparseExtra>

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"

#include <gtest/gtest.h>
#include <fstream>

using namespace std;
using namespace Eigen;
using namespace cv;

template<class TT> class sparseHash;

template<>
class sparseHash<Eigen::SparseMatrix<double> > {
public:
    size_t operator()(const SparseMatrix<double> &m) const
    {
        stringstream ss;
        for (int k=0; k<m.outerSize(); ++k)
            for (SparseMatrix<double>::InnerIterator it(m,k); it; ++it)
            {
                ss << it.value() << it.row() << it.col() << it.index() << endl;
            }
        string s = ss.str();
        return std::hash<std::string>()(s);
    }
};


template<class T> class vectorHash;

template<>
class vectorHash<VectorXd> {
public:
    size_t operator()(const VectorXd &v) const
    {
        stringstream ss;
        ss << v;
        string s = ss.str();
        return std::hash<std::string>()(s);
    }
};

/**
 * @brief Class in charge of the mathematical procedures to follow
 * the implementation of the Laplacian Coordinates segmentation
 * paper.
 *
 */
class CSegmentation : public QObject {
    Q_OBJECT

public:
    /**
     * @brief CSegmentation : function in charge of pulling variables
     * to the monitor so that they can be handled by the user
     *
     * @param _m : handles the monitor to pull data into the GUI
     */
    CSegmentation(CMonitorWidget *_m );
    /**
     * @brief Destructor member function for CSegmentation
     *
     */
    ~CSegmentation();

public slots:
    /**
     * @brief setInputImage : function in charge of getting the selected
     * image to convert it into matrix using opencv library Mat
     *
     * @param image : Input image selected by the user
     */
    void setInputImage( QImage image );
    /**
     * @brief run(): Where the magic happens
     *
     */
    void run();
    /**
     * @brief setBetha : function in charge of converting the parameter
     * obtained by the slider (input of the user) in a valuable usable
     * for the segmentation code
     *
     * @param _val : slider value (GUI user input)
     */
    void setBetha( int _val);

private:
    FRIEND_TEST( CSegmenterGTester, graphLapMat);
    FRIEND_TEST( CSegmenterGTester, seedDepVec);
    CToolsWidget *tools; /**< TODO */
    CMonitorWidget *monitor; /**< TODO */
    cv::Mat *inputImage; /**< TODO */
    QPixmap q; /**< TODO */
    double betta; /**< TODO */
    /**
     * @brief GraphLaplacianMatrix : Function to store the graph Laplacian
     * matrix and the Diagonal matrix
     *
     * @param I : input image matrix
     * @param betta : variable betta, tunning constant set by the user
     * @param sigma : variable sigma, set it to 0.1 (paper suggestion)
     * @param L : graph laplacian matrix
     */
    //    static SparseMatrix<double>* GraphLaplacianMatrix( const Mat &I , const double &betta , const double &sigma);
    size_t GraphLaplacianMatrix( const Mat &I , const double &betta , const double &sigma, SparseMatrix<double> &L, bool testing = false);
    /**
     * @brief SeedsDependentMatrixIs : function to calculate Is
     * diagonal matrix (for minimizing the energy functional)
     *
     * @param Is : Is diagonal matrix, Is(i,i)=1
     */
    size_t SeedsDependentMatrixIs( SparseMatrix<double> &Is , bool testing = false);
    /**
     * @brief SeedsDependentVectorb : function to calculate vector b
     * (for minimizing the energy functional)
     *
     * @param xf :foreground pixels label after segmentation
     * @param xb : background pixels label after segmentation
     * @param b : vector b, b(i) = xb if x belongs to B or b(i) = xf f x belongs to F
     * @param seed : seeds input by the user
     */
    size_t SeedsDependentVectorb( const int &xf , const int &xb , VectorXd &b, QString seed, bool testing = false);
    /**
     * @brief SparseMatrix : inline function to improve execution time on the calculation
     * of the graph Laplacian Matrix square (L^2)
     *
     * @param L : graph laplacian matrix
     * @return SparseMatrix<double> : square laplacian matrix
     */
    inline SparseMatrix<double> GraphLaplacianMatrixSquare( SparseMatrix<double> &L ) { return ( L * L ); }
    /**
     * @brief ComputeLinearSystem : function calculating Is + L^2
     * which should be symetric and positive
     *
     * @param Is_L : linear system Is + Ls^2
     * @param b : vector b
     * @return X : solution vector x
     */
    size_t ComputeLinearSystem(const SparseMatrix<double> &Is_L , const VectorXd &b, VectorXd &X, bool testing = false);
    /**
     * @brief AssignLabels : function that helps labeling the pixels
     * for background or foreground labels. It shapes from vector to
     * matrix and changes from eigen to opencv
     *
     * @param m : m rows of matrix
     * @param n : n columns of matrix
     * @param xf : foreground labels
     * @param xb : background labels
     * @param X : cholesky factorization of spare matrix
     * @param Y : output opencv matrix of input image
     */
    void AssignLabels( const int &m , const int &n , const double &xf , const double &xb , const VectorXd &X , Mat_<float> &Y );

signals:
    /**
     * @brief sendImage : signal that sends the output segmented to the
     * GUI
     *
     * @param QPixmap : q containing the QPixmap of the image
     * @param bool : either true or false
     */
    void sendImage( QPixmap, bool);
};

#endif // CSEGMENTATION_H
