#include "database.h"


DataBase::DataBase(QString spath)
{
	AppPath = spath;
	sqlConnectName = "INITIA1_PRO";

	createConnection();
	createReportTable();
	createLastDataTable();
	createMoldNoReportTable();
	createLastMoldNoTable();
}
//建立一个数据库连接
bool DataBase::createConnection()
{
	//QStringList strlist = QSqlDatabase::drivers();
	QSqlDatabase m_db = QSqlDatabase::addDatabase("QSQLITE",sqlConnectName);
		m_db.setDatabaseName("Report.dat");
	if( !m_db.open())
	{
		return false;
	}
	return true;
}

//创建数据库表
bool DataBase::createReportTable()
{
	QSqlDatabase m_db = QSqlDatabase::database(sqlConnectName);
	if(!m_db.isOpen())
		m_db.open();
	QSqlQuery query(m_db);
	QString sql("create table Report(time varchar,AllCount int,FrontErrorByType blob,ClampErrorByType blob,RearErrorByType blob)");
	bool ret=query.exec(sql);
	return ret;
}

bool DataBase::createLastDataTable()
{
	QSqlDatabase m_db = QSqlDatabase::database(sqlConnectName);
	if(!m_db.isOpen())
		m_db.open();
	QSqlQuery query(m_db);
	QString sql("create table LastData(AllCount int,failCount int,iAllcount int,FrontErrorByType blob,ClampErrorByType blob,RearErrorByType blob)");
	bool ret=query.exec(sql);
	return ret;
}

bool DataBase::createMoldNoReportTable()
{
	QSqlDatabase m_db = QSqlDatabase::database(sqlConnectName);
	if(!m_db.isOpen())
		m_db.open();
	QSqlQuery query(m_db);
	QString sql("create table MoldNoReport(time varchar,MoldNoAllcount int,MoldNoFailcount int, MoldNoCount blob,MoldNoTypeCount blob)");
	bool ret=query.exec(sql);
	return ret;
}

bool DataBase::createLastMoldNoTable()
{
	QSqlDatabase m_db = QSqlDatabase::database(sqlConnectName);
	if(!m_db.isOpen())
		m_db.open();
	QSqlQuery query(m_db);
	QString sql("create table LastMoldNo(MoldNoAllcount int,MoldNoFailcount int,MoldNoCount blob,MoldNoTypeCount blob)");
	bool ret=query.exec(sql);
	return ret;
}

bool DataBase::insert(QString timestr,cErrorInfo info)
{
	QByteArray frontData,ClampData,RearData;
	frontData.append((char*)info.iFrontErrorByType,sizeof(int)*ERRORTYPE_MAX_COUNT);
	ClampData.append((char*)info.iClampErrorByType,sizeof(int)*ERRORTYPE_MAX_COUNT);
	RearData.append((char*)info.iRearErrorByType,sizeof(int)*ERRORTYPE_MAX_COUNT);

	QSqlDatabase m_db = QSqlDatabase::database(sqlConnectName);
	if(!m_db.isOpen())
		m_db.open();
	QSqlQuery query(m_db);
	QString strsql = "insert into Report(time,AllCount,FrontErrorByType,ClampErrorByType,RearErrorByType) values(:time,:AllCount,:FrontErrorByType,:ClampErrorByType,:RearErrorByType)";
	query.prepare(strsql);
	query.bindValue(":time",timestr);
	query.bindValue(":AllCount",info.iAllCount);
	query.bindValue(":FrontErrorByType",frontData);
	query.bindValue(":ClampErrorByType",ClampData);
	query.bindValue(":RearErrorByType",RearData);	
	bool ret = query.exec();
	return ret;
}


bool DataBase::queryByOnce(QString timeStr,long long &ptime,cErrorInfo &info)
{
	QByteArray frontData,ClampData,RearData;
	QSqlDatabase m_db = QSqlDatabase::database(sqlConnectName);
	if(!m_db.isOpen())
		m_db.open();
	QSqlQuery query(m_db);
	QString sql = QString("select * from Report where time = %1").arg(timeStr);
	bool ret = query.exec(sql);
	if(ret)
	{
		//QSqlRecord rec = query.record();
		while(query.next())
		{
			ptime = query.value(0).toLongLong() ;
			info.iAllCount = query.value(1).toInt();
			frontData = query.value(2).toByteArray();
			ClampData = query.value(3).toByteArray();
			RearData  = query.value(4).toByteArray();

			for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
			{
				int tmp=0;
				memcpy(&tmp,frontData.data() + sizeof(int)*i,sizeof(int));
				info.iFrontErrorByType[i]=tmp;
				tmp=0;
				memcpy(&tmp,ClampData.data() + sizeof(int)*i,sizeof(int));
				info.iClampErrorByType[i]=tmp;
				tmp=0;
				memcpy(&tmp,RearData.data() + sizeof(int)*i,sizeof(int));
				info.iRearErrorByType[i]=tmp;
			}
				
		}
	}
	return ret;
}

