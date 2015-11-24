#include "mainwindow.h"

mainWindow::mainWindow( QWidget *parent ) : QMainWindow( parent ) {

    // Main window format
   // this->setWindowTitle( tr( "Segmentation" ) );
    //this->setWindowState( Qt::WindowMaximized );

    // Workspace
    workspace = new QMdiArea( this );
    //workspace->tileSubWindows();
    setCentralWidget( workspace );

    // Tools widget
    toolsWidget = new CToolsWidget( this );
    workspace->addSubWindow( toolsWidget /*, Qt::FramelessWindowHint */);


    // Monitor widget
    monitorWidget = new CMonitorWidget( toolsWidget , this );
    monitorWidget -> setWindowTitle("Monitor");
    workspace->addSubWindow( monitorWidget/* , Qt::FramelessWindowHint */);

    // Link buttons with actions
    segTool = new CSegmentation( toolsWidget , monitorWidget );


    segmentationThread = new QThread(this);
    segTool->moveToThread(segmentationThread);
    segmentationThread->start();

    connect( toolsWidget , SIGNAL( imageLoaded( QPixmap ) ) , monitorWidget , SLOT( updateImage( QPixmap ) ) );
    connect( toolsWidget , SIGNAL( imageLoaded2( QImage ) ) , segTool , SLOT( setInputImage( QImage ) ), Qt::BlockingQueuedConnection );
    connect( toolsWidget->execBtn , SIGNAL( clicked() ) , segTool, SLOT( run()));
    connect( toolsWidget->clearSeedsBtn , SIGNAL( clicked()) , monitorWidget , SLOT( clearAllSeeds() ) );
    connect( toolsWidget , SIGNAL( imageLoaded( QPixmap ) ) , monitorWidget , SLOT( clearAllSeeds()));
    connect( segTool , SIGNAL( sendImage( QPixmap ) ) , monitorWidget , SLOT( updateImageR( QPixmap ) ), Qt::BlockingQueuedConnection );
    connect( toolsWidget->bethaSlider, SIGNAL( valueChanged(int)), segTool, SLOT( setBetha(int)), Qt::BlockingQueuedConnection);
    //connect( segmentationThread, SIGNAL(finished()), segmentationThread, SLOT(quit()));

}

mainWindow::~mainWindow() {}

