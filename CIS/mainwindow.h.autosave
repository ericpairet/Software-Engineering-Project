#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QPixmap>

#include "widgets.h"
#include "segmentation.h"

class mainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit mainWindow( QWidget *parent = 0 );
    ~mainWindow();

private:
    QMdiArea *workspace;
    CMonitorWidget *monitorWidget;
    CToolsWidget *toolsWidget;
    CSegmentation *segTool;
};

#endif // MAINWINDOW_H
