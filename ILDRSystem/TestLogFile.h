#pragma once
#include <qapplication.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <QDebug>  
#include <QTextStream>  
#include <QDateTime>  
#include <QFile> 
#include <QString>
#include <QMutex>
class TestLogFile
{
public:
	TestLogFile(void);
	~TestLogFile(void);
	void InputLogData(QString valiableName,QString valiableData);
	void InputLogData(QString valiableName,QString valiableData, int bottleNum);
private:
	static QMutex mutex;
public:
	static TestLogFile* instance;
public:  
	static TestLogFile * GetInstance();  
};

