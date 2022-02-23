#include "cdetectspeed.h"

//#include <QPainter>
#include <QLayout>
#include <qwt_thermo.h>
#include "common.h"
#include "ILDRSystem.h"
CDetectSpeed::CDetectSpeed(QWidget *parent)
	: QWidget(parent)
{
	d_thermo = new QwtThermo();
	d_thermo->setOrientation( Qt::Horizontal, QwtThermo::BottomScale );
	d_thermo->setRange( 0.0, 500 );
	d_thermo->setFillBrush( Qt::green );

	label = new QLabel( tr("Speed:0"), this );
	label->setAlignment( Qt::AlignCenter );
	QFont font;
	font.setPointSize(22);
	label->setFont(font);

	QVBoxLayout *layout = new QVBoxLayout( this );
	layout->setMargin( 0 );
	layout->addWidget( label );
	layout->addWidget( d_thermo );
}

CDetectSpeed::~CDetectSpeed()
{

}
//void CDetectSpeed::paintEvent(QPaintEvent *event)
//{
//
//	//QPainter painter(this);
//
//	//iWidth = geometry().width();
//	//iHeight = geometry().height();
//	//if (iSpeed>iSpeedAlert)
//	//{
//	//	iSpeed = iSpeedAlert;
//	//}
//	//if (iSpeed == 0)
//	//{
//	//	iSpeed = 10;
//	//}
//	//
//	////QLinearGradient linearGradient(0, 0, iWidth, 0); 
//	////linearGradient.setColorAt(0.0, colorMiddle); 
//	////linearGradient.setColorAt(1.0*iSpeedNormal/iSpeedAlert, colorMiddle); 
//	////linearGradient.setColorAt(1.0, colorEnd); 
//
//	//painter.setBrush(QBrush(colorMiddle)); 
//	//painter.drawRect(QRect(0, 1.0*iHeight/2-25, 1.0*iSpeed/iSpeedAlert*(iWidth-100), iHeight/2+20));
//	//
//	//painter.drawText(QPoint(1.0*iSpeed/iSpeedAlert*(iWidth-100)+10,iHeight/2+7.5), tr("Speed:")+QString::number(iSpeed));
//
//}
void CDetectSpeed::slots_setSpeed(int speed)
{
	if (true)
	{
		int length = d_thermo->geometry().width();
		QLinearGradient linearGradient(0, 0, length, 0); 
		linearGradient.setColorAt(0.0, Qt::yellow); 
		linearGradient.setColorAt(0.199, Qt::yellow); 
		linearGradient.setColorAt(0.2, Qt::green); 
		linearGradient.setColorAt(0.599, Qt::green); 
		linearGradient.setColorAt(0.6, Qt::red); 
		d_thermo->setFillBrush( linearGradient);
	}
	else if (true)
	{
		int length = d_thermo->geometry().width();
		QLinearGradient linearGradient(0, 0, length, 0); 
		linearGradient.setColorAt(0.0, Qt::yellow); 
		linearGradient.setColorAt(0.2, Qt::green); 
		linearGradient.setColorAt(0.6, Qt::red); 
		d_thermo->setFillBrush( linearGradient);
	}
	else if (true)
	{
		if (speed < 100)
		{
			d_thermo->setFillBrush( Qt::yellow );
		}
		else if (speed >= 100 && speed < 300)
		{
			d_thermo->setFillBrush( Qt::green);
		}
		else if (speed >= 300)
		{
			d_thermo->setFillBrush( Qt::red);
		}
	}

	QFont labelfont;
	
	if (speed < 300)
	{
		label->setStyleSheet("color:black;");
	}
	else if (speed >= 300)
	{
		label->setStyleSheet("color:red;");
	}

	d_thermo->setValue( speed );
	label->setText( tr("Speed:") + QString::number(speed));
}
 