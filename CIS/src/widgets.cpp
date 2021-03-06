#include "widgets.h"

//Debuggger :
void debug(QString text, QColor color)
{
    printer->textBuffer.enqueue(CStatusText(text,color));
}

CTabDockWidget::CTabDockWidget(QWidget* parent)
    : QDockWidget(parent)
{
    w = new QWidget(this);
    tabs = new QTabWidget(w);
    QGridLayout *layout = new QGridLayout(w);
    tabs->setTabPosition(QTabWidget::East);
    layout->addWidget(tabs,0,0);
    w->setLayout(layout);
    setWidget(w);
    setTitleBarWidget( new QWidget());
}


CTabDockWidget::~CTabDockWidget()
{
    delete tabs;
    delete w;
}

//Monitor Widget for showing the image
CMonitorWidget::CMonitorWidget(CToolsWidget *_tools, QWidget *parent)
    : QWidget(parent)
{
    tools = _tools;
    image = new QPixmap;
    mainTimer = new QTimer();
    mainTimer->setInterval(30);
    mainTimer->start();
    dragging = false;
    this->setStyleSheet("background-color:black;");
    seedName = "";
    connect(mainTimer,SIGNAL(timeout()),this,SLOT(repaint()));
}

CMonitorWidget::CMonitorWidget(QPixmap *_im, QMap<QString, QColor> _sColors, QMap<QString, QSet<QPair<int, int> > > _sPos, QWidget *parent)
    : QWidget(parent)
{
    image = _im;
    seedsColor = _sColors;
    seedsPos = _sPos;
}

CMonitorWidget::~CMonitorWidget()
{

}

