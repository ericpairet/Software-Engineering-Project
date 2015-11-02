#include "mainwindow.h"

mainWindow::mainWindow(QWidget *parent) : QMainWindow(parent)
{
    workspace = new QMdiArea(this);
    setCentralWidget(workspace);

    toolsWidget = new CToolsWidget( this);
    toolsWidget->setWindowTitle(tr("Tools"));
    workspace->addSubWindow(toolsWidget);

    monitorWidget = new CMonitorWidget( toolsWidget, this);
    monitorWidget->setWindowTitle(tr("Monitor"));
    workspace->addSubWindow(monitorWidget, Qt::CustomizeWindowHint | Qt::WindowTitleHint);
//    monitorWidget->setWindowState(Qt::WindowMaximized );

    connect( toolsWidget, SIGNAL(imageLoaded(QPixmap)), monitorWidget, SLOT(updateImage(QPixmap)));
    connect( monitorWidget, SIGNAL(imageLoaded1(QPixmap)), monitorWidget, SLOT(updateImageR(QPixmap)));
    connect( toolsWidget->execBtn, SIGNAL(pressed()), monitorWidget, SLOT(segmentation()));
    connect( toolsWidget->clearSeedsBtn, SIGNAL(pressed()), monitorWidget, SLOT(clearAllSeeds()));
}

mainWindow::~mainWindow()
{

}

