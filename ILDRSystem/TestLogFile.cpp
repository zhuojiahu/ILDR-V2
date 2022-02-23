#include "TestLogFile.h"

TestLogFile* TestLogFile::instance = NULL;
QMutex TestLogFile::mutex;
TestLogFile::TestLogFile(void)
{ 
	//pthread_mutex_init()
}


TestLogFile::~TestLogFile(void)
{
}

void TestLogFile::InputLogData( QString valiableName,QString valiableData )
{
	QFile logFile("log.txt");
	if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append))
	{
		return;
	}
	QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");   
	QString message = QString("%1 --> %2 ： %3").arg(current_date_time).
												 arg(valiableName).
												 arg(valiableData);  
	QTextStream textStream(&logFile);  
	textStream << message << "\r\n";  
	logFile.flush();  
	logFile.close();
}

void TestLogFile::InputLogData( QString valiableName,QString valiableData, int bottleNum )
{
	QFile logFile("log.txt");
	if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append))
	{
		return;
	}
	QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");   
	QString message = QString("%1 --> %2 ： %3 ，bottle : %4").arg(current_date_time).
												arg(valiableName).
												arg(valiableData).
												arg(bottleNum);  
	QTextStream textStream(&logFile);  
	textStream << message << "\r\n";  
	logFile.flush();  
	logFile.close();
}

TestLogFile * TestLogFile::GetInstance()
{
	mutex.tryLock(); 
	if(instance == NULL)  //判断是否第一次调用  
		instance = new TestLogFile(); 
	mutex.unlock();  
	return instance;  
}
