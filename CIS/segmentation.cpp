#include "segmentation.h"

CSegmentation::CSegmentation( CToolsWidget *_t , CMonitorWidget *_m ) : QObject() {
    tools = _t;
    monitor = _m;
}

CSegmentation::~CSegmentation() {}

void CSegmentation::setInputImage( QImage image ) {
    Mat * I = new Mat( image.height() , image.width() , CV_8UC4 , image.bits() , image.bytesPerLine() );
    inputImage = new Mat( I->rows , I->cols , CV_32FC3 );
    cvtColor( *I , *inputImage , CV_RGBA2RGB );
}

void CSegmentation::run() {
    //All this should be moved out from this CLASS
// *****************************************************************************************************************************************************************
    // Initialize tunning constants
    double betta = 0.005;
    double sigma = 0.1;
// *****************************************************************************************************************************************************************

    // Save segmentation start time
    const clock_t t_start = clock();

    // Take m and n image size
    const int m = inputImage->rows;
    const int n = inputImage->cols;

    // Initialize background and foreground labels
    const double xb = 1;
    const double xf = 0;

    clock_t begin_time = clock();
    // Compute the Graph Laplacian Matrix (L)
    SparseMatrix<double> L( m * n , m * n );
    L.reserve( VectorXi::Constant( m * n , 8 ) );
    GraphLaplacianMatrix( *inputImage , betta , sigma , L );
    cout << "L took " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;

    begin_time = clock();
    // Compute seeds dependent matrices (Is , b)
    SparseMatrix<double> Is( m * n , m * n );
    VectorXd b( m * n );
    b = VectorXd::Zero( m * n );
    SeedsDependentMatrices( xf , xb , Is , b );
    cout << "Is and b took " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;

    begin_time = clock();
    // Compute the Graph Laplacian Matrix square
    SparseMatrix<double> L2( m * n , m * n );
    L2.reserve( VectorXi::Constant( m * n , 8 ) );
    L2 = GraphLaplacianMatrixSquare( L );
    cout << "L^2 took " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;

    begin_time = clock();
    // Solve linear system
    VectorXd X( m * n );
    ComputeLinearSystem( Is + L , b , X );
    cout << "Ax = b took " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;

    begin_time = clock();
    // Assign labels to x
    Mat_<float> Y = Mat_<float>::zeros( m , n );
    AssignLabels( m , n , xf , xb , X , Y );
    cout << "Y took " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;

    // Print time tooken for segmentation process
    cout << "Segmentation process took " << float( clock () - t_start ) /  CLOCKS_PER_SEC << endl;

    imshow("afawefw", Y);

    //All this should be moved out from this CLASS
// *****************************************************************************************************************************************************************
    // Show the segmented image
    QPixmap q = QPixmap::fromImage( QImage( ( unsigned char* ) Y.data , Y.cols , Y.rows, QImage::Format_RGB32 ) );
    emit sendImage(q);
// *****************************************************************************************************************************************************************
}

void CSegmentation::GraphLaplacianMatrix( const Mat &I , const double &betta , const double &sigma , SparseMatrix<double> &L ) {
    // Take m and n image size
    const int m = I.rows;
    const int n = I.cols;

    // Get the R, G, B channels in BGR order
    vector<Mat> channels( 3 );
    split( I , channels );
    const Mat_<double> chR = channels[2];
    const Mat_<double> chG = channels[1];
    const Mat_<double> chB = channels[0];

    // Initialize adjency matrix W with zeros
    SparseMatrix<double> W( m * n , m * n );
    W.reserve( VectorXi::Constant( m * n , 8 ) );                                             // Check reserved space for sparse matrices

    // Initialize diagonal matrix D with zeros
    SparseMatrix<double> D( m * n , m * n );
    D.reserve( VectorXi::Constant( m * n , 1 ) );

    // For each row
    for ( int i = 0 ; i < m ; i++ ) {
        // For each column
        for ( int j = 0 ; j < n ; j ++ ) {

            // Reset sum of dii
            double dii = 0;

            // For each row of the neighborhood
            for ( int a = i - 1 ; a <= i + 1 ; a++ ) {
                // For each column of the neighborhood
                for ( int b = j - 1 ; b <= j + 1 ; b++ ) {
                    // Avoid compute edge with itself, values out the matrix and recompute edges
                    if ( !( ( a == i ) and ( b == j ) ) and ( a >= 0 ) and ( a < m ) and ( b >= 0 ) and ( b < n ) ) {

                        // Compute wij if it is not already done
                        double wij = W.coeffRef( i * n + j , a * n + b );
                        if ( wij == 0 ) {
                            // Compute wij ( Pi is defined by i and j, Pj is defined by a and b )
                            wij = exp( - betta * pow( CMaths::maximumOfThree( abs( chR( i , j ) - chR( a , b ) ),
                                                                                     abs( chG( i , j ) - chG( a , b ) ),
                                                                                     abs( chB( i , j ) - chB( a , b ) ) ) , 2 ) / sigma ) + 10e-6;

                            // Store the result at W in wij and wji
                            W.coeffRef( i * n + j , a * n + b ) = wij;
                            W.coeffRef( a * n + b , i * n + j ) = wij;
                        }
                        // Compute dii
                        dii += wij;
                    }
                }
            }
            // Save final dii
            D.coeffRef( i * n + j , i * n + j ) = dii;
        }
    }
    // Compute L matrix
    L = D - W;
}

void CSegmentation::SeedsDependentMatrices( const int &xf , const int &xb , SparseMatrix<double> &Is , VectorXd &b ) {
    // Compute Is matrix and b vector
    for( QSet< QPair< int, int> >::Iterator it = monitor->fgSeeds.begin(); it != monitor->fgSeeds.end(); ++it) {
        Is.coeffRef( it->first + it->second * monitor->image->width() , it->first + it->second * monitor->image->width() ) = 1;
        b( it->first + it->second * monitor->image->width() ) = xf;
    }
    for( QSet< QPair< int, int> >::Iterator it = monitor->bgSeeds.begin(); it != monitor->bgSeeds.end(); ++it) {
        Is.coeffRef( it->first + it->second * monitor->image->width() , it->first + it->second * monitor->image->width() ) = 1;
        b( it->first + it->second * monitor->image->width() ) = xb;
    }
}

void CSegmentation::ComputeLinearSystem( const SparseMatrix<double> &Is_L , const VectorXd &b , VectorXd &X ) {
    //SimplicialCholesky< SparseMatrix<double>, Eigen::Symmetric > chol( Is_L );
    SimplicialCholesky< SparseMatrix<double> > chol( Is_L );
    X = chol.solve( b );
}

void CSegmentation::AssignLabels( const int &m , const int &n , const double &xf , const double &xb , const VectorXd &X , Mat_<float> &Y ) {
    // Threshold to assign labels
    const double threshold = ( ( xb + xf ) / 2 );

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
    }
}
