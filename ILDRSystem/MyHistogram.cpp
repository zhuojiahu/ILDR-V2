#include "myhistogram.h"
#include <QPalette>


Histogram::Histogram( const QString &title, const QColor &symbolColor ):
	QwtPlotHistogram( title )
{
	setStyle( QwtPlotHistogram::Columns );
	setColor( symbolColor );
}

void Histogram::setColor( const QColor &symbolColor )
{
	QColor color = symbolColor;
	color.setAlpha( 180 );

	setPen( QPen( Qt::black ) );
	setBrush( QBrush( color ) );

	QwtColumnSymbol *symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
	symbol->setFrameStyle( QwtColumnSymbol::Raised );
	symbol->setLineWidth( 2 );
	symbol->setPalette( QPalette( color ) );
	setSymbol( symbol );
}

void Histogram::setValues( uint numValues,  const double *dValues )
{
	QVector<QwtIntervalSample> samples( numValues );
	for ( uint i = 0; i < numValues; i++ )
	{
		QwtInterval interval( double( i+0.5 ), i + 1.5 );
		interval.setBorderFlags( QwtInterval::ExcludeMaximum );

		samples[i] = QwtIntervalSample( dValues[i], interval );
	}
	setData( new QwtIntervalSeriesData( samples ) );
}