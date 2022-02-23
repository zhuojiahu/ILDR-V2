#include "widget_plc1.h"

#include <QLayout>
#include <QGroupBox>
#include <QSettings>
#include <QTextCodec>
#include <QMouseEvent>
#include <QPainter>

#include "ILDRSystem.h"
extern SysMainUI *pMainFrm;

SensorStatusLabel::SensorStatusLabel(QWidget *parent)
	:CameraStatusLabel(parent)
{
	iMaxWidth = 0;
	iMaxHeight = 0;
	
	pointPosition = new QPoint(0,0);
	mousePosition = new QPoint(0,0);

	menu_SensorStatus = new QMenu(this);
	action_hide = new QAction(this);
	action_hide->setText(QString(tr("Hide")));

	connect(action_hide,SIGNAL(triggered()),this,SLOT(slots_action_hide()));
}
SensorStatusLabel::~SensorStatusLabel()
{
	delete pointPosition;
	delete mousePosition;
}
void SensorStatusLabel::contextMenuEvent(QContextMenuEvent *event)
{
	menu_SensorStatus->clear(); //清除原有菜单
	QPoint point = event->pos(); //得到窗口坐标
	menu_SensorStatus->addAction(action_hide);

	//菜单出现的位置为当前鼠标的位置
	menu_SensorStatus->exec(QCursor::pos());
	event->accept();

}
void SensorStatusLabel::slots_action_hide()
{
	setVisible(false);
}

void SensorStatusLabel::setMaxWidth(int width)
{
	iMaxWidth = width;
}
void SensorStatusLabel::setMaxHeight(int height)
{
	iMaxHeight = height;
}

void SensorStatusLabel::mousePressEvent(QMouseEvent *event)
{
	pointPosition->setX(pos().x());
	pointPosition->setY(pos().y());
	mousePosition->setX(cursor().pos().x());
	mousePosition->setY(cursor().pos().y());
}
void SensorStatusLabel::mouseReleaseEvent(QMouseEvent *event)
{
// 	emit signals_mouseReleaseEvent(QMouseEvent *event);
}
void SensorStatusLabel::mouseMoveEvent(QMouseEvent *event)
{
	int iPositionX = cursor().pos().x()-mousePosition->x()+pointPosition->x();
	int iPositionY = cursor().pos().y()-mousePosition->y()+pointPosition->y();

	if (iPositionX<0)
	{
		iPositionX = 0;
	}
	if (iPositionY<0)
	{
		iPositionY = 0;
	}
	if (iPositionX + rect().width()>iMaxWidth)
	{
		iPositionX = iMaxWidth-rect().width();
	}
	if (iPositionY + rect().height()>iMaxHeight)
	{
		iPositionY = iMaxHeight-rect().height();
	}
	move(iPositionX,iPositionY);
}

Widget_PLCImage::Widget_PLCImage(QWidget *parent)
	: QWidget(parent)
{
    if(!pMainFrm->buttonVisible[PLCPage])
        return;
	menu_PLCImage = new QMenu(this);
	action_ResetSensorPosition = new QAction(this);
	action_ResetSensorPosition->setText(QString(tr("ResetSensorPosition")));
	action_SaveSensorPosition = new QAction(this);
	action_SaveSensorPosition->setText(QString(tr("SaveSensorPosition")));
	action_SetPLCPara = new QAction(this);
	action_SetPLCPara->setText(QString(tr("SetPLCPara")));
}
Widget_PLCImage::~Widget_PLCImage()
{

}
void Widget_PLCImage::contextMenuEvent(QContextMenuEvent *event)
{
	menu_PLCImage->clear(); //清除原有菜单
	QPoint point = event->pos(); //得到窗口坐标
	menu_PLCImage->addAction(action_ResetSensorPosition);
	menu_PLCImage->addAction(action_SaveSensorPosition);
	//if (pMainFrm->sPermission.iPLCSet)
	{
		menu_PLCImage->addAction(action_SetPLCPara);
	}

	//菜单出现的位置为当前鼠标的位置
	menu_PLCImage->exec(QCursor::pos());
	event->accept();

}
void Widget_PLCImage::paintEvent(QPaintEvent *event)
{
	QPixmap pixmap("./layout.bmp");

	QPainter painter(this);
	//painter.setPen(Qt::NoPen);
	//painter.setBrush(Qt::red);
	painter.drawPixmap(QRect(0, 0, geometry().width(), geometry().height()),pixmap);
}

