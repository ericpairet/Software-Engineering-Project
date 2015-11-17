#include "widgets.h"

//Monitor Widget for showing the image
CMonitorWidget::CMonitorWidget(CToolsWidget *_tools, QWidget *parent)
    : QWidget(parent)
{
    tools = _tools;
    this->setFixedSize(690,360); //1,1
    image = new QPixmap;
//    image2 = NULL;
    //image = new QPixmap(1,1);
    image2 = new QPixmap; //(0,0)
    mainTimer = new QTimer();
    mainTimer->setInterval(30);
    mainTimer->start();
    dragging = false;
    this->setStyleSheet("background-color:black;");
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
    if( image2 != NULL)
        painter.drawPixmap( image->width(), 0, *image2); //100,0,image2
    resize( image->width(), image->height());
    painter.setPen(Qt::red);
    for( QSet< QPair< int, int> >::Iterator it = fgSeeds.begin(); it != fgSeeds.end(); ++it)
        painter.drawPoint( it->first, it->second);
    painter.setPen(Qt::blue);
    for( QSet< QPair< int, int> >::Iterator it = bgSeeds.begin(); it != bgSeeds.end(); ++it)
        painter.drawPoint( it->first, it->second);
}

//Updating Input Image
void CMonitorWidget::updateImage( QPixmap p)
{
    QPixmap output( p.width(), p.height());
    output.fill(Qt::transparent);
    QPainter painter( &output);
    painter.drawPixmap(0, 0, p);
    image = new QPixmap(output);
    //this->setFixedSize(p.width(), p.height());
    //this->updateGeometry();
}

//Updating Output Image
void CMonitorWidget::updateImageR( QPixmap p)
{
//    image2 = new QPixmap;
    QPixmap output( p.width(), p.height());
    output.fill(Qt::transparent);
    QPainter painter( &output);
    //Setting the mask for the output Image
    QBitmap mask = p.createMaskFromColor(Qt::MaskInColor);
    QPixmap tempImage = image->copy( image->rect());
    tempImage.setMask(mask);
    painter.drawPixmap(0, 0, tempImage);
    painter.drawPixmap(p.width(), 0 , p);
    image2 = new QPixmap(output);
//    image2->setMask(mask);
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
    delete image2;
    image2 = new QPixmap(0,0);
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
    QString path = QFileDialog::getOpenFileName(this, tr("Load image"), "./../Images", tr("Image files (*.jpg *.jpeg *.png)"));
    if(path != "")
    {
        //QPixmap pic( path);
        QPixmap pic = path;
        QImage img = QImage(path);
        emit imageLoaded( pic);
        emit imageLoaded2( img);
    }
    imagePath = path;
}

