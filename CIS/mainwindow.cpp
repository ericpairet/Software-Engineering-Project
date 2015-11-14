#include "mainwindow.h"

mainWindow::mainWindow( QWidget *parent ) : QMainWindow( parent ) {

    // Main window format
    this->setWindowTitle( tr( "Segmentation" ) );
    //this->setWindowState( Qt::WindowMaximized );

    // Workspace
    workspace = new QMdiArea( this );
    //workspace->tileSubWindows();
    setCentralWidget( workspace );

    // Tools widget
    toolsWidget = new CToolsWidget( this );
    workspace->addSubWindow( toolsWidget , Qt::FramelessWindowHint );

    // Monitor widget
    monitorWidget = new CMonitorWidget( toolsWidget , this );
    workspace->addSubWindow( monitorWidget , Qt::FramelessWindowHint );

    // Link buttons with actions
    segTool = new CSegmentation( toolsWidget , monitorWidget );
    connect( toolsWidget , SIGNAL( imageLoaded( QPixmap ) ) , monitorWidget , SLOT( updateImage( QPixmap ) ) );
    connect( toolsWidget , SIGNAL( imageLoaded2( QImage ) ) , segTool , SLOT( setInputImage( QImage ) ) );
    connect( toolsWidget->execBtn , SIGNAL( pressed() ) , segTool , SLOT( run() ) );
    connect( toolsWidget->clearSeedsBtn , SIGNAL( pressed() ) , monitorWidget , SLOT( clearAllSeeds() ) );
    connect( segTool , SIGNAL( sendImage( QPixmap ) ) , monitorWidget , SLOT( updateImageR( QPixmap ) ) );
}

mainWindow::~mainWindow() {}

