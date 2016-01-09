#include "segmentation.h"

CSegmentation::CSegmentation( CMonitorWidget *_m ) : QObject() {
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
        GraphLaplacianMatrix( *inputImage , betta , sigma, L);
//        cout << "L took " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;
        debug( QString("Calculating 'L' took : %1 seconds").arg(float( clock () - begin_time ) /  CLOCKS_PER_SEC));

        begin_time = clock();
        // Compute the Graph Laplacian Matrix square
        SparseMatrix<double> L2( m * n , m * n );
        L2 = GraphLaplacianMatrixSquare( L );
//        cout << "L^2 took " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;
        debug( QString("Calculating 'L*L' took : %1 seconds").arg(float( clock () - begin_time ) /  CLOCKS_PER_SEC));

        begin_time = clock();
        // Compute seeds dependent matrix (Is)
        SparseMatrix<double> Is( m * n , m * n );
        SeedsDependentMatrixIs( Is );
        debug( QString("Calculating 'Is' took : %1 seconds").arg(float( clock () - begin_time ) /  CLOCKS_PER_SEC));
//        cout << "Is took " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;
//        cout << endl;

    for( QMap< QString, QColor>::iterator it = monitor->seedsColor.begin() ; it != monitor->seedsColor.end() ; ++it ){
        if( it.key() == "BackGround")
                continue;
//    for( int index = 1 ; index < monitor->seedsColor.count() ; index++ ) {

        debug("Segmentation Started for " + it.key(), it.value());
        // Save segmentation start time
        begin_time = clock();
        // Compute seeds dependent vector (b)
        VectorXd b( m * n );
        b = VectorXd::Zero( m * n );
        debug( QString("Xf = %1, Xb = %2").arg(xf).arg(xb), "brown");
        SeedsDependentVectorb( xf , xb , b , it.key());
//        cout << "b took " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;
        debug( QString("Calculating 'b' took : %1 seconds").arg(float( clock () - begin_time ) /  CLOCKS_PER_SEC), it.value());

        begin_time = clock();
        // Solve linear system
        VectorXd X( m * n );
        ComputeLinearSystem( Is + L2 , b , X );
//        cout << "Ax = b took " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;
        debug( QString("Calculating 'Ax = b' took : %1 seconds").arg(float( clock () - begin_time ) /  CLOCKS_PER_SEC), it.value());

        begin_time = clock();
        // Assign labels to x
        Mat_<float> Y = Mat_<float>::zeros( m , n );
        AssignLabels( m , n , xf , xb , X , Y );
//        cout << "Y took " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;
        debug( QString("Calculating 'Y' took : %1 seconds").arg(float( clock () - begin_time ) /  CLOCKS_PER_SEC), it.value());

        // Print time tooken for segmentation process
//        cout << "Segmentation process took " << float( clock () - t_start ) /  CLOCKS_PER_SEC << endl;
        debug( QString("Segmentation process for %1 took : %2 seconds").arg(it.key()).arg(float( clock () - begin_time ) /  CLOCKS_PER_SEC), it.value());
//        cout << endl;

        // Show the segmented image
        q = QPixmap::fromImage( QImage( ( unsigned char* ) Y.data , Y.cols , Y.rows, QImage::Format_RGB32 ) );
        emit sendImage(q, false);
        debug( QString("%1 Segmented successfully !").arg(it.key()), it.value());
    }
}

size_t CSegmentation::GraphLaplacianMatrix(const Mat &I , const double &betta , const double &sigma, SparseMatrix<double> &L, bool testing) {
    // Take m and n image size
    const int m = I.rows;
    const int n = I.cols;

    // Get the R, G, B channels in BGR order
    vector<Mat> channels( 3 );
    split( I , channels );
    const Mat_<double> chR = channels[2];
    const Mat_<double> chG = channels[1];
    const Mat_<double> chB = channels[0];

//    SparseMatrix<double> L( m * n , m * n );
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
//    ofstream ofs("graphOutput.gtd", ios::binary);
//    ofs.write((char *)&L, sizeof(L));
//    saveMarket(L, "graphOutput.gtd");
    if( testing)
    {
        sparseHash<SparseMatrix<double> > hashGenerator;
        return hashGenerator( L);
    }
    return 0;
}

size_t CSegmentation::SeedsDependentMatrixIs(SparseMatrix<double> &Is , bool testing) {
    // Compute Is matrix
    for( QMap< QString , QSet< QPair< int , int > > >::iterator it = monitor->seedsPos.begin() ; it != monitor->seedsPos.end() ; it++ ) {
        for( QSet< QPair< int , int> >::Iterator itt = it->begin() ; itt != it->end() ; ++itt ) {
            Is.coeffRef( itt->first + itt->second * monitor->image->width() , itt-> first + itt->second * monitor->image->width() ) = 1;
        }
        for( QSet< QPair< int , int> >::iterator itt = it->begin() ; itt != it->end() ; ++itt ) {
            Is.coeffRef( itt->first + itt->second * monitor->image->width() , itt->first + itt->second * monitor->image->width() ) = 1;
        }
    }
    if( testing)
    {
        sparseHash<SparseMatrix<double> > hashGenerator;
        cout << hashGenerator( Is);
        return hashGenerator( Is);
    }
    return 0;
}

size_t CSegmentation::SeedsDependentVectorb(const int &xf , const int &xb , VectorXd &b , QString seed, bool testing) {
    // Compute b vector
    for( QMap< QString , QSet< QPair< int , int > > >::iterator it = monitor->seedsPos.begin() ; it != monitor->seedsPos.end() ; it++ ) {
        if( seed == it.key()) {
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
//    if( seed == "Foreground")
//        cout << "calced : \n\n" << b << "\n\n\n";
//    {
//        ofstream ofs("vecBOutput.gtd", ios::binary);
//        ofs << b;
//    }
    if( testing)
    {
        vectorHash<VectorXd> hashGenerator;
        return hashGenerator( b);
    }
    return 0;
}

size_t CSegmentation::ComputeLinearSystem(const SparseMatrix<double> &Is_L2 , const VectorXd &b, VectorXd &X, bool testing) {
    SimplicialCholesky< SparseMatrix<double> > chol( Is_L2 );
    X = chol.solve( b );
    // Calculating the hash of object
    if( testing)
    {
        vectorHash<VectorXd> hashGenerator;
        return hashGenerator( X);
    }
    return 0;
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
