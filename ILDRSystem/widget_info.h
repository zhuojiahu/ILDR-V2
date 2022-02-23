#ifndef WIDGET_INFO_H
#define WIDGET_INFO_H

#include <QObject>
#include <QTableView>
#include <QGraphicsView>
#include <QStandardItemModel>
#include <QMutex>
#include <QPushButton>
#include <QLCDNumber>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#include "cdetectspeed.h"
#include "userwidget.h"
#include "camerasatuslabel.h"
#include "rotemptybottle.h"
#include "chistogram.h"
#include "widget_pie.h"

#include "mylogtext.h"
#include <time.h>

class widget_info : public QWidget
{
	Q_OBJECT

public:
	widget_info(QWidget *parent);
	~widget_info();
	bool intoWidget(int iIndex/* = -1*/);
	bool leaveWidget();
	void updateInfo();
private:
	void autoLogout();

public slots:
	void slots_intoWidget();
	void slots_updateCameraFailureRate();
	void slots_updateTime();
	void slots_SaveCurveInfo();
signals:
	void signals_updateInfo();
	void signals_HideWarning(int);
	void signals_SendPLCAlarm(int);

public:
	time_t tLastOperaTime;
	QMutex m_mutexmAddRow;						//操作互斥

	UserWidget *widgetUser;
	DHBaseWidget *widgetBaseSpeed;
	CDetectSpeed *widgetSpeed;
	WidgetPie *widgetPie;
	QLabel *labelCateName;

	QVector <double> dXData;//横坐标	
	QVector <double> dTotalNumber;
	QVector <double> dFailureNumber;
	QVector <double> dFailureRate;
	QVector <double> dFailureNumberByErrorType[ERRORTYPE_MAX_COUNT];
	QVector <double> dFailureNumberByCamera[CAMERA_MAX_COUNT];

	QTimer *timerCurve;

//	QGridLayout *hLayoutMini;
	QHBoxLayout *hLayoutMini;
private:
	DHBaseWidget *failureImageWidget;
	DHBaseWidget *totleQwtWidget;
	DHBaseWidget *failureQwtWidget;
	DHBaseWidget *plotWidget;
	DHBaseWidget *alertWidget;
	DHBaseWidget *logWidget;
	DHBaseWidget *histogramWidget;

	QwtPlot *plotTotleNumber;
	QwtPlot *plotFailureRate;
	QwtPlot *plotFailureNumber;
	CHistogram *plotCameraFailureRate;

	QwtPlotCurve *pCurveTotal;
	QwtPlotCurve *pCurveFailure;
	QwtPlotCurve *pCurveFailureRate;
	double dXAxisScale;
	double dYAxisScaleTotal;
	double dYAxisScaleFailure;
	double dYAxisScaleFailureRate;

	double highestFailureRate;


	CMyLogText *logText;
	QLabel *labelAlert1;
	QLabel *labelAlert2;
	//算法使用的检测列表
	s_InputCheckerAry CherkerAry;

	QTimer *timerUpdateTime;
	QLCDNumber *lcdNumberTime;

	bool bIsPauseAlert;

	bool bAlarmCameraContinueReject;
	bool bAlarmCameraOffLine;

private:
	void initail();

};

#endif // WIDGET_INFO_H
