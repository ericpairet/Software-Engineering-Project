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
 * @brief Debugger : this is a global function which handles the debugging and stuff
 * being printed in statusWidget, it is being intoduced just for ease of use and the
 * debugger should be accessible all over the code.
 * This functin makes using the debugger easier and also we dont need to send the
 * pointer to printer class to every single calss.
 *
 * @param text : The input text we want to show
 * @param color : The color we want to use for printing the text, coud use QColor
 * or common coller names, or RGB hex values : #RRGGBB
 */
void debug(QString text, QColor color = "black");

/**
 * @brief CTabDockWidget : class in charge of creating the tabs and storing them in a
 * dockable widget, basically it's a combination of QTabWidget and QDickWidget which
 * has the properties of both, and customized.
 *
 */
class CTabDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    /**
     * @brief CTabDockWidget : class cuntructor, constructing a QDockWidget with
     * given parent
     *
     * @param parent : constructing a QDockWidget with parent parent
     */
    CTabDockWidget(QWidget* parent);
    /**
     * @brief ~CTabDockWidget : Destructor member function for CTabDockWidget
     *
     */
    ~CTabDockWidget();
    QTabWidget *tabs; /**< The tab widget storing all the tabs */
    QWidget* w; /**< the main widget which inherits the QDockWidget properties and holds widgets */
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
    QLineEdit *penSize; /**< The size of the pen is being used for drawing seeds */
    QPushButton *execBtn, *clearSeedsBtn; /**< The button which sends segmentation execution signal */
    QSlider *bethaSlider; /**< For choosing desired Betta value between predefined range */
private:
    QPushButton *loadButton; /**< The button that opens file dialog to choose the image */
    QLabel *loadLabel, *penLable, *seedsLbl, *segLbl; /**< Just lables for showing names */
    QLabel *bethalbl; /**< Labels for showing the slider name and selected Betta value */
    QSize imageLimit; /**< Stores the size of window to limit the image to this size */
private slots:
    /**
     * @brief loadSlot : slot in charge of getting the input image selected
     * by the user by following a string. It emits the signals:
     * imageLoadedQPixmap and imageLoadedQImage.
     *
     */
    void loadSlot();
    /**
     * @brief updateBethaValue : slot in charge of converting the value given
     * by the slider into a decimal value which is then used in the segmentation
     * code
     *
     * @param _val : value given by the slider in the GUI
     */
    void updateBethaValue(int _val);
signals:
    /**
     * @brief imageLoadedQPixmap : any image which has to be shown in viewer will
     * be emmited through this signal, this signal also cotrols the reseting of
     * environment, if it's a new image loaded by user this signal will cause
     * closing all the image tabs from previous segmentation and clears all the
     * previous selected seeds, otherwise it's the result of segmentation for one
     * of the seeds and will just be shown as a new tab
     *
     * @param QPixmap : The image signal carries
     * @param bool : whether or not it is a new image loaded by user
     */
    void imageLoadedQPixmap( QPixmap, bool);
    /**
     * @brief imageLoadedQImage : This signal carries the newly loaded image by user
     * as a QImage which is being used for converting the image to openCV matix in
     * CSegmentation class
     *
     * @param QImage : the newly loaded image by user
     */
    void imageLoadedQImage( QImage);
public slots:
    /**
     * @brief setImageSizeLimit : changes the image size limit being limited by mainWindow
     * size to limit the image
     *
     * @param QSize : maximum allowed size for image
     */
    void setImageSizeLimit( QSize);
};

/**
 * @brief CMonitorWidget : class in charge of handling and showing the images
 * in the monitor. Also selevting and storing the seeds. It contains the
 * following functions: setting and updating the image on the screen; and painting,
 * setting and clearing the selected seeds.
 *
 */
class CMonitorWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief CMonitorWidget : The cuntructor of monitorWidget class, it will
     * receive a pointer to previusly generated CToolsWidget which contains
     * tools information specially the seeds drawing pen size
     *
     * @param _tools : A pointer to CToolsWidget
     * @param parent : generator class parent
     */
    explicit CMonitorWidget( CToolsWidget *_tools, QWidget *parent = 0);
    CMonitorWidget( QPixmap *_im, QMap< QString, QColor> _sColors, QMap< QString, QSet< QPair< int, int> > > _sPos, QWidget *parent = 0);
    /**
     * @brief CMonitorWidget : Destructor member function for CMonitorWidget
     *
     */
    ~CMonitorWidget();
    QMap< QString, QSet< QPair< int, int> > > seedsPos; /**< A map from seeds name to a set of pairs of integers
    which contains all the coordinates for that seed and each pari indicates seeds position (X,Y) in image */
    QMap< QString, QColor> seedsColor; /**< A map from each seeds name to it's uniwue color */
    QPixmap *image; /**< The image being shown on monitor widget */
    QString seedName; /**< The active seed's name we are selecting/working with */
    QColor seedColor; /**< The active seed's color we are selecting/working with  */
private:
    CToolsWidget *tools; /**< Pointer to CToolsWidget */
    QTimer* mainTimer; /**< Timer used to update the monitor widget and show the selected/deleted seeds on image*/
    bool dragging; /**< If user is selecting the seeds or not */
public slots:
    /**
     * @brief updateImage : slot in charge of updating the image in
     * the GUI. Every time the user selects a new image this function shows
     * the new image in the first tab of the application.
     *
     * @param QPixmap : image selected by the user. The parameter is gotten from
     * imageLoadedQPixmap signal which sends the QPixmap to be read.
     *
     */
    void updateImage( QPixmap);
    /**
     * @brief clearAllSeeds : slot in charge of deleting the painted seeds of
     * the previous image when the user selects a new one to be segmented.
     *
     *
     */
    void clearAllSeeds();
    /**
     * @brief setCurrentSeed :  Activate a seed, select it as current seed and sets
     * current active seed name and color
     *
     * @param _s : The seed's name that is going to be activated
     * @param _c : The seed's color that is going to be activated
     */
    inline void setCurrentSeed( QString _s, QColor _c){ seedName = _s; seedColor = _c; seedsColor[seedName] = seedColor;}
private slots:
    /**
     * @brief paintEvent : function in charge refreshing the view, invoke from
     * QWdiget virtual paintEvent();
     *
     * @param : The event invokes painting, being handled by timer
     */
    void paintEvent(QPaintEvent *);
    /**
     * @brief mouseMoveEvent : it follows the position of the mouse to select
     * the seeds on the selected image
     *
     * @param event : The mouse event could be movement or clicks
     */
    void mouseMoveEvent(QMouseEvent *event);
};

/**
 * @brief CViewerWidget : A class to store all the images and segmentatin results
 * as separate widgets and show them in a QTabWidget
 *
 */
class CViewerWidget : public QTabWidget
{
    Q_OBJECT
public:
    /**
     * @brief CViewerWidget : The viewer class cunstructor
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
     * @brief resizeEvent : This function captures the widget size changes and
     * changes the limitation for image size
     *
     * @param event : Invokes by widget being resized
     */
    void resizeEvent(QResizeEvent *event);
signals:
    /**
     * @brief imageSizeLimitSignal : This signal carries the image limit size to
     * be delivered to CMonitorWidget so that it can resize the input image to this
     * limit
     *
     * @param QSize : image limit size
     */
    void imageSizeLimitSignal( QSize);
};

/**
 * @brief CSeedWidget : A class that handles on seed selection widget, such as its
 * Name, Color, Being active or not, etc.
 *
 */
class CSeedWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief CSeedWidget : The constractor of the widget
     *
     * @param name : Seed's name
     * @param gp : The button group it belongs to (needed for radio buttons)
     * @param parent
     */
    CSeedWidget( QString name, QButtonGroup *gp, QWidget *parent = 0);
    /**
     * @brief ~CSeedWidget : Destructor member function for CSeedWidget
     *
     */
    ~CSeedWidget();
    /**
     * @brief getColor : return the seed's color (getter for seed's color)
     * inline function in charge of getting the color selected by the user
     * to paint the seeds in the input image. Different colors correspond to different seeds,
     * this in order to differentiate between background and foreground seeds.
     *
     * @return QColor :
     */
    inline QColor getColor(){ return seedColor;}
    /**
     * @brief getName : return the seed's color (getter for seed's name)
     *
     * @return QString
     */
    inline QString getName(){ return seedName->text();}
private:
    QLabel *seedName; /**< The seed's name, also being shown as label */
    QRadioButton *selected; /**< The radio button indicates whether the seed is active */
    QColor seedColor; /**< The seed's name */
    QPushButton *colorSelectBtn; /**< Button that allows changing seeds color */
    QColorDialog *colorSelector; /**< The QQolorDialog for selecting a color for seed */
