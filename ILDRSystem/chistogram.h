#ifndef CHISTOGRAM_H
#define CHISTOGRAM_H

#include <QWidget>
#include <qwt_plot.h>
#include "common.h"
#include "myhistogram.h"

class CHistogram : public QwtPlot
{
	Q_OBJECT

public:
	CHistogram(QWidget *parent);
	~CHistogram();
	void populate();
public:
	QList<double> listCameraValues;
	double dCameraValues[CAMERA_MAX_COUNT];
	Histogram *histogramCamera;
private:
//	void setColor( const QColor & );
//	void setValues( uint numValues, const double * );
private Q_SLOTS:
	void showItem( QwtPlotItem *, bool on );
private:
	int iCameraNum;

};

#endif // CHISTOGRAM_H
