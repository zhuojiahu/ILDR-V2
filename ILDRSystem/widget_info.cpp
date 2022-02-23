#include "widget_info.h"
#include <QTime>
#include <qwt_plot_layout.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_legend.h>
#include <qwt_abstract_scale.h>
#include <qwt_scale_widget.h>
#include <qwt_text_label.h>

#include "timescaledraw.h"
#include "ILDRSystem.h"
extern SysMainUI *pMainFrm;

widget_info::widget_info(QWidget *parent)
	: QWidget(parent)
{

	dXAxisScale = 0;
	dYAxisScaleTotal = 0;
	dYAxisScaleFailure = 0;
	dYAxisScaleFailureRate = 0;

	highestFailureRate = 0;
	bIsPauseAlert = false;

	widgetBaseSpeed = new DHBaseWidget(this);
	widgetBaseSpeed->setWidgetName(tr("Detect Speed"));
	widgetBaseSpeed->setMinimumHeight(100);
	widgetSpeed = new CDetectSpeed(this);

	QVBoxLayout *vLayoutSpeedWidget = new QVBoxLayout(widgetBaseSpeed);
	vLayoutSpeedWidget->addWidget(widgetBaseSpeed->widgetName);
	vLayoutSpeedWidget->addWidget(widgetSpeed);
	vLayoutSpeedWidget->setSpacing(0);
	vLayoutSpeedWidget->setContentsMargins(5,0,5,5);

	widgetUser = new UserWidget(this);

	QFont font1,font2;
	font1.setPixelSize(16);
	font1.setBold(true);
	labelAlert1 = new QLabel(this);
	labelAlert1->setFont(font1);
	labelAlert1->setText(tr("Reject Rate in Last 10Minite:"));
	labelAlert2 = new QLabel(this);
	font2.setPixelSize(48);
	labelAlert2->setStyleSheet("color:black;");
	labelAlert2->setFont(font2);
	labelAlert2->setText(QString("0%"));
	alertWidget = new DHBaseWidget(this);
	alertWidget->setWidgetName(tr("Alert Widget"));
	alertWidget->setMinimumHeight(100);
	alertWidget->setVisible(false);

	QVBoxLayout *vLayoutAlertWidget = new QVBoxLayout(alertWidget);
	vLayoutAlertWidget->addWidget(alertWidget->widgetName);
	vLayoutAlertWidget->addWidget(labelAlert1,0,Qt::AlignLeft);
	vLayoutAlertWidget->addWidget(labelAlert2,0,Qt::AlignCenter);
	vLayoutAlertWidget->setSpacing(0);
	vLayoutAlertWidget->setContentsMargins(5,0,5,5);
	
	lcdNumberTime = new QLCDNumber();
	lcdNumberTime->setNumDigits(19);  
	lcdNumberTime->setMinimumSize(QSize(0, 50));
	lcdNumberTime->setFrameShape(QFrame::NoFrame);
	lcdNumberTime->setSegmentStyle(QLCDNumber::Flat);  
	lcdNumberTime->display(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

	labelCateName = new QLabel();

	font2.setPixelSize(32);
	labelCateName->setFont(font2);
	labelCateName->setText(pMainFrm->m_sSystemInfo.LastModelName);
	labelCateName->setAlignment(Qt::AlignCenter);

	logText = new CMyLogText(this);
	logText->setMaximumWidth(300);
	logWidget = new DHBaseWidget(this);
	logWidget->setWidgetName(tr("Information Widget"));
	logWidget->setMaximumWidth(300);

	QVBoxLayout *vLayoutlogWidget = new QVBoxLayout(logWidget);
	vLayoutlogWidget->addWidget(logWidget->widgetName);
	vLayoutlogWidget->addWidget(labelCateName);
	vLayoutlogWidget->addWidget(lcdNumberTime);
	vLayoutlogWidget->addWidget(logText);
	vLayoutlogWidget->setSpacing(5);
	vLayoutlogWidget->setContentsMargins(5,0,5,5);

	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	plotWidget = new DHBaseWidget(this);
	plotWidget->setSizePolicy(sizePolicy);
	plotTotleNumber = new QwtPlot(plotWidget);
	plotFailureNumber = new QwtPlot(plotWidget);
	plotFailureRate = new QwtPlot(plotWidget);
	plotWidget->setWidgetName(tr("Plot"));
	plotWidget->setMaximumHeight(300);

	histogramWidget = new DHBaseWidget(this);
	histogramWidget->setSizePolicy(sizePolicy);
	histogramWidget->setWidgetName(tr("histogram"));
	plotCameraFailureRate = new CHistogram(this);
	QVBoxLayout *vLayouthistogram = new QVBoxLayout(histogramWidget);
	vLayouthistogram->addWidget(histogramWidget->widgetName);
	vLayouthistogram->addWidget(plotCameraFailureRate);
	vLayouthistogram->setSpacing(5);
	vLayouthistogram->setContentsMargins(5,0,5,5);

	widgetPie = new WidgetPie(this);

	QSizePolicy sizePolicyMini(QSizePolicy::Preferred, QSizePolicy::Preferred);

	widgetUser->setSizePolicy(sizePolicyMini);
	widgetBaseSpeed->setSizePolicy(sizePolicyMini);
	alertWidget->setSizePolicy(sizePolicyMini);
	widgetPie->setSizePolicy(sizePolicyMini);

	QHBoxLayout *hLayoutQwt = new QHBoxLayout();
	hLayoutQwt->addWidget(plotFailureRate);
	hLayoutQwt->addWidget(plotTotleNumber);
	hLayoutQwt->addWidget(plotFailureNumber);
	hLayoutQwt->setSpacing(6);
	hLayoutQwt->setContentsMargins(5, 0, 5, 5);

	QVBoxLayout *qwtLayout = new QVBoxLayout(plotWidget);
	qwtLayout->addWidget(plotWidget->widgetName);
	qwtLayout->addLayout(hLayoutQwt);
	qwtLayout->setSpacing(6);
	qwtLayout->setContentsMargins(5, 0, 5, 5);

	hLayoutMini = new QHBoxLayout();
	hLayoutMini->addWidget(widgetUser);
	hLayoutMini->addWidget(widgetBaseSpeed);
	hLayoutMini->addWidget(alertWidget);
	hLayoutMini->addWidget(widgetPie);
	hLayoutMini->setSpacing(6);
	hLayoutMini->setContentsMargins(0,0,0,0);
	hLayoutMini->setStretch(0, 1);
	hLayoutMini->setStretch(1, 1);
	hLayoutMini->setStretch(2, 1);
	hLayoutMini->setStretch(3, 1);

	QVBoxLayout *vLayoutleft= new QVBoxLayout();
 	vLayoutleft->addLayout(hLayoutMini);
	vLayoutleft->addWidget(plotWidget);
 	vLayoutleft->addWidget(histogramWidget);
	vLayoutleft->setSpacing(6);
 	vLayoutleft->setContentsMargins(0,0,0,0);

	QHBoxLayout *hLayoutUp = new QHBoxLayout();
	hLayoutUp->addLayout(vLayoutleft);
	hLayoutUp->addWidget(logWidget);
	hLayoutUp->setSpacing(5);
	hLayoutUp->setContentsMargins(5, 0, 5, 0);

	QVBoxLayout *wholeLayout = new QVBoxLayout();
	wholeLayout->addLayout(hLayoutUp);
	wholeLayout->setSpacing(6);
	wholeLayout->setContentsMargins(0, 9, 9, 9);


	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addLayout(wholeLayout);
	mainLayout->setSpacing(5);
	mainLayout->setContentsMargins(5, 0, 5, 0);

	setLayout(mainLayout);
	for (int i = 0; i < pMainFrm->m_sSystemInfo.iCamCount; i++)
	{
		connect(pMainFrm->pdetthread[i], SIGNAL(signals_showspeed(int)), widgetSpeed, SLOT(slots_setSpeed(int)));
	}
	connect(pMainFrm, SIGNAL(signals_writeLogText(QString,e_SaveLogType)), logText, SLOT(slots_writeLogText(QString,e_SaveLogType)));

 	for (int i = 0;i<pMainFrm->m_sSystemInfo.iCamCount;i++)
	{
		connect(pMainFrm->pdetthread[i], SIGNAL(signals_updateCameraFailureRate()), this, SLOT(slots_updateCameraFailureRate()));
	}   
	initail();
}

widget_info::~widget_info()
{
}

void widget_info::initail()
{
	//qwt外观
	QwtTextLabel *title;
	title = plotTotleNumber->titleLabel();
	QFont font;
	font.setPointSize(24);
	title->setFont(font);	
	title = plotFailureRate->titleLabel();	
	title->setFont(font);	
	title = plotFailureNumber->titleLabel();	
	title->setFont(font);	


	plotTotleNumber->setTitle(tr("Product Number"));
	plotFailureRate->setTitle(tr("Reject Rate"));
	plotFailureNumber->setTitle(tr("Reject Number"));
	//网格
	QwtPlotGrid *grid = new QwtPlotGrid;  
	grid->setMajPen(QPen(Qt::darkGray, 0, Qt::DotLine));  
	grid->attach(plotTotleNumber); 
	QwtPlotGrid *grid2 = new QwtPlotGrid;  
	grid2->setMajPen(QPen(Qt::darkGray, 0, Qt::DotLine));  
	grid2->attach(plotFailureRate); 
	QwtPlotGrid *grid3 = new QwtPlotGrid;  
	grid3->setMajPen(QPen(Qt::darkGray, 0, Qt::DotLine));  
	grid3->attach(plotFailureNumber); 
	//背景
	QPalette pal = palette();
	QLinearGradient gradient( 0, 0, 0, 1 );
	gradient.setCoordinateMode( QGradient::StretchToDeviceMode );
	gradient.setColorAt( 0.0, QColor(220,220,220) );
	gradient.setColorAt( 0.5, QColor(160,160,160));
	gradient.setColorAt( 1.0, QColor(220,220,220));
	pal.setBrush( QPalette::Window, gradient );
	plotTotleNumber->canvas()->setPalette( pal );
	plotFailureRate->canvas()->setPalette(pal);
	plotFailureNumber->canvas()->setPalette(pal);
	//缩放
	QwtPlotMagnifier *pMagTotle = new QwtPlotMagnifier(plotTotleNumber->canvas());
	QwtPlotPanner *pPannerTotle= new QwtPlotPanner(plotTotleNumber->canvas());
	pPannerTotle->setMouseButton(Qt::MidButton);
	QwtPlotMagnifier *pMagFailure = new QwtPlotMagnifier(plotFailureRate->canvas());
	QwtPlotPanner *pPannerFailure= new QwtPlotPanner(plotFailureRate->canvas());
	pPannerFailure->setMouseButton(Qt::MidButton);
	QwtPlotMagnifier *pMagFailureNumber = new QwtPlotMagnifier(plotFailureNumber->canvas());
	QwtPlotPanner *pPannerFailureNumber = new QwtPlotPanner(plotFailureNumber->canvas());
	pPannerFailureNumber->setMouseButton(Qt::MidButton);

	//总数曲线
	pCurveTotal = new QwtPlotCurve(tr("Product number"));
	pCurveTotal->setPen(QPen(QColor(233,21,50)));
	pCurveTotal->setRenderHint(QwtPlotItem::RenderAntialiased,true);
	pCurveTotal->attach(plotTotleNumber);
	//不合格数曲线
	pCurveFailure = new QwtPlotCurve(tr("Reject number"));
	pCurveFailure->setPen(QPen(QColor(233,21,50)));
	pCurveFailure->setRenderHint(QwtPlotItem::RenderAntialiased,true);
	pCurveFailure->attach(plotFailureNumber);

	pCurveFailureRate = new QwtPlotCurve(tr("Reject rate"));
	pCurveFailureRate->setPen(QPen(QColor(233,21,50)));
	pCurveFailureRate->setRenderHint(QwtPlotItem::RenderAntialiased,true);
	pCurveFailureRate->attach(plotFailureRate);

	//设置横纵坐标
	QDateTime time = QDateTime::currentDateTime();
	QDateTime time2 = QDateTime::fromString("M1d1y7008:00:00","'M'M'd'd'y'yyhh:mm:ss");
	double x = time2.secsTo(time);
	dXAxisScale = x+3600;
	plotTotleNumber->setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw());
	plotTotleNumber->setAxisScale(QwtPlot::xBottom,x,x+3600);
	plotTotleNumber->setAxisScale(QwtPlot::yLeft,0,1000);
	plotFailureNumber->setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw());
	plotFailureNumber->setAxisScale(QwtPlot::xBottom,x,x+3600);
	plotFailureNumber->setAxisScale(QwtPlot::yLeft,0,1000);
	plotFailureRate->setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw());
	plotFailureRate->setAxisScale(QwtPlot::xBottom,x,x+3600);
	plotFailureRate->setAxisScale(QwtPlot::yLeft,0,2,1);
	//添加图例
	plotTotleNumber->insertLegend(new QwtLegend(),QwtPlot::BottomLegend); 
	plotTotleNumber->replot();
	plotFailureRate->insertLegend(new QwtLegend(),QwtPlot::BottomLegend); 
	plotFailureRate->replot();
	plotFailureNumber->insertLegend(new QwtLegend(),QwtPlot::BottomLegend); 
	plotFailureNumber->replot();

	plotCameraFailureRate->listCameraValues.clear();
	for (int i = 0;i<pMainFrm->m_sSystemInfo.iCamCount;i++)
	{
		plotCameraFailureRate->listCameraValues.append(pMainFrm->m_sRunningInfo.m_iErrorCamCount[i]*100);
	}
	plotCameraFailureRate->populate();

	timerCurve = new QTimer(this);   
	timerCurve->setInterval(6000);//6秒统计一次
	connect(timerCurve, SIGNAL(timeout()), this, SLOT(slots_SaveCurveInfo()));   
	timerCurve->start();  

	timerUpdateTime = new QTimer(this);
	timerUpdateTime->setInterval(1000);
	connect(timerUpdateTime, SIGNAL(timeout()), this, SLOT(slots_updateTime()),Qt::AutoConnection);   
	timerUpdateTime->start();
}

