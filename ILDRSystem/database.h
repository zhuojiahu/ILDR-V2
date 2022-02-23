#ifndef DATABASE_H
#define DATABASE_H

#include "qsqldatabase.h"
#include "qsqlquery.h"
#include "qsqldriver.h"
#include "qsqlrecord.h"
#include <QVariant>
#include "widget_count.h"

class DataBase
{
public:
	DataBase(QString spath);

public:
	bool createConnection();  //����һ������
	bool createReportTable();       //�������ݿ��
	bool createLastDataTable();       //�������ڱ�����һ����ʷ���ݵı�
	bool createMoldNoReportTable();	//�������ڱ���ģ��ͳ�Ƶ���ʷ����
	bool createLastMoldNoTable();	//�������ڱ���ģ��ͳ�Ƶ���һ����ʷ����
	bool insert(QString timestr,cErrorInfo info);
	bool queryByOnce(QString timeStr,long long &ptime,cErrorInfo &info);										//timeStr: yyyyMMddhhmm
	bool queryByDay(QString dayStr,QList<long long> &pTimes,QList<cErrorInfo> &infos);							//daystr : yyyyMMdd
	bool queryByShift(QString pStartTime,QString pEndTime,QList<long long> &pTimes,QList<cErrorInfo> &infos);	//daystr : yyyyMMdd
	bool insertLastData(int AllCount,int failCount,cErrorInfo info);	//������ʷ����
	bool queryLastData(int &AllCount,int &failCount,cErrorInfo &info);	//��ѯ��ʷ����
	
	bool insetLastMoldNoData(cMoldNoErrorInfo moldNoCount);		//������ʷģ������
	bool queryLastMoldNoData(cMoldNoErrorInfo &moldNoCount);	//��ѯ��һ����ʷ���� 
	bool insertMoldNoData(QString timestr,cMoldNoErrorInfo info);	//����ÿСʱ����Сʱģ��ͳ�Ƶ���ʷ����
	bool queryMoldNoByDay(QString dayStr,QList<long long> &pTimes,QList<cMoldNoErrorInfo> &infos);	//daystr : yyyyMMdd  ��ѯĳһ���ģ��ͳ����ʷ����
	bool queryMoldNoByOnce(QString timeStr,long long &ptime,cMoldNoErrorInfo &info);				//timeStr: yyyyMMddhhmm

private:
	QString sqlConnectName;
	QString AppPath;
};
#endif