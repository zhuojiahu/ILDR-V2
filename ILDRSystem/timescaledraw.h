#ifndef TIMESCALEDRAW_H
#define TIMESCALEDRAW_H

#include <qwt_scale_draw.h>
#include <QDateTime>

class TimeScaleDraw : public QwtScaleDraw
{
	double minLabelPos, maxLabelPos;
public:
	TimeScaleDraw(const double minLabelPos = 0, const double maxLabelPos = 10000);
	virtual QwtText label(double) const;
	void myinit(const double minLabelPos, const double maxLabelPos);
};

#endif // TIMESCALEDRAW_H