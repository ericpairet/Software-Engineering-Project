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

void CSegmentation::GraphLaplacianMatrix( const Mat &I , const double &betta , const double &sigma , SparseMatrix<double> &L ) {
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
    SparseMatrix<double> W( m * n , m * n );
    W.reserve( VectorXi::Constant( m * n , 8 ) );                                             // Check reserved space for sparse matrices

    // Initialize diagonal matrix D with zeros
    SparseMatrix<double> D( m * n , m * n );
    D.reserve( VectorXi::Constant( m * n , 1 ) );

    // For each row                                                                           // --> TODO: Optimize the code changing the way to compute D
    for ( int i = 0 ; i < m ; i++ ) {                                                         //           (avoid compute twice the same W -> uncomment condition in below if)
        // For each column
        for ( int j = 0 ; j < n ; j ++ ) {

            // Variable to store the sum of dii
            double tmp = 0;

            // For each row of the neighborhood
            for ( int a = i - 1 ; a <= i + 1 ; a++ ) {
                // For each column of the neighborhood
                for ( int b = j - 1 ; b <= j + 1 ; b++ ) {
                    // Avoid compute edge with itself, values out the matrix and recompute edges
                    if ( !( ( a == i ) and ( b == j ) ) and ( a >= 0 ) and ( a < m ) and ( b >= 0 ) and ( b < n ) ) { //and ( W( i * n + j , a * n + b ) == 0 ) ) {

                        // Compute wij ( Pi is defined by i and j, Pj is defined by a and b )
                        double wij = exp( - betta * pow( CMaths::maximumOfThree( abs( chR( i , j ) - chR( a , b ) ),
                                                                                 abs( chG( i , j ) - chG( a , b ) ),
                                                                                 abs( chB( i , j ) - chB( a , b ) ) ) , 2 ) / sigma ) + 10e-6;


                        // Store the result at W in wij and wji
                        W.coeffRef( i * n + j , a * n + b ) = wij;
                        W.coeffRef( a * n + b , i * n + j ) = wij;

                        // Compute dii
                        tmp += wij;
                    }
                }
            }
            // Save final dii
            D.coeffRef( i * n + j , i * n + j ) = tmp;
        }
    }

    // Compute L matrix                                                                       // --> TODO: Try to do in the same loop as D and W?
    L = D - W;
}

void CSegmentation::SeedsDependentMatrices( const int &xf , const int &xb , SparseMatrix<double> &Is , VectorXd &b ) {
    // Compute Is matrix and b vector                                                         // --> TODO: Apply OR gate? It can avoid go through all the seeds
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
    SimplicialCholesky< SparseMatrix<double> > chol( Is_L );
    X = chol.solve( b );
}

void CSegmentation::AssignLabels( const int &m , const int &n , const double &xf , const double &xb , const VectorXd &X , Mat_<float> &Y ) {
    // Threshold to assign labels
    double threshold = ( ( xb + xf ) / 2 );

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
        else {                                                                          // Depending on the value of xf this can be avoided
            Y( r , i - r * n ) = float(xf);
        }
    }
}