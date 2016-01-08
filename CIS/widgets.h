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
#include <QMouseEvent>
#include <QLineEdit>
#include <QImage>
#include <QBitmap>
#include <QSlider>
#include <QSizePolicy>
#include <QDockWidget>
#include <QColorDialog>
#include <QButtonGroup>
#include <QQueue>
#include <QTextEdit>
#include <QTime>
#include <QTextDocument>

#include "mathtools.h"

//Debugger :
void debug(QString text, QColor color);

class CTabDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    CTabDockWidget(QWidget* parent);
    ~CTabDockWidget();
    QTabWidget *tabs;
    QWidget* w;
signals:
    void resized( QSize);
};

class CToolsWidget : public QWidget
{
    Q_OBJECT
public:
    CToolsWidget(QWidget *parent = 0);
    ~CToolsWidget();
    QLineEdit *penSize;
    QPushButton *execBtn, *clearSeedsBtn;
    QSlider *bethaSlider;
private:
    QPushButton *loadButton;
    QLabel *loadLabel;
    QLabel *bethaName, *bethaVal;
    QSize imageLimit;
private slots:
    void loadSlot();
    void updateBethaValue(int _val);
signals:
    void imageLoadedQPixmap( QPixmap, bool);
    void imageLoadedQImage( QImage);
public slots:
    void setImageSizeLimit( QSize);
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
private:
    void resizeEvent(QResizeEvent *event);
signals:
    void imageSizeLimitSignal( QSize);
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


class CStatusText
{
    public:
    CStatusText(QString _text = "", QColor _color = QColor("black"), int _size = 12)
    {
        text= _text;
        color = _color;
        size = _size;
    }

    QString text;
    QColor color;
    int size;
};

class CStatusPrinter
{
    public:
    CStatusPrinter() {}
    QQueue<CStatusText> textBuffer; /**< TODO */
};

class CStatusWidget : public QDockWidget
{
    Q_OBJECT
public:
    CStatusWidget(CStatusPrinter* _statusPrinter);
    ~CStatusWidget();
    QTextEdit *statusText;
    QLabel *titleLbl;
    QTextDocument content;
public slots:
    void write(QString str, QColor color = QColor("black"));
    void update();
    void getSizeFromViewer(QSize size);
private:
    CStatusPrinter *statusPrinter;
    QTime logTime;
signals:
    void closeSignal(bool);
protected:
    void closeEvent(QEvent*);
};

extern CStatusPrinter *printer;

#endif // WIDGETS_H