bool DataBase::queryByDay(QString dayStr,QList<long long> &pTimes,QList<cErrorInfo> &infos)
{
	QByteArray frontData,ClampData,RearData;
	QSqlDatabase m_db = QSqlDatabase::database(sqlConnectName);
	if(!m_db.isOpen())
		m_db.open();
	QSqlQuery query(m_db);
	QString sql = QString("select * from Report where time like '%1%' order by time asc").arg(dayStr);
	bool ret = query.exec(sql);
	if(ret)
	{
		while(query.next())
		{
			cErrorInfo tmpInfo;
			pTimes << query.value(0).toLongLong() ;
			tmpInfo.iAllCount = query.value(1).toInt();
			frontData = query.value(2).toByteArray();
			ClampData = query.value(3).toByteArray();
			RearData  = query.value(4).toByteArray();

			for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
			{
				int tmp=0;
				memcpy(&tmp,frontData.data() + sizeof(int)*i,sizeof(int));
				tmpInfo.iFrontErrorByType[i]=tmp;
				tmp=0;
				memcpy(&tmp,ClampData.data() + sizeof(int)*i,sizeof(int));
				tmpInfo.iClampErrorByType[i]=tmp;
				tmp=0;
				memcpy(&tmp,RearData.data() + sizeof(int)*i,sizeof(int));
				tmpInfo.iRearErrorByType[i]=tmp;
			}
			infos<<tmpInfo;
		}
	}
	return ret;
}

bool DataBase::queryByShift(QString pStartTime,QString pEndTime,QList<long long> &pTimes,QList<cErrorInfo> &infos)
{
	QByteArray frontData,ClampData,RearData;
	QSqlDatabase m_db = QSqlDatabase::database(sqlConnectName);
	if(!m_db.isOpen())
		m_db.open();
	QSqlQuery query(m_db);
	QString sql = QString("select * from Report where time > %1 and time <= %2 ").arg(pStartTime).arg(pEndTime);
	bool ret = query.exec(sql);
	if(ret)
	{
		while(query.next())
		{
			cErrorInfo tmpInfo;
			pTimes << query.value(0).toLongLong() ;
			tmpInfo.iAllCount = query.value(1).toInt();
			frontData = query.value(2).toByteArray();
			ClampData = query.value(3).toByteArray();
			RearData  = query.value(4).toByteArray();

			for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
			{
				int tmp=0;
				memcpy(&tmp,frontData.data() + sizeof(int)*i,sizeof(int));
				tmpInfo.iFrontErrorByType[i]=tmp;
				tmp=0;
				memcpy(&tmp,ClampData.data() + sizeof(int)*i,sizeof(int));
				tmpInfo.iClampErrorByType[i]=tmp;
				tmp=0;
				memcpy(&tmp,RearData.data() + sizeof(int)*i,sizeof(int));
				tmpInfo.iRearErrorByType[i]=tmp;
			}
			infos<<tmpInfo;
		}
	}
	return ret;
}

bool DataBase::insertLastData(int AllCount,int failCount,cErrorInfo info)
{
	QByteArray frontData,ClampData,RearData;
	frontData.append((char*)info.iFrontErrorByType,sizeof(int)*ERRORTYPE_MAX_COUNT);
	ClampData.append((char*)info.iClampErrorByType,sizeof(int)*ERRORTYPE_MAX_COUNT);
	RearData.append((char*)info.iRearErrorByType,sizeof(int)*ERRORTYPE_MAX_COUNT);

	QSqlDatabase m_db = QSqlDatabase::database(sqlConnectName);
	if(!m_db.isOpen())
		m_db.open();
	QSqlQuery query(m_db);
	QString deleteSql = "delete from LastData";
	query.exec(deleteSql);

	QString strsql = "insert into LastData(AllCount,failCount,iAllcount,FrontErrorByType,ClampErrorByType,RearErrorByType) values(:AllCount,:failCount,:iAllcount,:FrontErrorByType,:ClampErrorByType,:RearErrorByType)";
	query.prepare(strsql);
	query.bindValue(":AllCount",AllCount);
	query.bindValue(":failCount",failCount);
	query.bindValue(":iAllcount",info.iAllCount);
	query.bindValue(":FrontErrorByType",frontData);
	query.bindValue(":ClampErrorByType",ClampData);
	query.bindValue(":RearErrorByType",RearData);	
	bool ret = query.exec();
	return ret;
}

