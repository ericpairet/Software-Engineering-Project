#include "widgets.h"


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
    qDebug() << screenPosX << screenPosY;
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

//Tools Widget for handling the tools (this class is temperory, we can put everything we need here for know, then we can separate them
CToolsWidget::CToolsWidget( QWidget *parent)
    : QWidget(parent)
{
    QGridLayout *lOut = new QGridLayout(this);
    setLayout( lOut);
    loadLabel = new QLabel("Load image");
    loadButton = new QPushButton("Browse");
    seedLabel = new QLabel("Seed selection:");
    fgRadioButton = new QRadioButton("ForeGround");
    fgRadioButton->setChecked( true);
    bgRadioButton = new QRadioButton("BackGroung");
    rmvRadioButton = new QRadioButton("Remove");
    penSize = new QLineEdit("1");
    execBtn = new QPushButton("Execute");
    clearSeedsBtn = new QPushButton("Clear");
    bethaSlider = new QSlider(this);
    bethaSlider->setRange(0, 10000);
    bethaSlider->setValue(50);
    bethaSlider->setOrientation(Qt::Horizontal);
    bethaSlider->setTickInterval(1);
    bethaName = new QLabel("Betha :");
    bethaVal = new QLabel("0.005");
    lOut->addWidget( loadLabel, 0, 0);
    lOut->addWidget( loadButton, 0, 1);
    lOut->addWidget( seedLabel, 1, 0);
    lOut->addWidget( fgRadioButton, 1, 1);
    lOut->addWidget( bgRadioButton, 2, 1);
    lOut->addWidget( rmvRadioButton, 3, 1);
    lOut->addWidget( penSize, 2,0);
    lOut->addWidget( clearSeedsBtn, 3,0);
    lOut->addWidget( execBtn, 4,0);
    lOut->addWidget( bethaName, 5, 0);
    lOut->addWidget( bethaVal, 5, 1);
    lOut->addWidget( bethaSlider, 6,0, 1, 2);

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
        //QPixmap pic( path);
        QPixmap pic = path;
        QImage img = QImage(path);
        emit imageLoaded( pic, true);
        emit imageLoaded2( img);
    }
    imagePath = path;
}

void CToolsWidget::updateBethaValue(int _val)
{
    bethaVal->setText(QString("%1").arg(_val/10000.0));
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

    connect( colorSelectBtn, SIGNAL( clicked()), this, SLOT(setColor()));
    connect( selected, SIGNAL( clicked(bool)), this, SLOT( emitSeedChanged(bool)));
    emit colorSelectBtn->clicked(true);
}

CSeedWidget::~CSeedWidget()
{

}

void CSeedWidget::setColor()
{
    seedColor = QColorDialog::getColor(QColor( rand()%256, rand()%256, rand()%256));
    colorSelectBtn->setStyleSheet(QString("background-color:#%1%2%3;").arg(QString::number(seedColor.red(),16)).arg(QString::number(seedColor.green(),16)).arg(QString::number(seedColor.blue(),16)));
    colorSelectBtn->setAutoFillBackground( true);
    colorSelectBtn->update();
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
