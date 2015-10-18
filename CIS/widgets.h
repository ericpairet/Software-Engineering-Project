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
#include <QRadioButton>
//#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>
#include <QLineEdit>
#include <QImage>

#include "mathtools.h"

class CToolsWidget : public QWidget
{
    Q_OBJECT
public:
    CToolsWidget(QWidget *parent = 0);
    ~CToolsWidget();
    QRadioButton *fgRadioButton, *bgRadioButton, *rmvRadioButton;
    QLineEdit *penSize;
    QPushButton *execBtn, *clearSeedsBtn;
    QString imagePath;
private:
    QPushButton *loadButton;
    QLabel *loadLabel;
    QLabel *seedLabel;
private slots:
    void loadSlot();
signals:
    void imageLoaded( QPixmap);
};

class CMonitorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CMonitorWidget( CToolsWidget *_tools, QWidget *parent = 0);
    ~CMonitorWidget();
private:
    CToolsWidget *tools;
    QPixmap *image;
    QTimer* mainTimer;
    QSet< QPair< int, int> > fgSeeds, bgSeeds;
    bool dragging;
public slots:
    void updateImage( QPixmap);
    void segmentaion();
    void clearAllSeeds();
private slots:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *event);
//    void mousePressEvent(QMouseEvent);
//    void mouseReleaseEvent(QMouseEvent);
};

#endif // WIDGETS_H
