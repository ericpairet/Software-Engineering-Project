#include "widgets.h"

//temp includes :
#include <iostream>
#include <Eigen/Dense>
#include <Eigen/LU>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"


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
    for(int i = 0; i < fgSeeds.count(); i++)
        painter.drawPoint( fgSeeds.at(i).first, fgSeeds.at(i).second);
    painter.setPen(Qt::blue);
    for(int i = 0; i < bgSeeds.count(); i++)
        painter.drawPoint( bgSeeds.at(i).first, bgSeeds.at(i).second);
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
    if( tools->fgRadioButton->isChecked())
    {
        fgSeeds.append( QPair< int, int>( screenPosX, screenPosY));
        for(int i = 1; i <= size; i++)
            for( int j = 1; j <= size; j++)
                if( screenPosX + i < image->width() && screenPosY + j < image->height())
                    fgSeeds.append( QPair< int, int>( screenPosX+i, screenPosY+j));
    }
    else
    {
        bgSeeds.append( QPair< int, int>( screenPosX, screenPosY));
        for(int i = 1; i <= size; i++)
            for( int j = 1; j <= size; j++)
                if( screenPosX + i < image->width() && screenPosY + j < image->height())
                    bgSeeds.append( QPair< int, int>( screenPosX+i, screenPosY+j));
    }
}

using namespace std;
using namespace Eigen;
using namespace cv;

void CMonitorWidget::segmentaion()
{
    // Load image to be computed (mxn)
//    Mat I = QPixmapToCvMat( *image);
    Mat I = imread( tools->imagePath.toStdString().c_str());
    // Take m and n image size
    int m = I.rows;
    int n = I.cols;
//    imshow( "IMG" , I );
//    waitKey();

    qDebug() << "Image Loaded";
    // Check if there is an image
    if ( ( m * n ) == 0 ) {
        cout << "No image" << endl;
        return;
    }

    // Initialize background and foreground labels (do not change the type!)
    float xb = 2;
    float xf = 1;

    // Check xb > xf
    if ( xb < xf ) {
        cout << "Background level must be greater than the foreground one" << endl;
        return;
    }

    // Initialize tunning constants
    float betta = 1.0;

    // Get the R, G, B channels in BGR order
    vector<Mat> channels(3);
    split(I,channels);
    Mat_<int> chR = channels[2];
    Mat_<int> chG = channels[1];
    Mat_<int> chB = channels[0];

    qDebug() << "Chanels created";
    //**************************
    //
    // Compute matrix W and D
    //
    //**************************

    // Initialize adjency matrix W with zeros
    MatrixXf W( m * n , m * n );
    W = MatrixXf::Zero( m * n , m * n );
    qDebug() << "WMatrix done";

    // Initialize diagonal matrix D with zeros
    MatrixXf D( m * n , m * n );
    D = MatrixXf::Zero( m * n , m * n );
    qDebug() << "D Matrix done";

    // For each row
    for ( int i = 0 ; i < m ; i++ ) {
        // For each column
        for ( int j = 0 ; j < n ; j ++ ) {

            // Variable to store the sum of dii
            float tmp = 0;

            // For each row of the neighborhood
            for ( int a = i - 1 ; a <= i + 1 ; a++ ) {
                // For each column of the neighborhood
                for ( int b = j - 1 ; b <= j + 1 ; b++ ) {
                    // Avoid compute edge with itself, values out the matrix and recompute edges
                    if ( !( ( a == i ) and ( b == j ) ) and ( a >= 0 ) and ( a < m ) and ( b >= 0 ) and ( b < n ) and ( W( i * n + j , a * n + b ) == 0 ) ) {

                        // Compute wij ( Pi is defined by i and j, Pj is defined by a and b )
                        float wij = exp( - betta * pow( double( CMaths::maximumOfThree( abs( chR( i , j ) - chR( a , b ) ),
                                                                                        abs( chG( i , j ) - chG( a , b ) ),
                                                                                        abs( chB( i , j ) - chB( a , b ) ) ) ) , 2 ) / 0.1 ) + 10e-6;

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

    //**************************
    //
    // Compute matrix L
    //
    //**************************

    MatrixXf L( m * n , m * n );
    L = D - W;
    qDebug() << "L Matrix";
    //**************************
    //
    // Solve linear system
    //   (Is + L^2)x = b
    //
    //**************************

    //
    MatrixXf Is( m * n , m * n );
    Is = MatrixXf::Zero( m * n , m * n );       // just for test (change with the GUI)
    VectorXf b( m * n );
    b = VectorXf::Zero( m*n);
    qDebug() << "I created";
    for(int i = 0; i < fgSeeds.count(); i++)
    {
        qDebug() << fgSeeds.at(i).first << fgSeeds.at(i).second << image->width() << image->height();
        Is( fgSeeds.at(i).first, fgSeeds.at(i).first) = 1;
        Is( fgSeeds.at(i).second, fgSeeds.at(i).second) = 1;
        b( fgSeeds.at(i).first * image->height()+ fgSeeds.at(i).second ) = xf;
    }
    for(int i = 0; i < bgSeeds.count(); i++)
    {
        qDebug() << bgSeeds.at(i).first << bgSeeds.at(i).second;
        Is( bgSeeds.at(i).first, bgSeeds.at(i).first) = 1;
        Is( bgSeeds.at(i).second, bgSeeds.at(i).second) = 1;
        b( bgSeeds.at(i).first * image->height()+ bgSeeds.at(i).second ) = xb;
    }
    qDebug() << "I matrix";

    MatrixXf Is_L( m * n , m * n );
    Is_L = Is + L * L;

    qDebug() << "Is_L Matrix";
    VectorXf X( m * n );
    X = Is_L.inverse() * b;
    qDebug() << "Inversed";

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
    qDebug() << "Y created";
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

//-------------------------------------------------------------------------------

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
    penSize = new QLineEdit("1");
    execButton = new QPushButton("Execute");
    lOut->addWidget( loadLabel, 0, 0);
    lOut->addWidget( loadButton, 0, 1);
    lOut->addWidget( seedLabel, 1, 0);
    lOut->addWidget( fgRadioButton, 1, 1);
    lOut->addWidget( bgRadioButton, 2, 1);
    lOut->addWidget( penSize, 2,0);
    lOut->addWidget( execButton, 3,0);

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

