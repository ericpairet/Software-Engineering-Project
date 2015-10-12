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
    connect( toolsWidget->execButton, SIGNAL(pressed()), monitorWidget, SLOT(segmentaion()));
}

mainWindow::~mainWindow()
{

}

