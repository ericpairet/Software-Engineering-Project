#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // events are passed here
    virtual bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::MainWindow *ui;

    uint *m_dNumbers; // device array
};

#endif // MAINWINDOW_H
