#include "widgetwarning.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QLayout>

#include <windows.h>
#include "clogfile.h"
#include "ILDRSystem.h"
extern SysMainUI *pMainFrm;

HideThread::HideThread(QObject *parent)
	: QThread(parent)
{
	//	pMainFrm = (GlasswareDetectSystem *)parent;
	m_bStopThread = false;
}

HideThread::~HideThread()
{

}

void HideThread::run()
{
	if(!m_bStopThread)
	{
		QDesktopWidget* desktopWidget = QApplication::desktop();
		for(int i = 0;i < 250 ;i++)
		{
			if(!m_bStopThread)
			{
				emit signal_Move(desktopWidget->width()-400,desktopWidget->height()-250+i);
				Sleep(2);
			}
			else
			{
				return;
			}
		}
		emit signal_HideWidget();
	}
}

void HideThread::WaitThreadStop()
{
	if (isRunning())
	{
		//m_bExitSystem = true;	//关闭系统标志位
		if (!m_bStopThread)
		{
			m_bStopThread = true;
		}
		wait();
	}
}




WidgetWarning::WidgetWarning(QWidget *parent)
	: QWidget(parent)
{
	setStyleSheet(QString::fromUtf8("background-color: rgb(138,171,140);"));

	bIsShow = false;
	iWarningType = 1;
	pHideThread = new HideThread(this);
	connect(pHideThread,SIGNAL(signal_Move(int,int)),this,SLOT(slots_Move(int,int)));
	connect(pHideThread,SIGNAL(signal_HideWidget()),this,SLOT(slots_HideWidget()));

	btnClose = new PushButton(this);
	btnClose->setPicName(QString(":/sysButton/close"));
	connect(btnClose, SIGNAL(clicked()), this, SLOT(hide()));

	btnStopWarning = new QPushButton();
	QPixmap pixmapBtnStop(":/sys/Pause1");
	btnStopWarning->setPixmap(pixmapBtnStop);
	btnStopWarning->setFixedSize(pixmapBtnStop.size());
	btnStopWarning->setFlat(true);
	//btnStopWarning->setPicName(QString(":/sys/Pause1"));
	//btnStopWarning->setText(tr("Stop"));
	connect(btnStopWarning, SIGNAL(clicked()), this, SLOT(slots_StopAlert()));

	connect(btnStopWarning, SIGNAL(clicked()), this, SIGNAL(signals_PauseAlert()));
	

	labelWarningPic = new QLabel();
	QPixmap pixmapWarning(":/sys/Alert");
	labelWarningPic->setPixmap(pixmapWarning);
	labelWarningPic->setFixedSize(pixmapWarning.size());

	labelWarningInfo = new QLabel();
	QFont font("宋体",20,QFont::DemiBold,false);
	labelWarningInfo->setFont(font);
	labelWarningInfo->setText(tr("Alert Info!"));
	labelWarningInfo->setAlignment(Qt::AlignCenter);
    labelWarningInfo->setWordWrap(true);
    MaxNumber=new QTimer(this);
    connect(MaxNumber, SIGNAL(timeout()), this, SLOT(slots_ReTim()));  
	InfoShowTimer=new QTimer(this);
	connect(InfoShowTimer, SIGNAL(timeout()), this, SLOT(slots_InfoShow()));  

	Init();
}

WidgetWarning::~WidgetWarning()
{
	pHideThread->WaitThreadStop();
}
void WidgetWarning::Init()
{
	setWindowFlags( Qt::FramelessWindowHint | Qt::Dialog);//去掉标题栏
	QDesktopWidget* desktopWidget = QApplication::desktop();
	setMinimumSize(400,300);

	QHBoxLayout *layoutTitle = new QHBoxLayout();
// 	QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
// 	layoutTitle->addItem(horizontalSpacer);
	layoutTitle->addStretch();
	layoutTitle->addWidget(btnClose, 0, Qt::AlignTop);
	layoutTitle->setContentsMargins(0,0,0,0);

	QVBoxLayout *layoutInfo = new QVBoxLayout();
	layoutInfo->addWidget(labelWarningInfo,Qt::AlignCenter);
	layoutInfo->addWidget(btnStopWarning,Qt::AlignCenter);

	QHBoxLayout *layoutBody = new QHBoxLayout();
	layoutBody->addWidget(labelWarningPic,Qt::AlignCenter);
	layoutBody->addLayout(layoutInfo,Qt::AlignCenter);
	layoutBody->setContentsMargins(10,0,10,0);
	layoutBody->setStretch(0,1);
	layoutBody->setStretch(1,1);

	QVBoxLayout *layoutMain = new QVBoxLayout(this);
	layoutMain->addLayout(layoutTitle);
	layoutMain->addLayout(layoutBody);
	layoutMain->setStretch(1,1);
	layoutMain->setContentsMargins(0,0,0,0);
	InfoIndex = 0;
}
bool WidgetWarning::IsShowWarning()
{
	return bIsShow;
}
void WidgetWarning::slots_ShowWarning(int warningType, QString warningInfo)
{
	iWarningType = warningType;
	InfoList<<warningInfo;
	CLogFile::write(warningInfo, AlarmLog,0);
	if (bHiding)
	{
		pHideThread->m_bStopThread = true;
		bIsShow = false;
	}

 	if (bIsShow)
 	{
		labelWarningInfo->setText(warningInfo);
 		return;
 	}
	
	switch (warningType)
	{
	case 1:
		btnStopWarning->setVisible(true);
		break;
	case 2:
		btnStopWarning->setVisible(false);
		break;
	case 8:
		btnStopWarning->setVisible(true);
		break;
	default:
		break;

	}
	labelWarningInfo->setText(warningInfo);
	QDesktopWidget* desktopWidget = QApplication::desktop();

	move(desktopWidget->width()-400,desktopWidget->height()-300);
	show();
	bIsShow = true;

	InfoIndex = 0;
	InfoShowTimer->start(1000);

}
void WidgetWarning::slots_HideWarning(int iHideType)
{
	if (!bIsShow)
	{
		return;
	}
	if (iHideType != iWarningType)
	{
		return;
	}
	bHiding = true;
	
	QDesktopWidget* desktopWidget = QApplication::desktop();
 	pHideThread->m_bStopThread = false;
	pHideThread->start();
}
void WidgetWarning::slots_Move(int iPositionX,int iPositionY)
{
	move(iPositionX ,iPositionY);

}
void WidgetWarning::slots_HideWidget()
{
	hide();
	bIsShow = false;
	bHiding = false;
	InfoShowTimer->stop();
	InfoList.clear();

}
void WidgetWarning::slots_StopAlert()
{
	slots_HideWarning(iWarningType);
	MaxNumber->start(10000,TRUE);
}

void WidgetWarning::slots_InfoShow()
{
	if (InfoList.count() == 0)
	{
		return;
	}
	if (InfoList.count()-1 < InfoIndex)
	{
		InfoIndex =0;
	}
	labelWarningInfo->setText(InfoList[InfoIndex]);
	InfoIndex++;
}

void WidgetWarning::slots_ReTim()
{

}