private slots:
    /**
     * @brief Seed's color setter, being used for selecting a color for the seed
     *
     */
    void setColor();
    /**
     * @brief emitSeedChanged : is being invoked whenever user acrivates a seed
     * it will deactivate the previous seed, activate this one and sends the
     * information to CMonitorWidget to change the activated seed
     *
     * @param is : Weather the seed is being activated or deactivated
     */
    void emitSeedChanged( bool is);
signals:
    /**
     * @brief seedChanged : This signal carries the new activated seed's name and color
     * to be delivered to CMonitorWidget and change the current seed
     *
     * @param QString : The seed's name
     * @param QColor : The seed's color
     */
    void seedChanged( QString, QColor);
};

/**
 * @brief Seeds class : this class created new seeds and keeps them all together in a single
 * widget, also holds the information that whichone is active.
 *
 */
class CSeedSelectionWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief CSeedSelectionWidget : The constructor of class
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
    QPushButton *addSeedBtn; /**< A push button to add new seed, the first seed always is backgroung and the rest forgrounds  */
    QGridLayout *lOut; /**< layout for handle seed widgets */
    QList< CSeedWidget*> seeds; /**< A list of CSeedWidget that stores all the created seed widgets */
    QButtonGroup *buttonsGroup; /**< being used to connect all the radio buttons together and track which one is active */
public slots:
    /**
     * @brief removeSeeds : slot in charge of deleting all the seeds in case of loading a new image
     * or seeds being cleared by user
     *
     */
    void removeSeeds();
private slots:
    /**
     * @brief addNewSeed : slot to add a new seed to previous seeds, being invoked by
     * pressing push button
     *
     */
    void addNewSeed();
    /**
     * @brief emitSelectedSeed : emits the selected seed's information as a signal
     *
     * @param _n : seed's name
     * @param _c : seed's color
     */
    void emitSelectedSeed( QString _n, QColor _c);
signals:
    /**
     * @brief selectedSeedSignal : The signal that carries selected seed's information
     * out of the class to be delivered by CMonitorWidget
     *
     * @param QString : seed's name
     * @param QColor : seed's color
     */
    void selectedSeedSignal( QString, QColor);
};


/**
 * @brief CStatusText : A class as a data type for texts used in Status Widget
 *
 */
class CStatusText
{
    public:
    /**
     * @brief CStatusText : A data being used in status printer, which carries the
     * text, text's color, and the size of the text user wants to print
     *
     * @param _text : the text string
     * @param _color : the text color
     * @param _size : the text size
     */
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

/**
 * @brief CStatusPrinter : A queue containing new text that is implemented as a class
 * user can add the new text in this queue and it's being added to text area by a timer
 * and being removed from the queue
 *
 */
class CStatusPrinter
{
    public:
    CStatusPrinter() {}
    QQueue<CStatusText> textBuffer; /**< TODO */ /**< TODO */
};

/**
 * @brief CStatusWidget : A class containing QTextEdit are that shows the debug there
 *
 */
class CStatusWidget : public QDockWidget
{
    Q_OBJECT
public:
    CStatusWidget(CStatusPrinter* _statusPrinter);
    ~CStatusWidget();
    QTextEdit *statusText; /**< The text area */
    QLabel *titleLbl;
    QTextDocument content; /**< Text is stored in this document */
public slots:
    /**
     * @brief write : writes new lines from printer queue with defined color in the text area
     *
     * @param str : the text
     * @param color : text's color
     */
    void write(QString str, QColor color = QColor("black"));
    /**
     * @brief update : updates the status bar information
     *
     */
    void update();
    /**
     * @brief getSizeFromViewer : if the window is being resized, this slot receives the new size
     * and resizes the text area to fit in the page
     *
     * @param size : new size for text area
     */
    void getSizeFromViewer(QSize size);
private:
    CStatusPrinter *statusPrinter; /**< The queue containg new text's information */
    QTime logTime; /**< Time from start of program, is printed before each output in status bar  */
signals:
    void closeSignal(bool);
protected:
    void closeEvent(QEvent*);
};

extern CStatusPrinter *printer; /**< Extern the pointer to this class to be accesible for any class includes this file */

#endif // WIDGETS_H
