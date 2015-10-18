#include "widgets.h"

// Temporal includes
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


//Monitor Widget for showing the image
CMonitorWidget::CMonitorWidget(CToolsWidget *_tools, QWidget *parent)
    : QWidget(parent)
{
    tools = _tools;
    this->setFixedSize(690,390);
    image = new QPixmap( 690,390);
    mainTimer = new QTimer();
    mainTimer->setInterval(30);
    mainTimer->start();
    dragging = false;
    connect(mainTimer,SIGNAL(timeout()),this,SLOT(repaint()));
}

CMonitorWidget::~CMonitorWidget()
{

}

void CMonitorWidget::paintEvent(QPaintEvent */*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPixmap( 0, 0, *image);
    resize( image->width(), image->height());
    painter.setPen(Qt::red);
    for( QSet< QPair< int, int> >::Iterator it = fgSeeds.begin(); it != fgSeeds.end(); ++it)
        painter.drawPoint( it->first, it->second);
    painter.setPen(Qt::blue);
    for( QSet< QPair< int, int> >::Iterator it = bgSeeds.begin(); it != bgSeeds.end(); ++it)
        painter.drawPoint( it->first, it->second);
}

void CMonitorWidget::updateImage( QPixmap p)
{
    QPixmap output( p.width(), p.height());
    output.fill(Qt::transparent);
    QPainter painter( &output);
    painter.drawPixmap( 0, 0, p);
    image = new QPixmap(output);
}

void CMonitorWidget::mouseMoveEvent(QMouseEvent *event)
{
    int screenPosX = event->pos().x();
    int screenPosY = event->pos().y();
    qDebug() << screenPosX << screenPosY;
    int size = tools->penSize->text().toInt();
    if( tools->fgRadioButton->isChecked() && image->rect().contains( screenPosX, screenPosY))
    {
        fgSeeds.insert( QPair< int, int>( screenPosX, screenPosY));
        for(int i = 1; i <= size; i++)
            for( int j = 1; j <= size; j++)
                if( image->rect().contains( screenPosX, screenPosY))
                    fgSeeds.insert( QPair< int, int>( screenPosX+i, screenPosY+j));
    }
    else if( tools->bgRadioButton->isChecked() && image->rect().contains( screenPosX, screenPosY))
    {
        bgSeeds.insert( QPair< int, int>( screenPosX, screenPosY));
        for(int i = 1; i <= size; i++)
            for( int j = 1; j <= size; j++)
                if( image->rect().contains( screenPosX, screenPosY))
                    bgSeeds.insert( QPair< int, int>( screenPosX+i, screenPosY+j));
    }
    else
    {
        fgSeeds.remove( QPair< int, int>( screenPosX, screenPosY));
        bgSeeds.remove( QPair< int, int>( screenPosX, screenPosY));
        for(int i = 1; i <= size; i++)
            for( int j = 1; j <= size; j++)
            {
                fgSeeds.remove( QPair< int, int>( screenPosX+i, screenPosY+j));
                bgSeeds.remove( QPair< int, int>( screenPosX+i, screenPosY+j));
            }
    }
}

void CMonitorWidget::clearAllSeeds()
{
    fgSeeds.clear();
    bgSeeds.clear();
}

using namespace std;
using namespace Eigen;
using namespace cv;

Mat img2grey(Mat img) {
    Mat dst;
    normalize(img, dst, 0.0, 1.0, NORM_MINMAX, CV_32FC3);
    return dst;
}

void CMonitorWidget::segmentaion()
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

    // Get the R, G, B channels in BGR order
    vector<Mat> channels(3);
    split(I,channels);
    Mat_<double> chR = channels[2];
    Mat_<double> chG = channels[1];
    Mat_<double> chB = channels[0];

    //**************************
    //
    // Compute matrix W and D
    //
    //**************************

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
                                                                                abs( chB( i , j ) - chB( a , b ) ) ) , 2 ) / 0.1 ) + 10e-6;


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
    MatrixXf L( m * n , m * n );
    L = D - W;

    //**************************
    //
    // Solve linear system
    //       Ax = b
    //
    //**************************

    // Create Is matrix and initialize with zeros
    MatrixXf Is( m * n , m * n );
    Is = MatrixXf::Zero( m * n , m * n );

    // Create b vector and initialize with zeros
    VectorXf b( m * n );
    b = VectorXf::Zero( m*n);

    /*
    // Just for test with testImage.jpg (7x9 Pixels image)
    fgSeeds.clear();
    fgSeeds.append(QPair<int, int> (3, 3));
    fgSeeds.append(QPair<int, int> (3, 4));
    fgSeeds.append(QPair<int, int> (4, 3));

    bgSeeds.clear();
    bgSeeds.append(QPair<int, int> (1, 6));
    bgSeeds.append(QPair<int, int> (1, 7));
    bgSeeds.append(QPair<int, int> (2, 7));
    bgSeeds.append(QPair<int, int> (3, 7));
    bgSeeds.append(QPair<int, int> (4, 7));
    bgSeeds.append(QPair<int, int> (5, 7));
    */

    // Compute Is matrix and b vector                                                         // --> TODO: Apply OR gate? It can avoid go through all the seeds
    for( QSet< QPair< int, int> >::Iterator it = fgSeeds.begin(); it != fgSeeds.end(); ++it)
    {
        Is( it->first + it->second * image->width() , it->first + it->second * image->width() ) = 1;
        b( it->first + it->second * image->width() ) = xf;
    }

    for( QSet< QPair< int, int> >::Iterator it = bgSeeds.begin(); it != bgSeeds.end(); ++it)
    {
        Is( it->first + it->second * image->width() , it->first + it->second * image->width() ) = 1;
        b( it->first + it->second * image->width() ) = xb;
    }




    // Compue Is_L
    qDebug() << "Starting L^2";
    MatrixXf Is_L( m * n , m * n );

    tstart = time(0);
    Eigen::SparseMatrix<float> L_sparse( m * n , m * n );
    L_sparse = L.sparseView();      // Check sparse = dense.sparseView(epsilon,reference) for better results?
    Eigen::SparseMatrix<float> L2_sparse_test( m * n , m * n);
    L2_sparse_test = L_sparse * L_sparse;
    Is_L = Is + MatrixXf(L2_sparse_test);
    tend = time(0);
    cout << "It took "<< difftime(tend, tstart) <<" second(s)."<< endl;

    /*tstart = time(0);
    Is_L = Is + L * L;
    tend = time(0);
    cout << "It took "<< difftime(tend, tstart) <<" second(s)."<< endl;
    */
    // Compute X
    VectorXf X( m * n );

// ----------------------------------------------------------------------------------------------------------------
/*
    tstart = time(0);
    // OPTION 1
    X = Is_L.inverse() * b;

    tend = time(0);
    cout << "It took "<< difftime(tend, tstart) <<" second(s)."<< endl;
*/

/*
    tstart = time(0);
    // OPTION 2
    X = b;
    LDLT<MatrixXf> ldlt;
    ldlt.compute(Is_L);
    ldlt.solveInPlace(X);

    tend = time(0);
    cout << "It took "<< difftime(tend, tstart) <<" second(s)."<< endl;
*/


    tstart = time(0);
    // OPTION 3
    X = Is_L.ldlt().solve(b);
    tend = time(0);
    cout << "It took "<< difftime(tend, tstart) <<" second(s)."<< endl;


/*
    tstart = time(0);
    // OPTION 4     -->     Sparse matrix
    SparseMatrix<float> A( m * n , m * n );
    A = Is_L.sparseView();      // Check sparse = dense.sparseView(epsilon,reference) for better results?

    // Solving:
    SimplicialCholesky<SpMat> chol(A);  // performs a Cholesky factorization of A
    X = chol.solve(b);         // use the factorization to solve for the given right hand side


    tend = time(0);
    cout << "It took "<< difftime(tend, tstart) <<" second(s)."<< endl;*/

// ----------------------------------------------------------------------------------------------------------------

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
    imshow( "SI" , Y );
    waitKey();
}