void CMonitorWidget::paintEvent(QPaintEvent */*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPixmap( 0, 0, *image);
    for( QMap< QString, QSet< QPair< int, int> > >::iterator it = seedsPos.begin(); it != seedsPos.end(); it++)
    {
        painter.setPen( seedsColor[it.key()]);
        for( QSet< QPair< int, int> >::iterator itt = it->begin(); itt != it->end(); ++itt)
            painter.drawPoint( itt->first, itt->second);
    }
}

//Updating Input Image
void CMonitorWidget::updateImage( QPixmap p)
{
    image = new QPixmap( p);
}

void CMonitorWidget::mouseMoveEvent(QMouseEvent *event)
{
    int screenPosX = event->pos().x();
    int screenPosY = event->pos().y();
    debug( QString("Point [%1,%2] added to the seeds").arg(screenPosX).arg(screenPosY), seedColor);
    int size = tools->penSize->text().toInt();
    if( image->rect().contains( screenPosX, screenPosY) && seedName != "")
    {
        seedsPos[seedName].insert( QPair< int, int>( screenPosX, screenPosY));
        for(int i = -size+1 ; i < size; i++)
            for( int j = -size+1 ; j < size; j++)
                if( sqrt( i*i+j*j) < size && image->rect().contains( screenPosX+i, screenPosY+j))
                    seedsPos[seedName].insert( QPair< int, int>( screenPosX+i, screenPosY+j));
    }
}

void CMonitorWidget::clearAllSeeds()
{
    seedsColor.clear();
    seedsPos.clear();
}

//Viewer widget;
CViewerWidget::CViewerWidget( QWidget *parent)
    : QTabWidget(parent)
{

}

CViewerWidget::~CViewerWidget()
{

}

void CViewerWidget::resizeEvent(QResizeEvent *event)
{
    emit imageSizeLimitSignal( event->size());
}

//Tools Widget for handling the tools (this class is temperory, we can put everything we need here for know, then we can separate them
CToolsWidget::CToolsWidget( QWidget *parent)
    : QWidget(parent)
{
//    imageLimit = QSize( 1024, 1024);

    QGridLayout *lOut = new QGridLayout(this);
    setLayout( lOut);
    loadLabel = new QLabel("Load image");
    loadButton = new QPushButton("Browse");
    penLable = new QLabel("Pen size");
    seedsLbl = new QLabel("Seeds");
    segLbl = new QLabel("Segmentation");
    penSize = new QLineEdit("1");
    execBtn = new QPushButton("Execute");
    clearSeedsBtn = new QPushButton("Clear");
    bethaSlider = new QSlider(this);
    bethaSlider->setRange(0, 10000);
    bethaSlider->setValue(50);
    bethaSlider->setOrientation(Qt::Vertical);
    bethaSlider->setTickInterval(1);
    bethalbl = new QLabel("Beta :\t0.005");
    lOut->addWidget( loadLabel, 0, 0);
    lOut->addWidget( loadButton, 0, 1);
    lOut->addWidget( penLable, 1,0);
    lOut->addWidget( penSize, 1,1);
    lOut->addWidget( seedsLbl, 2,0);
    lOut->addWidget( clearSeedsBtn, 2,1);
    lOut->addWidget( segLbl, 3,0);
    lOut->addWidget( execBtn, 3,1);
    lOut->addWidget( bethaSlider, 4, 0, Qt::AlignHCenter);
    lOut->addWidget( bethalbl, 4, 1);

    connect( loadButton, SIGNAL(pressed()), this, SLOT(loadSlot()));
    connect( bethaSlider, SIGNAL( valueChanged(int)), this, SLOT(updateBethaValue(int)));
}

CToolsWidget::~CToolsWidget()
{

}

void CToolsWidget::loadSlot()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Load image"), "./../Images", tr("Image files (*.jpg *.jpeg *.png)"));
    if(path != "")
    {

        QPixmap pic = path;
        QImage img = QImage(path);
        if( img.height() > imageLimit.height())
        {
            img = img.scaledToHeight(imageLimit.height());
            pic = pic.scaledToHeight(imageLimit.height());
        }
        if( img.width() > imageLimit.width())
        {
            img = img.scaledToWidth(imageLimit.width());
            pic = pic.scaledToWidth(imageLimit.width());
        }
        emit imageLoadedQPixmap( pic, true);
        emit imageLoadedQImage( img);
        debug(QString("%1 loaded successfully !").arg(path.toStdString().substr(path.lastIndexOf('/')+1).c_str()), "darkcyan");
    }
}

void CToolsWidget::updateBethaValue(int _val)
{
    bethalbl->setText(QString("Beta :\t%1").arg(_val/10000.0));
}

void CToolsWidget::setImageSizeLimit(QSize _s)
{
    imageLimit = _s;
}

//seed widget:
CSeedWidget::CSeedWidget(QString name, QButtonGroup *gp, QWidget *parent)
    : QWidget( parent)
{
    QGridLayout *lOut = new QGridLayout( this);
    selected = new QRadioButton( parent);
    lOut->addWidget( selected, 0, 0);
    seedName = new QLabel( name);
    lOut->addWidget( seedName, 0, 1);
    colorSelectBtn = new QPushButton();
    lOut->addWidget( colorSelectBtn, 0, 2);
    this->setLayout( lOut);
    gp->addButton( selected);
    selected->setChecked(false);
    connect( colorSelectBtn, SIGNAL( clicked()), this, SLOT(setColor()));
    connect( selected, SIGNAL( toggled(bool)), this, SLOT( emitSeedChanged(bool)));
    emit colorSelectBtn->clicked();
}

CSeedWidget::~CSeedWidget()
{

}

void CSeedWidget::setColor()
{
    seedColor = QColorDialog::getColor(QColor( rand()%256, rand()%256, rand()%256));
    colorSelectBtn->setStyleSheet(QString("background-color:#%1%2%3;").arg(QString::number(seedColor.red(),16), 2, QChar('0')).arg(QString::number(seedColor.green(),16), 2, QChar('0')).arg(QString::number(seedColor.blue(),16), 2, QChar('0')));
    colorSelectBtn->setAutoFillBackground( true);
    colorSelectBtn->update();
    selected->setChecked(false);
}

void CSeedWidget::emitSeedChanged(bool is)
{
    if( is)
        emit seedChanged( seedName->text(), seedColor);
}

//Seeds selection Widget
CSeedSelectionWidget::CSeedSelectionWidget(QWidget *parent)
    : QWidget( parent)
{
    lOut = new QGridLayout( this);
    addSeedBtn = new QPushButton( "Add Seed !");
    lOut->addWidget( addSeedBtn);
    this->setLayout( lOut);
    buttonsGroup = new QButtonGroup( this);
    connect( addSeedBtn, SIGNAL(clicked()), this, SLOT(addNewSeed()));
}

CSeedSelectionWidget::~CSeedSelectionWidget()
{

}

void CSeedSelectionWidget::addNewSeed()
{
    QString name = (seeds.isEmpty()?"BackGround":QString("ForeGround %1").arg(seeds.count()));
    CSeedWidget *s = new CSeedWidget( name, buttonsGroup, this);
    seeds.append( s);
    lOut->addWidget( seeds.last(), seeds.count(), 0);
    update();
    connect( s, SIGNAL( seedChanged(QString,QColor)), this, SLOT( emitSelectedSeed(QString,QColor)));
}

void CSeedSelectionWidget::emitSelectedSeed( QString _n, QColor _c)
{
    emit selectedSeedSignal( _n, _c);
}

void CSeedSelectionWidget::removeSeeds()
{
    for( int i = 0; i < seeds.count(); i++)
    {
        lOut->removeWidget( seeds.at(i));
        delete seeds.at(i);
    }
    seeds.clear();
    update();
}

//Status widget
CStatusPrinter *printer;

CStatusWidget::CStatusWidget(CStatusPrinter* _statusPrinter)
{
    statusPrinter = _statusPrinter;
    logTime.start();
    this->setAllowedAreas(Qt::BottomDockWidgetArea);
    this->setFeatures(QDockWidget::NoDockWidgetFeatures);
    statusText = new QTextEdit(this);
    titleLbl = new QLabel(tr("Messages"));
    QWidget* w = new QWidget(this);
    QGridLayout* layout = new QGridLayout(w);
    layout->addWidget(statusText, 0, 0);
    layout->setAlignment(statusText, Qt::AlignLeft);
    layout->setColumnMinimumWidth(0, 500);
    w->setLayout(layout);
    statusText->setReadOnly(true);
    this->setWidget(w);
}

CStatusWidget::~CStatusWidget()
{
    delete statusText;
    delete titleLbl;
}

void CStatusWidget::write(QString str, QColor color)
{
    if( statusText->textCursor().blockNumber() > 1000 )
        statusText->clear();
    statusText->setTextColor(color);
    statusText->append(QString::number(logTime.elapsed()) + " : " + str);
    statusText->setTextColor(QColor("black"));
}

void CStatusWidget::update()
{
    CStatusText text;
    while(!statusPrinter->textBuffer.isEmpty())
    {
        text = statusPrinter->textBuffer.dequeue();
        write(text.text, text.color);
    }
}

void CStatusWidget::closeEvent(QEvent*)
{
    emit closeSignal(false);
}

void CStatusWidget::getSizeFromViewer( QSize size)
{
    statusText->resize( size.width()+270, 200);
}
