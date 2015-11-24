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
#include <QBitmap>
#include <QSlider>

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
    QSlider *bethaSlider;
private:
    QPushButton *loadButton;
    QLabel *loadLabel;
    QLabel *seedLabel;
    QLabel *bethaName, *bethaVal;
private slots:
    void loadSlot();
    void updateBethaValue(int _val);
signals:
    void imageLoaded( QPixmap);
    void imageLoaded2( QImage);
};

class CMonitorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CMonitorWidget( CToolsWidget *_tools, QWidget *parent = 0);
    ~CMonitorWidget();
    QSet< QPair< int, int> > fgSeeds, bgSeeds;
    QPixmap *image;
private:
    CToolsWidget *tools;
    QPixmap *image2;
    QTimer* mainTimer;
    bool dragging;
public slots:
    void updateImage( QPixmap);
    void updateImageR( QPixmap);
    void clearAllSeeds();
private slots:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *event);
//    void mousePressEvent(QMouseEvent);
//    void mouseReleaseEvent(QMouseEvent);
signals:
    void imageLoaded1( QPixmap);
};

#endif // WIDGETS_H
