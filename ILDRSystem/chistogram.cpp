#include <stdlib.h>
#include <qpen.h>
#include <qwt_plot_layout.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include <qwt_plot_grid.h>
// #include <qwt_plot_histogram.h>
#include <qwt_column_symbol.h>
#include <qwt_series_data.h>
#include <qwt_scale_draw.h>
//#include <qwt_plot_barchart.h>
#include "chistogram.h"

#include "ILDRSystem.h"
extern SysMainUI *pMainFrm;


CHistogram::CHistogram(QWidget *parent)
	: QwtPlot(parent)
{

	iCameraNum = pMainFrm->m_sSystemInfo.iCamCount;
	histogramCamera = new Histogram( tr("Camera Reject Rate"), Qt::red );

	setTitle(tr("Reject Rate of Camera"));

	setCanvasBackground( QColor( Qt::gray ) );
	plotLayout()->setAlignCanvasToScales( true );

	setAxisTitle( QwtPlot::yLeft, tr("Reject Rate"));
	setAxisTitle( QwtPlot::xBottom, tr("Camera") );

	setAxisScale( QwtPlot::xBottom, 0.5, pMainFrm->m_sSystemInfo.iCamCount+0.5, 1 );
	setAxisScale( QwtPlot::yLeft, 0, 10, 0 );

//	axisScaleDraw(QwtPlot::xBottom)->enableComponent(QwtScaleDraw::Backbone, false);
	axisScaleDraw(QwtPlot::xBottom)->enableComponent(QwtScaleDraw::Ticks, false);

	QwtPlotGrid *grid = new QwtPlotGrid;
	grid->enableX( false );
	grid->enableY( true );
	grid->enableXMin( false );
	grid->enableYMin( false );
	grid->setMajPen( QPen( Qt::black, 0, Qt::DotLine ) );
	grid->attach( this );

	//QwtLegend *legend = new QwtLegend;
	//legend->setItemMode( QwtLegend::CheckableItem );
	//insertLegend( legend, QwtPlot::RightLegend );

	populate();

	connect( this, SIGNAL( legendChecked( QwtPlotItem *, bool ) ),
		SLOT( showItem( QwtPlotItem *, bool ) ) );

	replot(); // creating the legend items

// 	QwtPlotItemList items = itemList( QwtPlotItem::Rtti_PlotHistogram );
// 	for ( int i = 0; i < items.size(); i++ )
// 	{
// 		if ( i == 0 )
// 		{
// // 			QwtLegendItem *legendItem =
// // 				qobject_cast<QwtLegendItem *>( legend->find( items[i] ) );
// // 			if ( legendItem )
// // 				legendItem->setChecked( true );
// 
// 			items[i]->setVisible( true );
// 		}
// 		else
// 			items[i]->setVisible( false );
// 	}
	
	setAutoReplot( true );
	setLineWidth(20);
}

CHistogram::~CHistogram()
{

}



void CHistogram::populate()
{
	//for ( int i = 0; i < listCameraValues.length(); i++ )
	//{
	//	QwtColumnSymbol *symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
	//	symbol->setLineWidth( 2 );
	//	symbol->setFrameStyle( QwtColumnSymbol::Raised );
	//	if (listCameraValues.at(i)>10)
	//	{
	//		QPalette palette;
	//		palette.setColor(QPalette::Background,QColor(225,25,25,120));
	//		symbol->setPalette( palette );
	//	}
	//	else
	//	{
	//		QPalette palette;
	//		palette.setColor(QPalette::Background,QColor(25,225,25,120));
	//		symbol->setPalette( palette );
	//	}
	//	histogramCamera->setSymbol( symbol );
	//}

// 	dCameraValues = pMainFrm->m_sRunningInfo.m_iErrorCamRate;

 	histogramCamera->setValues(iCameraNum, pMainFrm->m_sRunningInfo.m_iErrorCamRate );
	double dYAxisMax = 0;
	for (int i = 0; i<listCameraValues.length(); i++)
	{
		if (dYAxisMax < listCameraValues.at(i))
		{
			dYAxisMax = listCameraValues.at(i);
		}		
	}
	setAxisScale( QwtPlot::yLeft, 0, (dYAxisMax + 1)*1.1, 0 );

// 	detach();
	histogramCamera->attach( this );

}

void CHistogram::showItem( QwtPlotItem *item, bool on )
{
	item->setVisible( on );
}
