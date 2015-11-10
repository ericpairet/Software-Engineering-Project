#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QPixmap>

#include "widgets.h"
#include "segmentation.h"

class mainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit mainWindow(QWidget *parent = 0);
    ~mainWindow();

private:
    QMdiArea *workspace; /**< The workspace to add widgets to it */
    CMonitorWidget *monitorWidget;
    CToolsWidget *toolsWidget;
    CSegmentation *segTool;
protected:

signals:

public slots:
};

#endif // MAINWINDOW_H
