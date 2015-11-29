#include "mainwindow.h"

mainWindow::mainWindow( QWidget *parent ) : QMainWindow( parent ) {

    // Main window format
   // this->setWindowTitle( tr( "Segmentation" ) );
    //this->setWindowState( Qt::WindowMaximized );

    // Workspace
    workspace = new QMdiArea( this );
    //workspace->tileSubWindows();
    setCentralWidget( workspace );

    tabWidget = new CTabDockWidget( 0);
    addDockWidget(Qt::LeftDockWidgetArea,tabWidget);

    // Tools widget
    toolsWidget = new CToolsWidget( this );
    tabWidget->tabs->addTab( toolsWidget, "Tools");

    // Monitor widget
    monitorWidget = new CMonitorWidget( toolsWidget , this );
    monitorWidget->hide();

    // Viewer Widget
    viewer = new CViewerWidget( this);
    workspace->addSubWindow( viewer, Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    viewer->setWindowState( Qt::WindowMaximized);

    // Link buttons with actions
    segTool = new CSegmentation( toolsWidget , monitorWidget );

    segmentationThread = new QThread(this);
    segTool->moveToThread(segmentationThread);
    segmentationThread->start();

    connect( toolsWidget , SIGNAL( imageLoaded( QPixmap ) ) , monitorWidget , SLOT( updateImage( QPixmap ) ) );
    connect( toolsWidget , SIGNAL( imageLoaded( QPixmap ) ) , this, SLOT( addWidgetToViewer(QPixmap)));
    connect( toolsWidget , SIGNAL( imageLoaded2( QImage ) ) , segTool , SLOT( setInputImage( QImage ) ), Qt::BlockingQueuedConnection );
    connect( toolsWidget->execBtn , SIGNAL( clicked() ) , segTool, SLOT( run()));
    connect( toolsWidget->clearSeedsBtn , SIGNAL( clicked()) , monitorWidget , SLOT( clearAllSeeds() ) );
    connect( toolsWidget , SIGNAL( imageLoaded( QPixmap ) ) , monitorWidget , SLOT( clearAllSeeds()));
    connect( segTool , SIGNAL( sendImage( QPixmap ) ) , this, SLOT( addWidgetToViewer(QPixmap)), Qt::BlockingQueuedConnection );
    connect( toolsWidget->bethaSlider, SIGNAL( valueChanged(int)), segTool, SLOT( setBetha(int)), Qt::BlockingQueuedConnection);

}

mainWindow::~mainWindow() {}

void mainWindow::addWidgetToViewer(QPixmap p)
{
    if( viewer->count() > 0)
    {
        QPixmap output( p.width(), p.height());
        output.fill(Qt::transparent);
        QPainter painter( &output);
        //Setting the mask for the output Image
        QBitmap mask = p.createMaskFromColor(Qt::MaskInColor);
        QPixmap tempImage = monitorWidget->image->copy( monitorWidget->image->rect());
        tempImage.setMask(mask);
        painter.drawPixmap(0, 0, tempImage);
        painter.drawPixmap(p.width(), 0 , p);
        QPixmap *image = new QPixmap(output);
        QWidget *w = new QWidget(0);
        QLabel *l = new QLabel( w);
        l->setPixmap( *image);
        viewer->addTab( w, QString("ForeGround %1").arg(viewer->count()));
    }
    else
    {
        monitorWidget->setFixedSize( p.size());
        viewer->addTab( monitorWidget, "Original");
    }
}
