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
/**
 * @brief
 *
 * @param text
 * @param color
 */
void debug(QString text, QColor color = "black");

/**
 * @brief
 *
 */
class CTabDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    /**
     * @brief
     *
     * @param parent
     */
    CTabDockWidget(QWidget* parent);
    /**
     * @brief
     *
     */
    ~CTabDockWidget();
    QTabWidget *tabs; /**< TODO */
    QWidget* w; /**< TODO */
signals:
    /**
     * @brief
     *
     * @param QSize
     */
    void resized( QSize);
};

/**
 * @brief
 *
 */
class CToolsWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief
     *
     * @param parent
     */
    CToolsWidget(QWidget *parent = 0);
    /**
     * @brief
     *
     */
    ~CToolsWidget();
    QLineEdit *penSize; /**< TODO */
    QPushButton *execBtn, *clearSeedsBtn; /**< TODO */
    QSlider *bethaSlider; /**< TODO */
private:
    QPushButton *loadButton; /**< TODO */
    QLabel *loadLabel; /**< TODO */
    QLabel *bethaName, *bethaVal; /**< TODO */
    QSize imageLimit; /**< TODO */
private slots:
    /**
     * @brief
     *
     */
    void loadSlot();
    /**
     * @brief
     *
     * @param _val
     */
    void updateBethaValue(int _val);
signals:
    /**
     * @brief
     *
     * @param QPixmap
     * @param bool
     */
    void imageLoadedQPixmap( QPixmap, bool);
    /**
     * @brief
     *
     * @param QImage
     */
    void imageLoadedQImage( QImage);
public slots:
    /**
     * @brief
     *
     * @param QSize
     */
    void setImageSizeLimit( QSize);
};

/**
 * @brief
 *
 */
class CMonitorWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief
     *
     * @param _tools
     * @param parent
     */
    explicit CMonitorWidget( CToolsWidget *_tools, QWidget *parent = 0);
    /**
     * @brief
     *
     */
    ~CMonitorWidget();
    QMap< QString, QSet< QPair< int, int> > > seedsPos; /**< TODO */
    QMap< QString, QColor> seedsColor; /**< TODO */
    QPixmap *image; /**< TODO */
    QString seedName; /**< TODO */
    QColor seedColor; /**< TODO */
private:
    CToolsWidget *tools; /**< TODO */
    QTimer* mainTimer; /**< TODO */
    bool dragging; /**< TODO */
public slots:
    /**
     * @brief
     *
     * @param QPixmap
     */
    void updateImage( QPixmap);
    /**
     * @brief
     *
     */
    void clearAllSeeds();
    /**
     * @brief
     *
     * @param _p
     */
    inline void setInputImage( QPixmap _p){ image = new QPixmap( _p);}
    /**
     * @brief
     *
     * @param _s
     * @param _c
     */
    inline void setCurrentSeed( QString _s, QColor _c){ seedName = _s; seedColor = _c; seedsColor[seedName] = seedColor;}
private slots:
    /**
     * @brief
     *
     * @param
     */
    void paintEvent(QPaintEvent *);
    /**
     * @brief
     *
     * @param event
     */
    void mouseMoveEvent(QMouseEvent *event);
signals:
    /**
     * @brief
     *
     * @param QPixmap
     */
    void imageLoaded1( QPixmap);
};

/**
 * @brief
 *
 */
class CViewerWidget : public QTabWidget
{
    Q_OBJECT
public:
    /**
     * @brief
     *
     * @param parent
     */
    CViewerWidget( QWidget *parent = 0);
    /**
     * @brief
     *
     */
    ~CViewerWidget();
private:
    /**
     * @brief
     *
     * @param event
     */
    void resizeEvent(QResizeEvent *event);
signals:
    /**
     * @brief
     *
     * @param QSize
     */
    void imageSizeLimitSignal( QSize);
};

/**
 * @brief
 *
 */
class CSeedWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief
     *
     * @param name
     * @param gp
     * @param parent
     */
    CSeedWidget( QString name, QButtonGroup *gp, QWidget *parent = 0);
    /**
     * @brief
     *
     */
    ~CSeedWidget();
    /**
     * @brief
     *
     * @return QColor
     */
    inline QColor getColor(){ return seedColor;}
    /**
     * @brief
     *
     * @return QString
     */
    inline QString getName(){ return seedName->text();}
private:
    QLabel *seedName; /**< TODO */
    QRadioButton *selected; /**< TODO */
    QColor seedColor; /**< TODO */
    QPushButton *colorSelectBtn; /**< TODO */
    QColorDialog *colorSelector; /**< TODO */
private slots:
    /**
     * @brief
     *
     */
    void setColor();
    /**
     * @brief
     *
     * @param is
     */
    void emitSeedChanged( bool is);
signals:
    /**
     * @brief
     *
     * @param QString
     * @param QColor
     */
    void seedChanged( QString, QColor);
};

/**
 * @brief
 *
 */
class CSeedSelectionWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief
     *
     * @param parent
     */
    CSeedSelectionWidget( QWidget *parent = 0);
    /**
     * @brief
     *
     */
    ~CSeedSelectionWidget();
private:
    QPushButton *addSeedBtn; /**< TODO */
    QGridLayout *lOut; /**< TODO */
    QList< CSeedWidget*> seeds; /**< TODO */
    QButtonGroup *buttonsGroup; /**< TODO */
public slots:
    /**
     * @brief
     *
     */
    void removeSeeds();
private slots:
    /**
     * @brief
     *
     */
    void addNewSeed();
    /**
     * @brief
     *
     * @param _n
     * @param _c
     */
    void emitSelectedSeed( QString _n, QColor _c);
signals:
    /**
     * @brief
     *
     * @param QString
     * @param QColor
     */
    void selectedSeedSignal( QString, QColor);
};


/**
 * @brief
 *
 */
class CStatusText
{
    public:
    /**
     * @brief
     *
     * @param _text
     * @param _color
     * @param _size
     */
    CStatusText(QString _text = "", QColor _color = QColor("black"), int _size = 12)
    {
        text= _text;
        color = _color;
        size = _size;
    }

    QString text; /**< TODO */
    QColor color; /**< TODO */
    int size; /**< TODO */
};

/**
 * @brief
 *
 */
class CStatusPrinter
{
    public:
    /**
     * @brief
     *
     */
    CStatusPrinter() {}
    QQueue<CStatusText> textBuffer; /**< TODO */ /**< TODO */
};

/**
 * @brief
 *
 */
class CStatusWidget : public QDockWidget
{
    Q_OBJECT
public:
    /**
     * @brief
     *
     * @param _statusPrinter
     */
    CStatusWidget(CStatusPrinter* _statusPrinter);
    /**
     * @brief
     *
     */
    ~CStatusWidget();
    QTextEdit *statusText; /**< TODO */
    QLabel *titleLbl; /**< TODO */
    QTextDocument content; /**< TODO */
public slots:
    /**
     * @brief
     *
     * @param str
     * @param color
     */
    void write(QString str, QColor color = QColor("black"));
    /**
     * @brief
     *
     */
    void update();
    /**
     * @brief
     *
     * @param size
     */
    void getSizeFromViewer(QSize size);
private:
    CStatusPrinter *statusPrinter; /**< TODO */
    QTime logTime; /**< TODO */
signals:
    /**
     * @brief
     *
     * @param bool
     */
    void closeSignal(bool);
protected:
    /**
     * @brief
     *
     * @param
     */
    void closeEvent(QEvent*);
};

extern CStatusPrinter *printer; /**< TODO */

#endif // WIDGETS_H
