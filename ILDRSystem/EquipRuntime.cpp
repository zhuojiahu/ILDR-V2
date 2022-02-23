#include "EquipRuntime.h"

#include <QMutex>
#include <QSettings>
#include <QTextCodec>
#include "ILDRSystem.h"

extern SysMainUI *pMainFrm;

#define HOUR 60*60*1000

QScopedPointer<EquipRuntime> EquipRuntime::self;
EquipRuntime *EquipRuntime::Instance()
{
	if (self.isNull()) {
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		if (self.isNull()) {
			self.reset(new EquipRuntime);
		}
	}
	return self.data();
}

EquipRuntime::EquipRuntime(QObject *parent)
	: QObject(parent)
{
	LogFileName ="EquipMaintLog.ini";
	saveInterval =  HOUR;
	//saveInterval =  60 * 1000;
	startTime = QDateTime::currentDateTime();
	m_Timer=new QTimer(this);
	m_Timer->setInterval(saveInterval);
	connect(m_Timer,SIGNAL(timeout()),this,SLOT(Slots_timer()));

	QFile m_LogFile(LogFileName);
	if (!m_LogFile.exists())
	{
		initLogFile();
	}
	ReadLogFile();
}

EquipRuntime::~EquipRuntime()
{
	//EquipExitLogFile();
}

void EquipRuntime::start()
{
	m_Timer->start();
}

void EquipRuntime::stop()
{
	m_Timer->stop();
}

void EquipRuntime::initLogFile()
{
	QSettings m_logFile(LogFileName , QSettings::IniFormat);
	m_logFile.setIniCodec(QTextCodec::codecForName("GBK"));
	for (int i=0;i<pMainFrm->m_sRuntimeInfo.total;i++)
	{
		m_logFile.setValue(QString("Alarm%1_CumulativeTime").arg(i+1),0);
	}
}

void EquipRuntime::ReadLogFile()
{
	QSettings m_logFile(LogFileName , QSettings::IniFormat);
	m_logFile.setIniCodec(QTextCodec::codecForName("GBK"));
	for (int i=0;i<pMainFrm->m_sRuntimeInfo.total;i++)
	{
		m_CumulTime <<  m_logFile.value(QString("Alarm%1_CumulativeTime").arg(i+1)).toInt();
		m_AlarmsSatus<<false;
	}
}

void EquipRuntime::ResetLogFile()
{
	QSettings m_logFile(LogFileName , QSettings::IniFormat);
	m_logFile.setIniCodec(QTextCodec::codecForName("GBK"));
	for (int i=0;i<pMainFrm->m_sRuntimeInfo.total;i++)
	{
		if (m_AlarmsSatus[i])
		{
			m_CumulTime[i] = 0;
			emit SendAlarms(i,false);
		}
		else
		{
			if (pMainFrm->m_sRuntimeInfo.AlarmsEnable[i])
			{
				QDateTime endTime = QDateTime::currentDateTime();
				int hours=startTime.secsTo(endTime)/ (60*60);
				m_CumulTime[i] += hours;
			}
		}
	}
	startTime = QDateTime::currentDateTime();
}

void EquipRuntime::EquipExitLogFile()
{
	QDateTime endTime = QDateTime::currentDateTime();
	int hours=startTime.secsTo(endTime)/ (60*60);
	QSettings m_logFile(LogFileName , QSettings::IniFormat);
	m_logFile.setIniCodec(QTextCodec::codecForName("GBK"));
	for (int i=0;i<pMainFrm->m_sRuntimeInfo.total;i++)
	{
		if (pMainFrm->m_sRuntimeInfo.AlarmsEnable[i])
		{
			m_logFile.setValue(QString("Alarm%1_CumulativeTime").arg(i+1),m_CumulTime[i]+hours);
		}
	}
}

void EquipRuntime::Slots_timer()
{
	bool temp=false;
	m_AlarmsSatus.clear();
	QDateTime CurrentTime=QDateTime::currentDateTime();
	int hours=startTime.secsTo(CurrentTime)/(60*60);
	for (int i=0;i<pMainFrm->m_sRuntimeInfo.total;i++)
	{
		if (pMainFrm->m_sRuntimeInfo.AlarmsEnable[i])
		{
			//if ( m_CumulTime[i] +hours >= pMainFrm->m_sRuntimeInfo.AlarmsDays[i])
			if ( (m_CumulTime[i] +hours)/24 >= pMainFrm->m_sRuntimeInfo.AlarmsDays[i])
			{
				if (!temp)
				{
					pMainFrm->sVersion =QString("<font style = 'color:red;'> %1 </font>").arg(pMainFrm->m_sRuntimeInfo.AlarmsInfo[i]);
					temp = true;
				}
				m_AlarmsSatus << true;
				emit SendAlarms(i,true);
				continue;
			}
		}
		m_AlarmsSatus << false;
	}
}
