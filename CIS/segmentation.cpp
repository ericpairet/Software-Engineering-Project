#include "segmentation.h"

CSegmentation::CSegmentation( CToolsWidget *_t , CMonitorWidget *_m ) : QObject() {
    tools = _t;
    monitor = _m;
    betta = 0.005;
}

CSegmentation::~CSegmentation() {}

void CSegmentation::setInputImage( QImage image ) {
    Mat * I = new Mat( image.height() , image.width() , CV_8UC4 , image.bits() , image.bytesPerLine() );
    inputImage = new Mat( I->rows , I->cols , CV_32FC3 );
    cvtColor( *I , *inputImage , CV_RGBA2RGB );
}

void CSegmentation::run() {

        // Take m and n image size
        const int m = inputImage->rows;
        const int n = inputImage->cols;

        // Initialize background and foreground labels
        const double xb = 1;
        const double xf = 0;

        // Initialize tunning constants
        double sigma = 0.1;

        clock_t begin_time = clock();
        // Compute the Graph Laplacian Matrix (L)
        SparseMatrix<double> L( m * n , m * n );
        L.reserve( VectorXi::Constant( m * n , 9 ) );
        GraphLaplacianMatrix( *inputImage , betta , sigma , L );
        cout << "L took " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;

        begin_time = clock();
        // Compute the Graph Laplacian Matrix square
        SparseMatrix<double> L2( m * n , m * n );
        L2 = GraphLaplacianMatrixSquare( L );
        cout << "L^2 took " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;

        begin_time = clock();
        // Compute seeds dependent matrix (Is)
        SparseMatrix<double> Is( m * n , m * n );
        SeedsDependentMatrixIs( Is );
        cout << "Is took " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;
        cout << endl;

    for( int index = 1 ; index < monitor->seedsColor.count() ; index++ ) {

        // Save segmentation start time
        const clock_t t_start = clock();

        begin_time = clock();
        // Compute seeds dependent vector (b)
        VectorXd b( m * n );
        b = VectorXd::Zero( m * n );
        SeedsDependentVectorb( xf , xb , b , index);
        cout << "b took " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;

        begin_time = clock();
        // Solve linear system
        VectorXd X( m * n );
        ComputeLinearSystem( Is + L2 , b , X );
        cout << "Ax = b took " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;

        begin_time = clock();
        // Assign labels to x
        Mat_<float> Y = Mat_<float>::zeros( m , n );
        AssignLabels( m , n , xf , xb , X , Y );
        cout << "Y took " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;

        // Print time tooken for segmentation process
        cout << "Segmentation process took " << float( clock () - t_start ) /  CLOCKS_PER_SEC << endl;
        cout << endl;

        // Show the segmented image
        q = QPixmap::fromImage( QImage( ( unsigned char* ) Y.data , Y.cols , Y.rows, QImage::Format_RGB32 ) );
        emit sendImage(q, false);
    }
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

                        // Compute wij ( Pi is defined by i and j, Pj is defined by a and b )
                        double wij = exp( - betta * pow( CMaths::maximumOfThree( abs( chR( i , j ) - chR( a , b ) ),
                                                                                 abs( chG( i , j ) - chG( a , b ) ),
                                                                                 abs( chB( i , j ) - chB( a , b ) ) ) , 2 ) / sigma ) + 10e-6;

                        // Store the result wij in L
                        L.coeffRef( i * n + j , a * n + b ) = -wij;

                        // Compute dii
                        dii += wij;
                    }
                }
            }
            // Save final dii in L
            L.coeffRef( i * n + j , i * n + j ) = dii;
        }
    }
}

void CSegmentation::SeedsDependentMatrixIs( SparseMatrix<double> &Is ) {
    // Compute Is matrix
    for( QMap< QString , QSet< QPair< int , int > > >::iterator it = monitor->seedsPos.begin() ; it != monitor->seedsPos.end() ; it++ ) {
        for( QSet< QPair< int , int> >::Iterator itt = it->begin() ; itt != it->end() ; ++itt ) {
            Is.coeffRef( itt->first + itt->second * monitor->image->width() , itt-> first + itt->second * monitor->image->width() ) = 1;
        }
        for( QSet< QPair< int , int> >::iterator itt = it->begin() ; itt != it->end() ; ++itt ) {
            Is.coeffRef( itt->first + itt->second * monitor->image->width() , itt->first + itt->second * monitor->image->width() ) = 1;
        }
    }
}

void CSegmentation::SeedsDependentVectorb(const int &xf , const int &xb , VectorXd &b , int ind) {
    // Compute b vector
    for( QMap< QString , QSet< QPair< int , int > > >::iterator it = monitor->seedsPos.begin() ; it != monitor->seedsPos.end() ; it++ ) {
        if( ind == it.key().right(2).toInt() ) {
            for( QSet< QPair< int, int > >::Iterator itt = it->begin() ; itt != it->end() ; ++itt ) {
                b( itt->first + itt->second * monitor->image->width() ) = xb;
            }
        }
        else {
            for( QSet< QPair< int , int > >::iterator itt = it->begin() ; itt != it->end() ; ++itt ) {
                b( itt->first + itt->second * monitor->image->width() ) = xf;
            }
        }
    }
}

void CSegmentation::ComputeLinearSystem( const SparseMatrix<double> &Is_L2 , const VectorXd &b , VectorXd &X ) {
    SimplicialCholesky< SparseMatrix<double> > chol( Is_L2 );
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

void CSegmentation::setBetha( int _val ) {
    betta = _val / 10000.0;
}