bool DataBase::queryLastData(int &AllCount,int &failCount,cErrorInfo &info)
{
	QByteArray frontData,ClampData,RearData;
	QSqlDatabase m_db = QSqlDatabase::database(sqlConnectName);
	if(!m_db.isOpen())
		m_db.open();
	QSqlQuery query(m_db);
	QString sql = QString("select * from LastData");
	bool ret = query.exec(sql);
	if(ret)
	{
		//QSqlRecord rec = query.record();
		while(query.next())
		{
			AllCount = query.value(0).toInt();
			failCount = query.value(1).toInt();
			info.iAllCount = query.value(2).toInt();
			frontData = query.value(3).toByteArray();
			ClampData = query.value(4).toByteArray();
			RearData  = query.value(5).toByteArray();

			for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
			{
				int tmp=0;
				memcpy(&tmp,frontData.data() + sizeof(int)*i,sizeof(int));
				info.iFrontErrorByType[i]=tmp;
				tmp=0;
				memcpy(&tmp,ClampData.data() + sizeof(int)*i,sizeof(int));
				info.iClampErrorByType[i]=tmp;
				tmp=0;
				memcpy(&tmp,RearData.data() + sizeof(int)*i,sizeof(int));
				info.iRearErrorByType[i]=tmp;
			}

		}
	}
	return ret;
}

bool DataBase::insetLastMoldNoData(cMoldNoErrorInfo moldNoCount)
{
	QByteArray pMoldNoCount,pMoldNoTypeCount;
	pMoldNoCount.append((char*)moldNoCount.m_iMoldNoCount,sizeof(UINT32)*MOLDNO_MAX_COUNT);
	pMoldNoTypeCount.append((char*)moldNoCount.m_iMoldNoTypeCount,sizeof(UINT32)*ERRORTYPE_MAX_COUNT*MOLDNO_MAX_COUNT);

	QSqlDatabase m_db = QSqlDatabase::database(sqlConnectName);
	if(!m_db.isOpen())
		m_db.open();
	QSqlQuery query(m_db);
	QString deleteSql = "delete from LastMoldNo";
	query.exec(deleteSql);

	QString strsql = "insert into LastMoldNo(MoldNoAllcount,MoldNoFailcount,MoldNoCount,MoldNoTypeCount) values(:MoldNoAllcount,:MoldNoFailcount,:MoldNoCount,:MoldNoTypeCount)";
	query.prepare(strsql);
	query.bindValue(":MoldNoAllcount",moldNoCount.m_iMoldNoAllCount);
	query.bindValue(":MoldNoFailcount",moldNoCount.m_iMoldNoFailCount);
	query.bindValue(":MoldNoCount",pMoldNoCount);
	query.bindValue(":MoldNoTypeCount",pMoldNoTypeCount);
	bool ret = query.exec();
	return ret;
}

bool DataBase::queryLastMoldNoData(cMoldNoErrorInfo &moldNoCount)
{
	QSqlDatabase m_db = QSqlDatabase::database(sqlConnectName);
	if(!m_db.isOpen())
		m_db.open();
	QSqlQuery query(m_db);
	QString sql = QString("select * from LastMoldNo");
	bool ret = query.exec(sql);
	if(ret)
	{
		while(query.next())
		{	
			moldNoCount.m_iMoldNoAllCount = query.value(0).toUInt();
			moldNoCount.m_iMoldNoFailCount = query.value(1).toUInt();
			QByteArray pMoldNoCount,pMoldNoTypeCount;
			pMoldNoCount = query.value(2).toByteArray();
			pMoldNoTypeCount = query.value(3).toByteArray();
			//s_MoldNoCount pTmpMoldNoCount;
			memcpy(moldNoCount.m_iMoldNoCount,pMoldNoCount.data(),sizeof(UINT32)*MOLDNO_MAX_COUNT);
			memcpy(moldNoCount.m_iMoldNoTypeCount,pMoldNoTypeCount.data(),sizeof(UINT32)*ERRORTYPE_MAX_COUNT*MOLDNO_MAX_COUNT);
		}
	}
	return ret;
}

