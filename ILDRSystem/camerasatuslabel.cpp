#include "camerasatuslabel.h"

CameraStatusLabel::CameraStatusLabel(QWidget *parent)
	: QLabel(parent)
{
	//QPixmap pixmap = QPixmap(":/toolButton/green");
	//setPixmap(pixmap);
	setAutoFillBackground(true);
	eColor = Green;
	setFixedSize(12,12);
	m_showState = TRUE;
	QPalette palette;
	palette.setColor(QPalette::Background, Qt::green);
	setPalette(palette);
	blinkTimer = new QTimer;
	blinkTimer->setInterval(100);
	connect(blinkTimer, SIGNAL(timeout()), this, SLOT(slots_reStore()));
}

CameraStatusLabel::~CameraStatusLabel()
{

}

void CameraStatusLabel::slots_reStore()
{
	QPalette palette;
	palette.setColor(QPalette::Background, Qt::green);
	setPalette(palette);
	blinkTimer->stop();  
}

void CameraStatusLabel::BlinkCameraStatus(int iMode)
{
	if(m_showState)
	{
		QPalette palette;
		switch (iMode)
		{
		case 0:
			palette.setColor(QPalette::Background, Qt::green);
			break;
		case 1:
			palette.setColor(QPalette::Background, Qt::red);
			break;
		default:
			break;
		}
        setPalette(palette);
        blinkTimer->start();  
	}
}
void CameraStatusLabel::SetCameraStatus(int iMode)
{
	//QPixmap pixmap = QPixmap(":/toolButton/green");
	QPalette palette;
	switch (iMode)
	{
	case 0:
		palette.setColor(QPalette::Background, Qt::green);
		m_showState = true;
		break;
	case 1:
		//pixmap = QPixmap(":/toolButton/red");
		palette.setColor(QPalette::Background, Qt::red);
		break;
	case 2:
		//pixmap = QPixmap(":/toolButton/blue");
		palette.setColor(QPalette::Background, Qt::gray);
		m_showState = false;
		break;
	default:
		break;
	}
	//setPixmap(pixmap);
	setPalette(palette);

}
