#include "widgets.h"

//Monitor Widget for showing the image
CMonitorWidget::CMonitorWidget(CToolsWidget *_tools, QWidget *parent)
    : QWidget(parent)
{
    tools = _tools;
    this->setFixedSize(690,390);
    image = new QPixmap( 690,390);
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
    resize( image->width(), image->height());
    painter.setPen(Qt::red);
    for(int i = 0; i < fgSeeds.count(); i++)
        painter.drawPoint( fgSeeds.at(i).first, fgSeeds.at(i).second);
    painter.setPen(Qt::blue);
    for(int i = 0; i < bgSeeds.count(); i++)
        painter.drawPoint( bgSeeds.at(i).first, bgSeeds.at(i).second);
}

void CMonitorWidget::updateImage( QPixmap p)
{
    QPixmap output( p.width(), p.height());
    output.fill(Qt::transparent);
    QPainter painter( &output);
    painter.drawPixmap( 0, 0, p);
    image = new QPixmap(output);
}

void CMonitorWidget::mouseMoveEvent(QMouseEvent *event)
{
    int screenPosX = event->pos().x();
    int screenPosY = event->pos().y();
    qDebug() << screenPosX << screenPosY;
    if( tools->fgRadioButton->isChecked())
        fgSeeds.append( QPair< int, int>( screenPosX, screenPosY));
    else
        bgSeeds.append( QPair< int, int>( screenPosX, screenPosY));
}

//-------------------------------------------------------------------------------

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
    lOut->addWidget( loadLabel, 0, 0);
    lOut->addWidget( loadButton, 0, 1);
    lOut->addWidget( seedLabel, 1, 0);
    lOut->addWidget( fgRadioButton, 1, 1);
    lOut->addWidget( bgRadioButton, 2, 1);

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
        QPixmap pic( path);
        emit imageLoaded( pic);
    }
}

