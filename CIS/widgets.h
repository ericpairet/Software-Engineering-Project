#ifndef WIDGETS_H
#define WIDGETS_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QFileDialog>
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <QTimer>

class CMonitorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CMonitorWidget( QWidget *parent = 0);
    ~CMonitorWidget();
private:
    QPixmap *image;
    QTimer* mainTimer;
public slots:
    void updateImage( QPixmap);
private slots:
    void paintEvent(QPaintEvent *);
};

class CToolsWidget : public QWidget
{
    Q_OBJECT
public:
    CToolsWidget(QWidget *parent = 0);
    ~CToolsWidget();
    QPushButton *loadButton;
private:
    QLabel *loadLabel;
private slots:
    void loadSlot();
signals:
    void imageLoaded( QPixmap);
};

#endif // WIDGETS_H