void widget_info::slots_intoWidget()
{
	updateInfo();
}

bool widget_info::leaveWidget()
{
  	return true;
}
void widget_info::slots_SaveCurveInfo()
{
	QSettings iniset(pMainFrm->m_sConfigInfo.m_strDataPath,QSettings::IniFormat);
	iniset.setIniCodec(QTextCodec::codecForName("GBK"));
	iniset.setValue("/system/failureNum",pMainFrm->m_sRunningInfo.m_failureNum);
	iniset.setValue("/system/checkedNum",pMainFrm->m_sRunningInfo.m_checkedNum);

	QDateTime time = QDateTime::currentDateTime();
	QDateTime time2 = QDateTime::fromString("M1d1y7008:00:00","'M'M'd'd'y'yyhh:mm:ss");
	double x = time2.secsTo(time);
	dXData.push_back(x);
	double total = pMainFrm->m_sRunningInfo.m_checkedNum;
	dTotalNumber.push_back(total);
	double failur = pMainFrm->m_sRunningInfo.m_failureNum;
	dFailureNumber.push_back(failur);
	dFailureNumberByErrorType[0].push_back(failur);
	for (int i=1; i<=pMainFrm->m_sErrorInfo.m_iErrorTypeCount;i++)
	{
		double failurByErrorType = pMainFrm->m_sRunningInfo.m_iErrorTypeCount[i];
		dFailureNumberByErrorType[i].push_back(failurByErrorType);
	}
	for (int i=0; i<pMainFrm->m_sSystemInfo.iIsCameraCount;i++)
	{
		double failurByCamera = pMainFrm->m_sRunningInfo.m_iErrorCamCount[i];
		dFailureNumberByCamera[i].push_back(failurByCamera);
	}
	double failurRate;
	if (0 == total)
	{
		failurRate = 0;
	}
	else
	{
		failurRate = (failur/total)*100;
	}
	dFailureRate.push_back(failurRate);
	if (dXData.size() > 7200)
	{
		dXData.pop_front();
	}
	if (dTotalNumber.size() > 7200)
	{
		dTotalNumber.pop_front();
	}
	if (dFailureNumber.size() > 7200)
	{
		dFailureNumber.pop_front();
	}
	for (int i=0; i<=pMainFrm->m_sErrorInfo.m_iErrorTypeCount;i++)
	{
		if (dFailureNumberByErrorType[i].size() > 7200)
		{
			dFailureNumberByErrorType[i].pop_front();
		}
	}
	for (int i=0; i<pMainFrm->m_sSystemInfo.iIsCameraCount;i++)
	{
		if (dFailureNumberByCamera[i].size() > 7200)
		{
			dFailureNumberByCamera[i].pop_front();
		}	
	}

	if (dFailureRate.size() > 7200)
	{
		dFailureRate.pop_front();
	}

	QString str = time.toString();
	QString str2 = time2.toString();

	double lowTotal = failur-3000;
	if (lowTotal<0)
	{
		lowTotal = 0;
	}
	if (highestFailureRate<failurRate)
	{
		highestFailureRate = failurRate;
	}	

	if (x >= dXAxisScale - 600)
	{
		plotTotleNumber->setAxisScale(QwtPlot::xBottom,x-3000,x+600,800);
		plotFailureRate->setAxisScale(QwtPlot::xBottom,x-3000,x+600,800);
		plotFailureNumber->setAxisScale(QwtPlot::xBottom,x-3000,x+600,800);
		dXAxisScale = x+600;
	}
	if (total >= dYAxisScaleTotal)
	{
		plotTotleNumber->setAxisScale(QwtPlot::yLeft,lowTotal,total+1000);
		dYAxisScaleTotal = total+1000;
	}
	if (failurRate >= dYAxisScaleFailureRate)
	{
		plotFailureRate->setAxisScale(QwtPlot::yLeft,-0.2,failurRate+2);
		dYAxisScaleFailureRate = failurRate+2;
	}
	if (failur >= dYAxisScaleFailure)
	{
		plotFailureNumber->setAxisScale(QwtPlot::yLeft,lowTotal,failur+1000);
		dYAxisScaleFailure = failur+1000;
	}
	
	pCurveTotal->setSamples(dXData,dTotalNumber);
	pCurveFailure->setSamples(dXData,dFailureNumber);
	pCurveFailureRate->setSamples(dXData,dFailureRate);

	plotTotleNumber->replot();
	plotFailureRate->replot();
	plotFailureNumber->replot();
}
void widget_info::updateInfo()
{
	autoLogout();
	double total = pMainFrm->m_sRunningInfo.m_checkedNum;
	double failur = pMainFrm->m_sRunningInfo.m_failureNum;
	double failurRate = 0;
	if (0 == total)
	{
		failurRate = 0;
	}
	else
	{
		failurRate = (failur/total)*100;

	}

	plotTotleNumber->setTitle(tr("Product Number:")+QString::number(total));
	plotFailureRate->setTitle(tr("Reject Rate:")+QString::number(failurRate,'f',2)+"%");
	plotFailureNumber->setTitle(tr("Reject Number:")+QString::number(failur));

	int iAlertTotalNumber = 0;
	int iAlertFailureNumber[ERRORTYPE_MAX_COUNT] = {0};
	double dAlertFailureRate[ERRORTYPE_MAX_COUNT] = {0};

	//alertWidget->setVisible(false);
	emit signals_updateInfo();
}
void widget_info::autoLogout()
{
	time_t tTemp;
	time(&tTemp);

	if(pMainFrm->m_sRunningInfo.m_iPermission>0 && pMainFrm->iLastPage != ENaviPageAlgSet && (tTemp - tLastOperaTime > pMainFrm->m_sSystemInfo.nLoginHoldTime*60))
	{
		widgetUser->logout();
		pMainFrm->slots_turnPage(HomePage);
	}
}
void widget_info::slots_updateCameraFailureRate()
{
	plotCameraFailureRate->listCameraValues.clear();
	for (int i = 0;i<pMainFrm->m_sSystemInfo.iCamCount;i++)
	{
		if (pMainFrm->m_sRunningInfo.m_checkedNum != 0)
		{
			qreal val = 1.0*pMainFrm->m_sRunningInfo.m_iErrorCamCount[i]/pMainFrm->m_sRunningInfo.m_checkedNum * 100 ;
			plotCameraFailureRate->listCameraValues.append(val);
		}
		else
		{
			plotCameraFailureRate->listCameraValues.append(0);
		}
	}
	plotCameraFailureRate->populate();
	widgetPie->updatePie( pMainFrm->m_sRunningInfo.m_iErrorTypeCount,pMainFrm->m_sRunningInfo.m_checkedNum , pMainFrm->m_sRunningInfo.m_failureNum);
}
void widget_info::slots_updateTime()
{
	QString str = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	lcdNumberTime->display(str);  

	updateInfo();
}
