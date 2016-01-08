#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QPixmap>
#include <QThread>

#include "widgets.h"
#include "segmentation.h"

class mainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit mainWindow( QWidget *parent = 0 );
    ~mainWindow();

private:
    QMdiArea *workspace;
    CTabDockWidget *tabWidget;
    CMonitorWidget *monitorWidget;
    CToolsWidget *toolsWidget;
    CSegmentation *segTool;
    QThread *segmentationThread;
    CViewerWidget *viewer;
    CSeedSelectionWidget *seedsWidget;
    CStatusWidget *statusWidget;
    QTimer* statusUpdateTimer;
private slots:
    void addWidgetToViewer(QPixmap p, bool isNewPic);
};

#endif // MAINWINDOW_H
