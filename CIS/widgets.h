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
 * @brief CTabDockWidget : class in charge of creating the tabs
 * when a new seed is selected in the GUI
 *
 */
class CTabDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    /**
     * @brief CTabDockWidget : function in charge of creating a new tab
     * every time the user selects a new seed for multisegmentation
     *
     * @param parent : constructing a QDockWidget with parent parent
     */
    CTabDockWidget(QWidget* parent);
    /**
     * @brief ~CTabDockWidget : Destructor member function for CTabDockWidget
     *
     */
    ~CTabDockWidget();
    QTabWidget *tabs; /**< TODO */
    QWidget* w; /**< TODO */
signals:
    /**
     * @brief resized : ???? WHERE IS IT ??
     *
     * @param QSize
     */
    void resized( QSize);
};

/**
 * @brief : CToolsWidged : class in charge of handling the tools
 * needed to manage the GUI. It handles the functions of updating
 * the input image selected by the user (as QPixmap and QImage),the
 * function in charge of converting the beta value given by the slider
 * into double so that it can be used in the segmentation code, and
 * the function in charge of resizing the input image.
 *
 */
class CToolsWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief CToolsWidget : function in charge of handling the size
     * postion and labels of the different push-buttons,line
     * inputs and slider in the GUI. It connects the push-buttons
     * loadButton and bethaSlider with the funtions loadSlot and
     * updateBethaValue respectevely.
     *
     * @param parent : constructing a QDockWidget with parent parent
     */
    CToolsWidget(QWidget *parent = 0);
    /**
     * @brief ~CToolsWidget : Destructor member function for CToolsWidget
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
     * @brief loadSlot : function in charge of getting the input image selected
     * by the user by following a string. It emits the signals:
     * imageLoadedQPixmap and imageLoadedQImage.
     *
     */
    void loadSlot();
    /**
     * @brief updateBethaValue : function in charge of converting the value given
     * by the slider into a decimal value which is then used in the segmentation
     * code
     *
     * @param _val : value given by the slider in the GUI
     */
    void updateBethaValue(int _val);
signals:
    /**
     * @brief imageLoadedQPixmap :
     *
     * @param QPixmap
     * @param bool
     */
    void imageLoadedQPixmap( QPixmap, bool);
    /**
     * @brief imageLoadedQImage :
     *
     * @param QImage
     */
    void imageLoadedQImage( QImage);
public slots:
    /**
     * @brief setImageSizeLimit :
     *
     * @param QSize
     */
    void setImageSizeLimit( QSize);
};

/**
 * @brief CMonitorWidget : class in charge of handling showing the images
 * in the monitor. It contains the following functions: setting and updating the
 * image on the screen; and painting, setting and clearing the selected seeds.
 *
 */
class CMonitorWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief CMonitorWidget :
     *
     * @param _tools
     * @param parent
     */
    explicit CMonitorWidget( CToolsWidget *_tools, QWidget *parent = 0);
    /**
     * @brief CMonitorWidget : Destructor member function for CMonitorWidget
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
     * @brief updateImage : function in charge of updating the image in
     * the GUI. Every time the user selects a new image this function shows
     * the new image in the first tab of the application.
     *
     * @param QPixmap : image selected by the user. The parameter is gotten from
     * imageLoadedQPixmap signal which sends the QPixmap to be read.
     *
     */
    void updateImage( QPixmap);
    /**
     * @brief clearAllSeeds : function in charge of deleting the painted seeds of
     * the previous image when the user selects a new one to be segmented.
     *
     *
     */
    void clearAllSeeds();
    /**
     * @brief setInputImage :
     *
     * @param _p
     */
    inline void setInputImage( QPixmap _p){ image = new QPixmap( _p);}
    /**
     * @brief setCurrentSeed :  ??? WHERE IS IT???
     *
     * @param _s
     * @param _c
     */
    inline void setCurrentSeed( QString _s, QColor _c){ seedName = _s; seedColor = _c; seedsColor[seedName] = seedColor;}
