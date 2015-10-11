#include "widgets.h"

//Monitor Widget for showing the image
CMonitorWidget::CMonitorWidget(QWidget *parent)
    : QWidget(parent)
{
    this->setFixedSize(690,390);
    image = new QPixmap( 690,390);
    mainTimer = new QTimer();
    mainTimer->setInterval(30);
    mainTimer->start();
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
}

void CMonitorWidget::updateImage( QPixmap p)
{
    QPixmap output( p.width(), p.height());
    output.fill(Qt::transparent);
    QPainter painter( &output);
    painter.drawPixmap( 0, 0, p);
    image = new QPixmap(output);
}

//Tools Widget for handling the tools (this class is temperory, we can put everything we need here for know, then we can separate them
CToolsWidget::CToolsWidget( QWidget *parent)
    : QWidget(parent)
{
    QGridLayout *lOut = new QGridLayout(this);
    setLayout( lOut);
    loadLabel = new QLabel("Load image");
    loadButton = new QPushButton("Browse");
    lOut->addWidget( loadLabel, 0, 0);
    lOut->addWidget( loadButton, 0, 1);

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