ReadPLCThread::ReadPLCThread(QObject *parent)
	: QThread(parent)
{
	if(pMainFrm->sLanguage == 0)
		m_strPLCInfoPath = "Config\\PLC_Info.ini";
	else
		m_strPLCInfoPath = "Config\\PLC_Info_en.ini";
	char charPLCInfoPath[maxpath];
	memset(charPLCInfoPath,0,maxpath);
	strcpy_s(charPLCInfoPath,m_strPLCInfoPath); 
	PLCContorl = new CHMIDLL();
	PLCContorl->InitPath(charPLCInfoPath);
	if (!pMainFrm->m_sSystemInfo.m_iTest)
	{
		PLCContorl->InitSerialComm();
	}
	m_bStopThread = false;
}
ReadPLCThread::~ReadPLCThread()
{
	PLCContorl->CloseComm();
	delete PLCContorl;

}
void ReadPLCThread::slots_SetPLCPara()
{
	mutexPLC.lock();
	PLCContorl->CallParamDlg();	
	mutexPLC.unlock();

}
void ReadPLCThread::slots_SendPLCAlarm(int iAlarm)
{

	if (!PLCContorl->m_bIsCommBusy)
	{
		mutexPLC.lock();	
		PLCContorl->SetAlarm(iAlarm);
		mutexPLC.unlock();	
	}

}
void ReadPLCThread::run()
{
	m_bStopThread = false;
	while(!m_bStopThread)
	{
		if (!PLCContorl->m_bIsCommBusy)
		{
			s_PLCStatus sPLCStatus;
			mutexPLC.lock();
			PLCContorl->GetPLCStatus(sPLCStatus);
			mutexPLC.unlock();
			emit signalsUpdatePLCStatus(sPLCStatus);
		}
		Sleep(500);
	}
}
void ReadPLCThread::StartThread()
{
	m_bStopThread = false;
	start();
}
void ReadPLCThread::StopThread()
{
	m_bStopThread = true;
}
void ReadPLCThread::WaitThreadStop()
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