private slots:
    /**
     * @brief paintEvent : function in charge of painting the seeds in the selected
     * image ->......???
     *
     * @param
     */
    void paintEvent(QPaintEvent *);
    /**
     * @brief mouseMoveEvent : it follows the position of the mouse to draw
     * the seeds on the selected image ->.... ???
     *
     * @param event
     */
    void mouseMoveEvent(QMouseEvent *event);
signals:
    /**
     * @brief imageLoaded1 : ??? WHERE IS IT ???
     *
     * @param QPixmap
     */
    void imageLoaded1( QPixmap);
};

/**
 * @brief CViewerWidget : tabs...?
 *
 */
class CViewerWidget : public QTabWidget
{
    Q_OBJECT
public:
    /**
     * @brief CViewerWidget :
     *
     * @param parent
     */
    CViewerWidget( QWidget *parent = 0);
    /**
     * @brief ~CViewerWidget : Destructor member function for CViewerWidget
     *
     */
    ~CViewerWidget();
private:
    /**
     * @brief resizeEvent :
     *
     * @param event
     */
    void resizeEvent(QResizeEvent *event);
signals:
    /**
     * @brief imageSizeLimitSignal :
     *
     * @param QSize
     */
    void imageSizeLimitSignal( QSize);
};

/**
 * @brief CSeedWidget :
 *
 */
class CSeedWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief CSeedWidget :
     *
     * @param name
     * @param gp
     * @param parent
     */
    CSeedWidget( QString name, QButtonGroup *gp, QWidget *parent = 0);
    /**
     * @brief ~CSeedWidget : Destructor member function for CSeedWidget
     *
     */
    ~CSeedWidget();
    /**
     * @brief getColor : inline function in charge of getting the color selected by the user
     * to paint the seeds in the input image. Different colors correspond to different seeds,
     * this in order to differentiate between background and foreground seeds.
     *
     * @return QColor :
     */
    inline QColor getColor(){ return seedColor;}
    /**
     * @brief getName : ??? WHERE IS IT??
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
     * @brief emitSeedChanged :
     *
     * @param is
     */
    void emitSeedChanged( bool is);
signals:
    /**
     * @brief seedChanged :
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
     * @brief CSeedSelectionWidget :
     *
     * @param parent
     */
    CSeedSelectionWidget( QWidget *parent = 0);
    /**
     * @brief ~CSeedSelectionWidget : Destructor member function for CSeedSelectionWidget
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
     * @brief removeSeeds :
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
     * @brief emitSelectedSeed :
     *
     * @param _n
     * @param _c
     */
    void emitSelectedSeed( QString _n, QColor _c);
signals:
    /**
     * @brief selectedSeedSignal :
     *
     * @param QString
     * @param QColor
     */
    void selectedSeedSignal( QString, QColor);
};


/**
 * @brief CStatusText :
 *
 */
class CStatusText
{
    public:
    /**
     * @brief CStatusText :
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
 * @brief CStatusPrinter :
 *
 */
class CStatusPrinter
{
    public:
    /**
     * @brief CStatusPrinter :
     *
     */
    CStatusPrinter() {}
    QQueue<CStatusText> textBuffer; /**< TODO */ /**< TODO */
};

/**
 * @brief CStatusWidget :
 *
 */
class CStatusWidget : public QDockWidget
{
    Q_OBJECT
public:
    /**
     * @brief CStatusWidget :
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
     * @brief write :
     *
     * @param str
     * @param color
     */
    void write(QString str, QColor color = QColor("black"));
    /**
     * @brief update :
     *
     */
    void update();
    /**
     * @brief getSizeFromViewer :
     *
     * @param size
     */
    void getSizeFromViewer(QSize size);
private:
    CStatusPrinter *statusPrinter; /**< TODO */
    QTime logTime; /**< TODO */
signals:
    /**
     * @brief closeSignal :
     *
     * @param bool
     */
    void closeSignal(bool);
protected:
    /**
     * @brief closeEvent :
     *
     * @param
     */
    void closeEvent(QEvent*);
};

extern CStatusPrinter *printer; /**< TODO */

#endif // WIDGETS_H