bool DataBase::insertMoldNoData(QString timestr,cMoldNoErrorInfo info)
{
	QByteArray pMoldNoCount,pMoldNoTypeCount;
	pMoldNoCount.append((char*)info.m_iMoldNoCount,sizeof(UINT32)*ERRORTYPE_MAX_COUNT);
	pMoldNoTypeCount.append((char*)info.m_iMoldNoTypeCount,sizeof(UINT32)*ERRORTYPE_MAX_COUNT*MOLDNO_MAX_COUNT);

	QSqlDatabase m_db = QSqlDatabase::database(sqlConnectName);
	if(!m_db.isOpen())
		m_db.open();
	QSqlQuery query(m_db);
	QString strsql = "insert into MoldNoReport(time,MoldNoAllcount,MoldNoFailcount,MoldNoCount,MoldNoTypeCount) values(:time,:MoldNoAllcount,:MoldNoFailcount,:MoldNoCount,:MoldNoTypeCount)";
	query.prepare(strsql);
	query.bindValue(":time",timestr);
	query.bindValue(":MoldNoAllcount",info.m_iMoldNoAllCount);
	query.bindValue(":MoldNoFailcount",info.m_iMoldNoFailCount);
	query.bindValue(":MoldNoCount",pMoldNoCount);
	query.bindValue(":MoldNoTypeCount",pMoldNoTypeCount);	
	bool ret = query.exec();
	return ret;
}

bool DataBase::queryMoldNoByDay(QString dayStr,QList<long long> &pTimes,QList<cMoldNoErrorInfo> &infos)
{
	QByteArray pMoldNoCount,pMoldNoTypeCount;
	QSqlDatabase m_db = QSqlDatabase::database(sqlConnectName);
	if(!m_db.isOpen())
		m_db.open();
	QSqlQuery query(m_db);
	QString sql = QString("select * from MoldNoReport where time like '%1%' order by time asc").arg(dayStr);
	bool ret = query.exec(sql);
	if(ret)
	{
		while(query.next())
		{
			cMoldNoErrorInfo tmpInfo;
			pTimes << query.value(0).toLongLong() ;
			tmpInfo.m_iMoldNoAllCount = query.value(1).toUInt();
			tmpInfo.m_iMoldNoFailCount = query.value(2).toUInt();
			pMoldNoCount = query.value(3).toByteArray();
			pMoldNoTypeCount  = query.value(4).toByteArray();
			memcpy(tmpInfo.m_iMoldNoCount,pMoldNoCount.data(),sizeof(UINT32)*MOLDNO_MAX_COUNT);
			memcpy(tmpInfo.m_iMoldNoTypeCount,pMoldNoTypeCount.data(),sizeof(UINT32)*ERRORTYPE_MAX_COUNT*MOLDNO_MAX_COUNT);
			infos<<tmpInfo;
		}
	}
	return ret;
}

bool DataBase::queryMoldNoByOnce(QString timeStr,long long &ptime,cMoldNoErrorInfo &info)
{
	QByteArray pMoldNoCount,pMoldNoTypeCount;
	QSqlDatabase m_db = QSqlDatabase::database(sqlConnectName);
	if(!m_db.isOpen())
		m_db.open();
	QSqlQuery query(m_db);
	QString sql = QString("select * from MoldNoReport where time = %1").arg(timeStr);
	bool ret = query.exec(sql);
	if(ret)
	{
		while(query.next())
		{
			ptime = query.value(0).toLongLong() ;
			info.m_iMoldNoAllCount = query.value(1).toUInt();
			info.m_iMoldNoFailCount = query.value(2).toUInt();
			pMoldNoCount = query.value(3).toByteArray();
			pMoldNoTypeCount  = query.value(4).toByteArray();
			memcpy(info.m_iMoldNoCount,pMoldNoCount.data(),sizeof(UINT32)*MOLDNO_MAX_COUNT);
			memcpy(info.m_iMoldNoTypeCount,pMoldNoTypeCount.data(),sizeof(UINT32)*ERRORTYPE_MAX_COUNT*MOLDNO_MAX_COUNT);
		}
	}
	return ret;
}