Widget_PLC1::Widget_PLC1(QWidget *parent)
	: QWidget(parent)
{
    pThreadReadPLCStatus = new ReadPLCThread(this);
    if(!pMainFrm->buttonVisible[PLCPage])
        return;
	qRegisterMetaType<s_PLCStatus>("s_PLCStatus");
	if (pMainFrm->sLanguage == 0)
		m_strPLCInfoPath = "Config\\PLC_Info.ini";//读取传感器名
	else
		m_strPLCInfoPath = "Config\\PLC_Info_en.ini";
	m_strSensorPositionPath = "./Config/PLCStatusType.ini";
	pThreadReadPLCStatus->StartThread();

	widgetPLCImage = new Widget_PLCImage(this);
	widgetPLCInfo = new QWidget(this);
	labelCoderCounter = new QLabel(this);

	connect(pThreadReadPLCStatus, SIGNAL(signalsUpdatePLCStatus(s_PLCStatus)), this, SLOT(slots_updatePLCInfo(s_PLCStatus)));   
	connect(widgetPLCImage->action_ResetSensorPosition,SIGNAL(triggered()),this,SLOT(slots_ResetSensorPosition()));
	connect(widgetPLCImage->action_SaveSensorPosition,SIGNAL(triggered()),this,SLOT(slots_SaveSensorPosition()));
	connect(widgetPLCImage->action_SetPLCPara,SIGNAL(triggered()),this,SLOT(slots_SetPLCPara()));
	//connect(pMainFrm->info_widget, SIGNAL(signals_SendPLCAlarm(int)), pThreadReadPLCStatus, SLOT(slots_SendPLCAlarm(int)));

	labelCoderCounter->move(50,10);

	QSettings SensorPoiniset(m_strSensorPositionPath,QSettings::IniFormat);
	SensorPoiniset.setIniCodec(QTextCodec::codecForName("GBK"));
	QString strSession;
	for (int i=0;i<10;i++)
	{
		strSession = QString("/SensorPositionX/%1").arg(i);
		SensorPositionX[i] = SensorPoiniset.value(strSession,(i+1)*40).toInt();
		strSession = QString("/SensorPositionY/%1").arg(i);
		SensorPositionY[i] = SensorPoiniset.value(strSession,50).toInt();
		strSession = QString("/SensorVisable/%1").arg(i);
		bool bVisible = SensorPoiniset.value(strSession,1).toBool();

		SensorStatusLabel *labelSensorStatus = new SensorStatusLabel(widgetPLCImage);
		labelSensorStatus->setFixedSize(30,30);
		labelSensorStatus->setMaxWidth(geometry().width());
		labelSensorStatus->setMaxHeight(geometry().height());
		labelSensorStatus->move(SensorPositionX[i],SensorPositionY[i]);
		labelSensorStatus->setVisible(bVisible);
 		listStatuslabel.append(labelSensorStatus);
	}


 	for (int i=0;i<12;i++)
 	{
 		CameraStatusLabel *labelSensorStatus = new CameraStatusLabel(widgetPLCInfo);
 		listStatuslabelIN0.append(labelSensorStatus);
		QLabel *labelSensorText = new QLabel(widgetPLCInfo);
		listSensorTextIN0.append(labelSensorText);
 	}

 	for (int i=0;i<6;i++)
 	{
 		CameraStatusLabel *labelSensorStatus = new CameraStatusLabel(widgetPLCInfo);
 		listStatuslabelIN1.append(labelSensorStatus);
		QLabel *labelSensorText = new QLabel(widgetPLCInfo);
		listSensorTextIN1.append(labelSensorText);
	}
 	for (int i=0;i<8;i++)
 	{
 		CameraStatusLabel *labelSensorStatus = new CameraStatusLabel(widgetPLCInfo);
 		listStatuslabelOUT0.append(labelSensorStatus);
		QLabel *labelSensorText = new QLabel(widgetPLCInfo);
		listSensorTextOUT0.append(labelSensorText);
 	}
 	for (int i=0;i<4;i++)
 	{
 		CameraStatusLabel *labelSensorStatus = new CameraStatusLabel(widgetPLCInfo);
 		listStatuslabelOUT1.append(labelSensorStatus);
		QLabel *labelSensorText = new QLabel(widgetPLCInfo);
		listSensorTextOUT1.append(labelSensorText);
 	}
	initial();
}
Widget_PLC1::~Widget_PLC1()
{
	pThreadReadPLCStatus->StopThread();
	pThreadReadPLCStatus->WaitThreadStop();
}
void Widget_PLC1::slots_intoWidget()
{
}
bool Widget_PLC1::leaveWidget()
{
	return true;
}
void Widget_PLC1::initial()
{
	QGroupBox *groupboxPLCIN = new QGroupBox(widgetPLCInfo);
	groupboxPLCIN->setTitle(tr("PLCIN"));
	QGridLayout *gridLayoutPLCIN0 = new QGridLayout();
	for (int i=0;i<12;i++)
	{
		CameraStatusLabel *labelSensorStatus = listStatuslabelIN0.at(i);
		gridLayoutPLCIN0->addWidget(labelSensorStatus,2*i/10,2*i%10);
		QLabel *labelSensorText = listSensorTextIN0.at(i);
		gridLayoutPLCIN0->addWidget(labelSensorText,(2*i+1)/10,(2*i+1)%10);
	}
	QGridLayout *gridLayoutPLCIN1 = new QGridLayout();
	for (int i=0;i<6;i++)
	{
		CameraStatusLabel *labelSensorStatus = listStatuslabelIN1.at(i);
		gridLayoutPLCIN1->addWidget(labelSensorStatus,2*i/10,2*i%10);
		QLabel *labelSensorText = listSensorTextIN1.at(i);
		gridLayoutPLCIN1->addWidget(labelSensorText,(2*i+1)/10,(2*i+1)%10);
	}
	QGridLayout *gridLayoutPLCIN = new QGridLayout(groupboxPLCIN);
	gridLayoutPLCIN->addLayout(gridLayoutPLCIN0,0,0);
	gridLayoutPLCIN->addLayout(gridLayoutPLCIN1,1,0);


	QGroupBox *groupBoxPLCOUT = new QGroupBox(widgetPLCInfo);
	groupBoxPLCOUT->setTitle(tr("PLCOUT"));
	QGridLayout *gridLayoutPLCOUT0 = new QGridLayout();
	for (int i=0;i<8;i++)
	{
		CameraStatusLabel *labelSensorStatus = listStatuslabelOUT0.at(i);
		gridLayoutPLCOUT0->addWidget(labelSensorStatus,2*i/10,2*i%10);
		QLabel *labelSensorText = listSensorTextOUT0.at(i);
		gridLayoutPLCOUT0->addWidget(labelSensorText,(2*i+1)/10,(2*i+1)%10);
	}
	QGridLayout *gridLayoutPLCOUT1 = new QGridLayout();
	for (int i=0;i<4;i++)
	{
		CameraStatusLabel *labelSensorStatus = listStatuslabelOUT1.at(i);
		gridLayoutPLCOUT1->addWidget(labelSensorStatus,2*i/10,2*i%10);
		QLabel *labelSensorText = listSensorTextOUT1.at(i);
		gridLayoutPLCOUT1->addWidget(labelSensorText,(2*i+1)/10,(2*i+1)%10);
	}
	QGridLayout *gridLayoutPLCOUT = new QGridLayout(groupBoxPLCOUT);
	gridLayoutPLCOUT->addLayout(gridLayoutPLCOUT0,0,0);
	gridLayoutPLCOUT->addLayout(gridLayoutPLCOUT1,1,0);

	QVBoxLayout *layoutPLCInfo = new QVBoxLayout(widgetPLCInfo);
	layoutPLCInfo->addWidget(groupboxPLCIN);
	layoutPLCInfo->addWidget(groupBoxPLCOUT);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(widgetPLCImage);
	mainLayout->addWidget(widgetPLCInfo);

	setLayout(mainLayout);

	setLabelText();
}
void Widget_PLC1::slots_ResetSensorPosition()
{
	for (int i=0;i<10;i++)
	{
		listStatuslabel.at(i)->setVisible(true);
		listStatuslabel.at(i)->move((i+1)*40,50);

	}
}
void Widget_PLC1::slots_SetPLCPara()
{
	pThreadReadPLCStatus->slots_SetPLCPara();
}

