#include "widgets.h"

//Monitor Widget for showing the image
CMonitorWidget::CMonitorWidget(CToolsWidget *_tools, QWidget *parent)
    : QWidget(parent)
{
    tools = _tools;
    this->setFixedSize(690,390);
    image = new QPixmap;
    image2 = new QPixmap;
    //image = new QPixmap(1,1);
    //image2 = new QPixmap(1,1);
    mainTimer = new QTimer();
    mainTimer->setInterval(30);
    mainTimer->start();
    dragging = false;
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
    painter.drawPixmap( image->width(), 0, *image2); //100,0,image2
    resize( image->width(), image->height());
    painter.setPen(Qt::red);
    for( QSet< QPair< int, int> >::Iterator it = fgSeeds.begin(); it != fgSeeds.end(); ++it)
        painter.drawPoint( it->first, it->second);
    painter.setPen(Qt::blue);
    for( QSet< QPair< int, int> >::Iterator it = bgSeeds.begin(); it != bgSeeds.end(); ++it)
        painter.drawPoint( it->first, it->second);
}

void CMonitorWidget::updateImage( QPixmap p)
{
    QPixmap output( p.width(), p.height());
    output.fill(Qt::transparent);
    QPainter painter( &output);
    painter.drawPixmap(0, 0, p);
    image = new QPixmap(output);
}

void CMonitorWidget::updateImageR( QPixmap p)
{
    QPixmap output( p.width(), p.height());
    output.fill(Qt::transparent);
    QPainter painter( &output);
    //painter.translate(100,0);
    painter.drawPixmap(0, 0, p);
    image2 = new QPixmap(output);
}

void CMonitorWidget::mouseMoveEvent(QMouseEvent *event)
{
    int screenPosX = event->pos().x();
    int screenPosY = event->pos().y();
    qDebug() << screenPosX << screenPosY;
    int size = tools->penSize->text().toInt();
    if( tools->fgRadioButton->isChecked() && image->rect().contains( screenPosX, screenPosY))
    {
        fgSeeds.insert( QPair< int, int>( screenPosX, screenPosY));
        for(int i = -size+1 ; i < size; i++)
            for( int j = -size+1 ; j < size; j++)
                if( sqrt( i*i+j*j) < size && image->rect().contains( screenPosX+i, screenPosY+j))
                    fgSeeds.insert( QPair< int, int>( screenPosX+i, screenPosY+j));
    }
    else if( tools->bgRadioButton->isChecked() && image->rect().contains( screenPosX, screenPosY))
    {
        bgSeeds.insert( QPair< int, int>( screenPosX, screenPosY));
        for(int i = -size+1 ; i < size; i++)
            for( int j = -size+1 ; j < size; j++)
                if( sqrt( i*i+j*j) <= size && image->rect().contains( screenPosX+i, screenPosY+j))
                    bgSeeds.insert( QPair< int, int>( screenPosX+i, screenPosY+j));
    }
    else
    {
        fgSeeds.remove( QPair< int, int>( screenPosX, screenPosY));
        bgSeeds.remove( QPair< int, int>( screenPosX, screenPosY));
        for(int i = -size+1 ; i < size; i++)
            for( int j = -size+1 ; j < size; j++)
                if(sqrt( i*i+j*j) <= size)
                {
                    fgSeeds.remove( QPair< int, int>( screenPosX+i, screenPosY+j));
                    bgSeeds.remove( QPair< int, int>( screenPosX+i, screenPosY+j));
                }
    }
}

void CMonitorWidget::clearAllSeeds()
{
    fgSeeds.clear();
    bgSeeds.clear();
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
    lOut->addWidget( loadLabel, 0, 0);
    lOut->addWidget( loadButton, 0, 1);
    lOut->addWidget( seedLabel, 1, 0);
    lOut->addWidget( fgRadioButton, 1, 1);
    lOut->addWidget( bgRadioButton, 2, 1);
    lOut->addWidget( rmvRadioButton, 3, 1);
    lOut->addWidget( penSize, 2,0);
    lOut->addWidget( clearSeedsBtn, 3,0);
    lOut->addWidget( execBtn, 4,0);

    connect( loadButton, SIGNAL(pressed()), this, SLOT(loadSlot()));
}

CToolsWidget::~CToolsWidget()
{

}

void CToolsWidget::loadSlot()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Load image"), "./", tr("Image files (*.jpg *.jpeg *.png)"));
    if(path != "")
    {
        //QPixmap pic( path);
        QPixmap pic = path;
        emit imageLoaded( pic);
    }
    imagePath = path;
}

