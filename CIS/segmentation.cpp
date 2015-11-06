#include "segmentation.h"

using namespace std;
using namespace Eigen;
using namespace cv;

CSegmentation::CSegmentation( CToolsWidget *_t, CMonitorWidget *_m) : QObject()
{
    tools = _t;
    monitor = _m;
}

CSegmentation::~CSegmentation()
{

}

Mat img2grey(Mat img) {
    Mat dst;
    normalize(img, dst, 0.0, 1.0, NORM_MINMAX, CV_32FC3);
    return dst;
}

MatrixXf CSegmentation::GraphLaplacianMatrix( const Mat &I , const float &betta , const float &sigma ) {
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
    MatrixXf W( m * n , m * n );
    W = MatrixXf::Zero( m * n , m * n );

    // Initialize diagonal matrix D with zeros
    MatrixXf D( m * n , m * n );
    D = MatrixXf::Zero( m * n , m * n );

    // For each row                                                                           // --> TODO: Optimize the code changing the way to compute D
    for ( int i = 0 ; i < m ; i++ ) {                                                         //           (avoid compute twice the same W -> uncomment condition in below if)
        // For each column
        for ( int j = 0 ; j < n ; j ++ ) {

            // Variable to store the sum of dii
            float tmp = 0;

            // For each row of the neighborhood
            for ( int a = i - 1 ; a <= i + 1 ; a++ ) {
                // For each column of the neighborhood
                for ( int b = j - 1 ; b <= j + 1 ; b++ ) {
                    // Avoid compute edge with itself, values out the matrix and recompute edges
                    if ( !( ( a == i ) and ( b == j ) ) and ( a >= 0 ) and ( a < m ) and ( b >= 0 ) and ( b < n ) ) { //and ( W( i * n + j , a * n + b ) == 0 ) ) {

                        // Compute wij ( Pi is defined by i and j, Pj is defined by a and b )
                        float wij = exp( - betta * pow( CMaths::maximumOfThree( abs( chR( i , j ) - chR( a , b ) ),
                                                                                abs( chG( i , j ) - chG( a , b ) ),
                                                                                abs( chB( i , j ) - chB( a , b ) ) ) , 2 ) / sigma ) + 10e-6;


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

    // Compute L matrix                                                                       // --> TODO: Try to do in the same loop as D and W?
    //MatrixXf L( m * n , m * n );
    return ( D - W );
}

void CSegmentation::SeedsDependentMatrices( const int &m , const int &n , const int &xf , const int &xb , MatrixXf &Is , VectorXf &b ) {

    // Initialize Is and b with zeros
    Is = MatrixXf::Zero( m * n , m * n );
    b = VectorXf::Zero( m * n );

    // Compute Is matrix and b vector                                                         // --> TODO: Apply OR gate? It can avoid go through all the seeds
    for( QSet< QPair< int, int> >::Iterator it = monitor->fgSeeds.begin(); it != monitor->fgSeeds.end(); ++it) {
        Is( it->first + it->second * monitor->image->width() , it->first + it->second * monitor->image->width() ) = 1;
        b( it->first + it->second * monitor->image->width() ) = xf;
    }
    for( QSet< QPair< int, int> >::Iterator it = monitor->bgSeeds.begin(); it != monitor->bgSeeds.end(); ++it) {
        Is( it->first + it->second * monitor->image->width() , it->first + it->second * monitor->image->width() ) = 1;
        b( it->first + it->second * monitor->image->width() ) = xb;
    }
}

void CSegmentation::GraphLaplacianMatrixSquare( const int &m , const int &n , MatrixXf &L ) {
    Eigen::SparseMatrix<float> L_sparse( m * n , m * n );
    Eigen::SparseMatrix<float> L2_sparse_test( m * n , m * n );
    L_sparse = L.sparseView();      // Check sparse = dense.sparseView(epsilon,reference) for better results?
    L2_sparse_test = L_sparse * L_sparse;
    L = MatrixXf(L2_sparse_test);
}

void CSegmentation::ComputeLinearSystem( const MatrixXf &Is_L , const VectorXf &b , VectorXf &X ) {
    /*
        // OPTION 1
        X = Is_L.inverse() * b;
    */

    /*
        // OPTION 2
        X = b;
        LDLT<MatrixXf> ldlt;
        ldlt.compute(Is_L);
        ldlt.solveInPlace(X);
    */

        // OPTION 3
        X = Is_L.ldlt().solve(b);

    /*
        // OPTION 4     -->     Sparse matrix
        SparseMatrix<float> A( m * n , m * n );
        A = Is_L.sparseView();      // Check sparse = dense.sparseView(epsilon,reference) for better results?

        // Solving:
        SimplicialCholesky<SpMat> chol(A);  // performs a Cholesky factorization of A
        X = chol.solve(b);         // use the factorization to solve for the given right hand side
    */
}

void CSegmentation::run()
{
    // Variables only for performance purposes
    time_t tstart, tend;

    // Load image to be computed (mxn)
    // Mat I = QPixmapToCvMat( *image);
    Mat I = imread( tools->imagePath.toStdString().c_str() , CV_32FC3 );
    //I = img2grey(I);

    // Take m and n image size
    int m = I.rows;
    int n = I.cols;

    // Check if there is an image
    if ( ( m * n ) == 0 ) {
        cout << "No image" << endl;
        return;
    }

    // Initialize background and foreground labels (do not change the type!)
    float xb = 1;
    float xf = 0;

    // Check xb > xf
    if ( xb < xf ) {
        cout << "Background level must be greater than the foreground one" << endl;
        return;
    }

    // Initialize tunning constants
    float betta = 0.005;
    float sigma = 0.1;

    // Compute the Graph Laplacian Matrix (L)
    MatrixXf L( m * n , m * n );
    L = GraphLaplacianMatrix( I, betta , sigma );

    // Compute seeds dependent matrices (Is , b)
    MatrixXf Is( m * n , m * n );
    VectorXf b( m * n );
    SeedsDependentMatrices( m , n, xf , xb , Is , b );

    tstart = time(0);                                                           // ***
    // Compute the Graph Laplacian Matrix square
    GraphLaplacianMatrixSquare( m , n , L );
    tend = time(0);                                                             // ***
    cout << "L^2 took "<< difftime(tend, tstart) <<" second(s)."<< endl;        // ***

    tstart = time(0);                                                           // ***
    // Solve linear system
    VectorXf X( m * n );
    ComputeLinearSystem( Is + L , b , X );
    tend = time(0);                                                             // ***
    cout << "Ax = B took "<< difftime(tend, tstart) <<" second(s)."<< endl;     // ***


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
        else {                          // Depends on the value of xf this can be avoided
            Y( r , i - r * n ) = xf;
        }
    }

    //**************************
    //
    // Show the segmented image
    //
    //**************************

    // Show the segmented image
    //namedWindow( "SI" , CV_WINDOW_NORMAL );
//    imshow( "SI" , Y );
    cout << "Y.cols" << Y.cols << endl;
    cout << "Y.rows" << Y.rows << endl;
    //QPixmap dest= QPixmap((uchar*) Y.data, Y.cols, Y.rows, Y.step, QPixmap::Format_RGB888);
    QPixmap q = QPixmap::fromImage(QImage((unsigned char*) Y.data,
                                      Y.cols,
                                      Y.rows,
                                      //QImage::Format_Grayscale8));
                                      QImage::Format_RGB32));
       //q = q.scaledToWidth(1);

      // label->setPixmap( q );

    //cout << "q.width" << q.width << endl;
    //cout << "q.height" << q.height << endl;
    emit sendImage(q);

/*
    QLabel myLabel;
        myLabel.setPixmap(QPixmap::fromImage(QImage((unsigned char*) Y.data,
                                                    Y.cols,
                                                    Y.rows,
                                                    QImage::Format_RGB32)));

        myLabel.show();
*/
}
