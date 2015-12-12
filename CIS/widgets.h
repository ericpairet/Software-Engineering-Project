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
#include <QSizePolicy>
#include <QDockWidget>
#include <QColorDialog>
#include <QButtonGroup>

#include "mathtools.h"

class CTabDockWidget : public QDockWidget
{
    Q_OBJECT
public:
        CTabDockWidget(QWidget* parent);
        ~CTabDockWidget();
        QTabWidget *tabs;
        QWidget* w;
};

class CToolsWidget : public QWidget
{
    Q_OBJECT
public:
    CToolsWidget(QWidget *parent = 0);
    ~CToolsWidget();
    QLineEdit *penSize;
    QPushButton *execBtn, *clearSeedsBtn;
    QString imagePath;
    QSlider *bethaSlider;
private:
    QPushButton *loadButton;
    QLabel *loadLabel;
    QLabel *bethaName, *bethaVal;
private slots:
    void loadSlot();
    void updateBethaValue(int _val);
signals:
    void imageLoaded( QPixmap, bool);
    void imageLoaded2( QImage);
};

class CMonitorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CMonitorWidget( CToolsWidget *_tools, QWidget *parent = 0);
    ~CMonitorWidget();
    QMap< QString, QSet< QPair< int, int> > > seedsPos;
    QMap< QString, QColor> seedsColor;
    QPixmap *image;
    QString seedName;
    QColor seedColor;
private:
    CToolsWidget *tools;
    QTimer* mainTimer;
    bool dragging;
public slots:
    void updateImage( QPixmap);
    void clearAllSeeds();
    inline void setInputImage( QPixmap _p){ image = new QPixmap( _p);}
    inline void setCurrentSeed( QString _s, QColor _c){ seedName = _s; seedColor = _c; seedsColor[seedName] = seedColor;}
private slots:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *event);
signals:
    void imageLoaded1( QPixmap);
};

class CViewerWidget : public QTabWidget
{
    Q_OBJECT
public:
    CViewerWidget( QWidget *parent = 0);
    ~CViewerWidget();
};

class CSeedWidget : public QWidget
{
    Q_OBJECT
public:
    CSeedWidget( QString name, QButtonGroup *gp, QWidget *parent = 0);
    ~CSeedWidget();
    inline QColor getColor(){ return seedColor;}
    inline QString getName(){ return seedName->text();}
private:
    QLabel *seedName;
    QRadioButton *selected;
    QColor seedColor;
    QPushButton *colorSelectBtn;
    QColorDialog *colorSelector;
private slots:
    void setColor();
    void emitSeedChanged( bool is);
signals:
    void seedChanged( QString, QColor);
};

class CSeedSelectionWidget : public QWidget
{
    Q_OBJECT
public:
    CSeedSelectionWidget( QWidget *parent = 0);
    ~CSeedSelectionWidget();
private:
    QPushButton *addSeedBtn;
    QGridLayout *lOut;
    QList< CSeedWidget*> seeds;
    QButtonGroup *buttonsGroup;
public slots:
    void removeSeeds();
private slots:
    void addNewSeed();
    void emitSelectedSeed( QString _n, QColor _c);
signals:
    void selectedSeedSignal( QString, QColor);
};

#endif // WIDGETS_H
