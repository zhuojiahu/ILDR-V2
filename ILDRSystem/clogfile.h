#ifndef CLOGFILE_H
#define CLOGFILE_H

#include <QString>
#include <QApplication>
#include <QMutex>
#include <QThread>

//#include "common.h"
#include "stateTool.h"
#include "ConfigInfo.h"

struct s_LogInfo
{
	QString strInfo;
	e_SaveLogType logtype;
	int showInferface;
	s_LogInfo()
	{
		strInfo = "";
		logtype = OperationLog;
		showInferface = 0;
	}

};
class WriteLogThread : public QThread
{
	Q_OBJECT

public:
	WriteLogThread(QObject *parent);
	WriteLogThread();
	~WriteLogThread();

	QList<s_LogInfo> listLog;
	QString strLastFilePath;
 	QMutex mutexLog;
private:
	bool m_bStopThread;
// 	QMutex mutxLogFile;
	QString m_strlogfilepath;
	QString m_strLogInfo;

signals:
public slots:
// 	void slots_write();

public:
	// 	s_PLCStatus GetPLCStatus();
	void run();
	void StartThread();
	void StopThread();
	void WaitThreadStop();

	void Initpath(e_SaveLogType logtype);
	void write(QString strInfo,e_SaveLogType logtype, int showInterface = 1);

};

class CLogFile
{
public:
	//enum e_SaveLogType	    //日志类型
	//{
	//	OperationLog = 0,			//操作日志
	//	CheckLog,		            //检测日志
	//	MendParamLog,			    //修改参数日志
	//	AbnormityLog,               //异常日志
	//	StatInfo,					//检测统计信息日志
	//	AlgorithmDLL				//算法库日志
	//};
public:
	~CLogFile();
    static CLogFile* getInstance();
    static void write(QString strInfo,e_SaveLogType logtype, int showInterface = 1);
private:
    CLogFile();
    void writePrivate(QString strInfo,e_SaveLogType logtype, int showInterface = 1);
private:
	QMutex mutexWriteLogFile;
// 	static void Initpath(e_SaveLogType logtype);
	QString strLastFilePath;
	WriteLogThread *pThreadWriteLog;

    static CLogFile* instance;
};

#endif // CLOGFILE_H
