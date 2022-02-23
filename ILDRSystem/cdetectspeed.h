#ifndef CDETECTSPEED_H
#define CDETECTSPEED_H

#include <QObject>
#include <QColor>
#include <QLabel>

#include <qwt_thermo.h>
// #include "dhbasewidget.h"

class CDetectSpeed : public QWidget
{
	Q_OBJECT

public:
	CDetectSpeed(QWidget *parent);
	~CDetectSpeed();
public slots:
	void slots_setSpeed(int speed);
//protected:
// 	void paintEvent(QPaintEvent *);

//private:
//	int iSpeedAlert;
//	int iSpeedNormal;
//	int iSpeed;
//	int iWidth;
//	int iHeight;
//
//	QColor colorBegin;
//	QColor colorMiddle;
//	QColor colorEnd;
//	QLabel showSpeed;

private:
	QwtThermo *d_thermo;
	QLabel *label;
};

#endif // CDETECTSPEED_H