void Widget_PLC1::slots_SaveSensorPosition()
{
	QSettings SensorPoiniset(m_strSensorPositionPath,QSettings::IniFormat);
	SensorPoiniset.setIniCodec(QTextCodec::codecForName("GBK"));
	QString strSession;
	for (int i=0;i<10;i++)
	{
		SensorPositionX[i] = listStatuslabel.at(i)->pos().x();
		SensorPositionY[i] = listStatuslabel.at(i)->pos().y();

		strSession = QString("/SensorPositionX/%1").arg(i);
		SensorPoiniset.setValue(strSession,SensorPositionX[i]);
		strSession = QString("/SensorPositionY/%1").arg(i);
		SensorPoiniset.setValue(strSession,SensorPositionY[i]);
		strSession = QString("/SensorVisable/%1").arg(i);
		SensorPoiniset.setValue(strSession,listStatuslabel.at(i)->isVisible());
	}
}

void Widget_PLC1::setLabelText()
{

	QSettings PLCiniset(m_strPLCInfoPath,QSettings::IniFormat);
	PLCiniset.setIniCodec(QTextCodec::codecForName("GBK"));

	QString  strSession;
	for (int i=0;i<12;i++)
	{
		if (i<10)
		{
			strSession = QString("/IO/X00%1").arg(i);
		}
		else
		{
			strSession = QString("/IO/X0%1").arg(i);
		}
		listPLCInfoIN0.append(QString::fromLocal8Bit(PLCiniset.value(strSession,tr("reserve")).toString()));
		listSensorTextIN0.at(i)->setText(listPLCInfoIN0.at(i));
	}
	for (int i=0;i<10;i++)
	{
		listStatuslabel.at(i)->setText(listPLCInfoIN0.at(i+1).left(listPLCInfoIN0.at(i).findRev(" ")));
		listStatuslabel.at(i)->setToolTip(listPLCInfoIN0.at(i+1));
	}
	for (int i=0;i<6;i++)
	{
		strSession = QString("/IO/X10%1").arg(i);
		listPLCInfoIN1.append(QString::fromLocal8Bit(PLCiniset.value(strSession,tr("reserve")).toString()));//.toLatin1().data()));
		listSensorTextIN1.at(i)->setText(listPLCInfoIN1.at(i));
	}
	for (int i=0;i<8;i++)
	{
		strSession = QString("/IO/Y00%1").arg(i);
		listPLCInfoOUT0.append(QString::fromLocal8Bit(PLCiniset.value(strSession,tr("reserve")).toString()));//.toLatin1().data()));
		listSensorTextOUT0.at(i)->setText(listPLCInfoOUT0.at(i));
	}
	for (int i=0;i<4;i++)
	{
		strSession = QString("/IO/Y10%1").arg(i);
		listPLCInfoOUT1.append(QString::fromLocal8Bit(PLCiniset.value(strSession,tr("reserve")).toString()));//.toLatin1().data()));
		listSensorTextOUT1.at(i)->setText(listPLCInfoOUT1.at(i));
	}
}

