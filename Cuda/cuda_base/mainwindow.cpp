#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QEvent>
#include <QMouseEvent>
#include <assert.h>

#include "wrappers.cuh"

const uint MAX_NUMBERS = 5000;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // basic ui setup and event filter for mouse movements
    ui->setupUi(this);
    qApp->installEventFilter(this);


    // create a host array and initialize it to {1, 2, 3, ..., MAX_NUMBERS}
    uint hNumbers[MAX_NUMBERS];
    for (uint i = 0; i < MAX_NUMBERS; i++)
    {
        hNumbers[i] = i + 1;
    }

    // CUDA FUNCTIONS:
    cudaInit(); // initialiaze the cuda device
    allocateArray((void**)&m_dNumbers, MAX_NUMBERS*sizeof(int)); // allocate device array
    copyArrayToDevice(m_dNumbers, hNumbers, 0, MAX_NUMBERS*sizeof(int)); // copy host array to device array

}

MainWindow::~MainWindow()
{
    // CUDA FUNCTION: free device memory
    freeArray(m_dNumbers);
    delete ui;
}

// used to detect mouse movement events
bool MainWindow::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::MouseMove)
    {
        // find mouseX + mouseY
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint p = mouseEvent->pos();
        uint n = std::min((uint)(p.x() + p.y()), MAX_NUMBERS);

        // CUDA FUNCTION:
        // compute the sum of 1 + 2 + 3 + ... + n
        uint sum = sumNumbers(m_dNumbers, n);

        // check that the sum is correct
        assert(sum == ( (n * (n+1) ) / 2 ) );

        // show the sum at the bottom of the window
        statusBar()->showMessage(QString("Mouse pos: (%1, %2)    Sum from 0 to %3 = %4").arg(p.x()).arg(p.y()).    arg(n).arg(sum));
    }
    return false;
}
