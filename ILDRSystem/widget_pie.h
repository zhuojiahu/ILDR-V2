#ifndef WIDGETPIE_H
#define WIDGETPIE_H

#include "DHBaseWidget.h"
#include "pieview.h"
#include "common.h"
#include <QStandardItemModel>

class WidgetPie : public QWidget
{
	Q_OBJECT

public:
	WidgetPie(QWidget *parent);
	~WidgetPie();
//	void updatePie(int iFailur, int iPass);
	void updatePie(int Error[ERRORTYPE_MAX_COUNT],int pAllCount,int pFailCount);
protected:
	virtual void paintEvent(QPaintEvent *event);

private:
	void setupModel();
	void setupViews();
	void resizeEvent(QResizeEvent *event);

private:
	QStandardItemModel *model;
	PieView *pieChart;
	QItemSelectionModel *selectionModel;
	QMutex UpdateMutex;
};

#endif // WIDGETPIE_H