//Tools Widget for handling the tools (this class is temperory, we can put everything we need here for know, then we can separate them
CToolsWidget::CToolsWidget( QWidget *parent)
    : QWidget(parent)
{
    QGridLayout *lOut = new QGridLayout(this);
    setLayout( lOut);
    loadLabel = new QLabel("Load image");
    loadButton = new QPushButton("Browse");
    seedLabel = new QLabel("Seed selection:");
    fgRadioButton = new QRadioButton("ForeGround");
    fgRadioButton->setChecked( true);
    bgRadioButton = new QRadioButton("BackGroung");
    rmvRadioButton = new QRadioButton("Remove");
    penSize = new QLineEdit("1");
    execBtn = new QPushButton("Execute");
    clearSeedsBtn = new QPushButton("Clear");
    lOut->addWidget( loadLabel, 0, 0);
    lOut->addWidget( loadButton, 0, 1);
    lOut->addWidget( seedLabel, 1, 0);
    lOut->addWidget( fgRadioButton, 1, 1);
    lOut->addWidget( bgRadioButton, 2, 1);
    lOut->addWidget( rmvRadioButton, 3, 1);
    lOut->addWidget( penSize, 2,0);
    lOut->addWidget( clearSeedsBtn, 3,0);
    lOut->addWidget( execBtn, 4,0);

    connect( loadButton, SIGNAL(pressed()), this, SLOT(loadSlot()));
}

CToolsWidget::~CToolsWidget()
{

}

void CToolsWidget::loadSlot()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Load image"), "./", tr("Image files (*.jpg *.jpeg *.png)"));
    if(path != "")
    {
        QPixmap pic( path);
        emit imageLoaded( pic);
    }
    imagePath = path;
}

