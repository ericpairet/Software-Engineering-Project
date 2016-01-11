#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QPixmap>
#include <QThread>
#include <QShortcut>

#include "widgets.h"
#include "segmentation.h"

/**
 * @brief mainWndows : Class creates the main window of the aaplication
 * creates instances of all the necessary classes and connects the whole
 * structure of the code together, this is the main class of the code and
 * the main function creates on instance of this class and shows it.
 * Program finishes by closing thid window
 *
 */
class mainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Constructor : The constructor of the class.
     *
     * @param parent : QWidget parent object
     */
    explicit mainWindow( QWidget *parent = 0 );
    /**
     * @brief Destructor
     *
     */
    ~mainWindow();

private:
    QMdiArea *workspace; /**< The main workspace as central widget, all the widgets will be added to this worspace */
    CTabDockWidget *tabWidget; /**< Costum maid dockable tab widget for holding all the widgets */
    CMonitorWidget *monitorWidget; /**< Instance of monitor widget that loads and handles the image */
    CToolsWidget *toolsWidget; /**< Instace of tools wodget that contains necessary tools for receiving inputs */
    CSegmentation *segTool; /**< The segmentation tool */
    QThread *segmentationThread; /**< Segmentation thread, segmentation is being done on different thread so that it does not block the main thread */
    CViewerWidget *viewer; /**< Viever class instance, for showing all the images */
    CSeedSelectionWidget *seedsWidget; /**< Widget for selecting and adding the seeds */
    CStatusWidget *statusWidget; /**< Debugger widget for showing usefull information */
    QTimer* statusUpdateTimer; /**< Time for reading the data from Debugger queue and printing them */
private slots:
    /**
     * @brief Viewer handler : this slot will add any new image (segmentation results)
     * to the viewer tab widget
     *
     * @param p : The input picture
     * @param isNewPic : this boolean indicates that is this a new image loaded by user
     * or the segmentation result, in case of being segmentation result it is just being
     * shown, otherwise (in case of loading a new image for segmentation) it will restart
     * the whole environment and removes the previous selected seeds
     */
    void addWidgetToViewer(QPixmap p, bool isNewPic);
    /**
     * @brief Google Test Handler : this slot initiate the complete GTest by running all
     * the unit tests, show the overall result in debugger and the full report in terminal
     *
     */
    void gTest();
};

#endif // MAINWINDOW_H