void Widget_PLC1::resizeEvent(QResizeEvent *event)
{
	for (int i=0;i<10;i++)
	{
		SensorStatusLabel *labelSensorStatus = listStatuslabel.at(i);
		labelSensorStatus->setMaxWidth(widgetPLCImage->geometry().width());
		labelSensorStatus->setMaxHeight(widgetPLCImage->geometry().height());
	}
}

void Widget_PLC1::slots_updatePLCInfo(s_PLCStatus sPLCStatus)
{
	if (sPLCStatus.bManual || sPLCStatus.bTestMode)
	{
		if (sPLCStatus.bManual)
		{
			emit signals_sendPLCStatus(-1);
		}
		else if (sPLCStatus.bTestMode)
		{
			emit signals_sendPLCStatus(-2);
		}
	}
	else 
	{
		int iAlarm = sPLCStatus.nAlarm;
		int iAlarmSignal = 0; 
		while (iAlarm!=0) // iAlarm = 2^iAlarmSignal
		{
			iAlarm = iAlarm/2;
			iAlarmSignal++;
		}
// 		if (iAlarmSignal != 0)
// 			CLogFile::write(QString("PLC Alarm:%1").arg(iAlarmSignal),CheckLog);
		emit signals_sendPLCStatus(iAlarmSignal);
	}
	labelCoderCounter->setText(tr("CoderCounter:")+QString::number(sPLCStatus.nEncoder));

	//闪烁传感器
	if (sPLCStatus.nCh0 != 0)
	{
		int tempSignal = sPLCStatus.nCh0;
		for(int i = 0;i<10;i++)
		{
			if (tempSignal%2)
			{
				if (i > 0 )
				{
					listStatuslabel.at(i-1)->BlinkCameraStatus(1);
				}
			}
			tempSignal = tempSignal/2;
		}
	}
	if (sPLCStatus.nCh0 != 0)
	{
		int tempSignal = sPLCStatus.nCh0;
		for(int i = 0;i<12;i++)
		{
			if (tempSignal%2)
			{
				listStatuslabelIN0.at(i)->BlinkCameraStatus(1);
			}
			tempSignal = tempSignal/2;
		}

	}
	if (sPLCStatus.nCh1 != 0)
	{
		int tempSignal = sPLCStatus.nCh1;
		for(int i = 0;i<6;i++)
		{
			if (tempSignal%2)
			{
				listStatuslabelIN1.at(i)->BlinkCameraStatus(1);
			}
			tempSignal = tempSignal/2;
		}
	}
	if (sPLCStatus.nCh100 != 0)
	{
		int tempSignal = sPLCStatus.nCh100;
		for(int i = 0;i<8;i++)
		{
			if (tempSignal%2)
			{
				listStatuslabelOUT0.at(i)->BlinkCameraStatus(1);
			}
			tempSignal = tempSignal/2;
		}
	}
	if (sPLCStatus.nCh101 != 0)
	{
		int tempSignal = sPLCStatus.nCh101;
		for(int i = 0;i<4;i++)
		{
			if (tempSignal%2)
			{
				listStatuslabelOUT1.at(i)->BlinkCameraStatus(1);
			}
			tempSignal = tempSignal/2;
		}
	}
}
