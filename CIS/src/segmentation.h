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
/**
 * @brief Sparce Matrix hash calculator : this template class is generated to convert
 * openCV sparse matrices to a data structure that std hash calculator has calculate
 * the hash of the object for that.
 * Hashes being used for comparing two matrix together instead of comparing elementwise
 * in Google Test unit tests.
 *
 */
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
/**
 * @brief Vector hash calculator : this template class is generated to convert
 * openCV vectors to a data structure that std hash calculator has calculate
 * the hash of the object for that
 * Hashes being used for comparing two vector together instead of comparing elementwise
 * in Google Test unit tests.
 *
 */
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
 * @brief CSegmentation : Class in charge of the mathematical procedures to follow
 * the implementation of the Laplacian Coordinates segmentation
 * paper. The class is divided into functions that calculate the different parameters
 * needed to follow the equations for seeded segmentation. For example:
 * GraphLaplacianMatrix : to calculate L;
 * SeedsDependentMatrixIs : to calculate Is;
 * SeedsDependentVectorb : to calculate vector b;
 * SparseMatrix : to calculate L^2;
 * ComputeLinearSystem : to calculate Is +L^2 using Simplicial Cholesky;
 *
 *
 */
class CSegmentation : public QObject {
    Q_OBJECT

public:
    /**
     * @brief CSegmentation : The class cunstructor
     *
     * @param _m : a pointer of CMonitorWidget class is being passed to this class
     * which contains the seeds and input image
     */
    CSegmentation(CMonitorWidget *_m );
    /**
     * @brief Destructor member function for CSegmentation
     *
     */
    ~CSegmentation();

public slots:
    /**
     * @brief setInputImage : slot in charge of getting the selected
     * image to convert it into matrix using opencv library Mat
     *
     * @param image : Input image selected by the user
     */
    void setInputImage( QImage image );
    /**
     * @brief run(): Where the magic happens. the main loop of segmentation proccess wich
     * is being run by segmentation thread. It uses the functions containing the
     * calculations of the different parameters for the neighborhood average preserving
     * energy and minimizing energy funtional equations to get the final Y image.Y image
     * is the resulting segmented region selected by the foreground seeds. This function
     * emits the signal sendImage so that it can be updated in the GUI.
     * Note: go to widgets.h to see the functionality of updating the images in the GUI.
     *
     */
    void run();
    /**
     * @brief setBetha : slot in charge of converting the parameter
     * obtained by the slider (input of the user) into a useful value
     * for the segmentation code. In case the user does not select this value
     * the default beta is set to 0.005.
     *
     * @param _val : slider value (GUI user input)
     */
    void setBetha( int _val);

private:
    // Making friendship between Google Test and this class so that GTest will have access to this class private functions
    FRIEND_TEST( CSegmenterGTester, graphLapMat);
    FRIEND_TEST( CSegmenterGTester, seedDepMat);
    FRIEND_TEST( CSegmenterGTester, seedDepVec);
    FRIEND_TEST( CSegmenterGTester, l2Mat);
    FRIEND_TEST( CSegmenterGTester, systemSolver);
    CMonitorWidget *monitor; /**< Monitor widget pointer that receives a pointer to main monitor widget instance */
    cv::Mat *inputImage; /**< Input image converted to openCV matrix */
    QPixmap outputImage; /**< Output image converted from openCV matrix to QPixmap */
    double betta; /**< The betta value used in segmentation */
    /**
     * @brief GraphLaplacianMatrix : Function to store the graph Laplacian
     * matrix L and the Diagonal matrix dii.
     *
     * @param I : input image matrix
     * @param betta : variable betta, tunning constant set by the user
     * @param sigma : variable sigma, set it to 0.1 (paper suggestion)
     * @param L : graph laplacian matrix
     * @return : Hash of the result being returned in case of testing
     */
    //    static SparseMatrix<double>* GraphLaplacianMatrix( const Mat &I , const double &betta , const double &sigma);
    size_t GraphLaplacianMatrix( const Mat &I , const double &betta , const double &sigma, SparseMatrix<double> &L, bool testing = false);
    /**
     * @brief SeedsDependentMatrixIs : function to calculate Is
     * diagonal matrix (for minimizing the energy functional).
     *
     * @param Is : Is diagonal matrix, Is(i,i)=1
     * @return : Hash of the result being returned in case of testing
     */
    size_t SeedsDependentMatrixIs( SparseMatrix<double> &Is , bool testing = false);
    /**
     * @brief SeedsDependentVectorb : function to calculate vector b
     * (for minimizing the energy functional).
     *
     * @param xf :foreground pixels label after segmentation
     * @param xb : background pixels label after segmentation
     * @param b : vector b, b(i) = xb if x belongs to B or b(i) = xf f x belongs to F
     * @param seed : seeds input by the user
     * @return : Hash of the result being returned in case of testing
     */
    size_t SeedsDependentVectorb(const double &xf , const double &xb , VectorXd &b, QString seed, bool testing = false);
    /**
     * @brief SparseMatrix : inline function to improve execution time on the calculation
     * of the graph Laplacian Matrix square (L^2).
     *
     * @param L : graph laplacian matrix
     * @param SparseMatrix<double> : square laplacian matrix
     * @return : Hash of the result being returned in case of testing
     */
     size_t GraphLaplacianMatrixSquare( const SparseMatrix<double> &L, SparseMatrix<double> &L2, bool testing = false);
    /**
     * @brief ComputeLinearSystem : static vector function calculating Is + L^2
     * which should be symetric and positive. It useds the method of Simplicial Cholesky for
     * the calculation.
     *
     * @param Is_L : linear system Is + Ls^2
     * @param b : vector b
     * @param X : solution vector x
     * @return : Hash of the result being returned in case of testing
     */
    size_t ComputeLinearSystem(const SparseMatrix<double> &Is_L , const VectorXd &b, VectorXd &X, bool testing = false);
    /**
     * @brief AssignLabels : function that helps labeling the pixels
     * for background or foreground labels. It shapes from vector to
     * matrix and changes from eigen to opencv.
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
     * @brief sendImage : signal that sends the segmented region (output Y of run())
     * to be shown in the GUI.
     *
     * @param QPixmap : q containing the QPixmap of the image
     * @param bool : either true or false
     */
    void sendImage( QPixmap, bool);
};

#endif // CSEGMENTATION_H
