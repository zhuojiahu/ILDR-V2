#include "timescaledraw.h"

TimeScaleDraw::TimeScaleDraw(const double minLabelPos, const double maxLabelPos)
{
	myinit(minLabelPos, maxLabelPos);
}

QwtText TimeScaleDraw::label(double v) const
{
	QDateTime datetime;

	//if(v < minLabelPos || v > maxLabelPos)
//		return QwtText();

	datetime.setTime_t((uint)v);
// 	return QwtText(datetime.time().toString(Qt::ISODate));//+"\n"+datetime.date().toString(Qt::ISODate));
	return QwtText(datetime.time().toString("hh:mm"));//+"\n"+datetime.date().toString(Qt::ISODate));
}

void TimeScaleDraw::myinit(const double minLabelPos, const double maxLabelPos)
{
	this->minLabelPos = minLabelPos;
	this->maxLabelPos = maxLabelPos;
}