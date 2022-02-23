#ifndef MYHISTOGRAM_H
#define MYHISTOGRAM_H

#include <qwt_plot_histogram.h>

class Histogram: public QwtPlotHistogram
{
public:
	Histogram( const QString &, const QColor & );

	void setColor( const QColor & );
	void setValues( uint numValues, const double *);
};

#endif // MyHISTOGRAM_H
