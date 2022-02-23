#ifndef WIDGET_COUNT_H
#define WIDGET_COUNT_H


#include <QWidget>
#include "ui_widget_count.h"

#include <QPushButton>
#include <QStandardItemModel>
#include <QSettings>
#include <QThread>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>

#include "common.h"
#include "pieview.h"
#include "qcustomplot.h"
#include "ExcelObject.h"

#include<windows.h>
#include<tlhelp32.h>

//缺陷统计类
class cErrorInfo;
//模号统计类
class cMoldNoErrorInfo;

//错误信息类-单一缺陷，未使用
class cErrorTypeCountInfo;
//错误信息类-每个小时错误，未使用
class cErrorCountbyTime;

//自定义QCustomPlot直方图bar
class CustomBars;

class ExportExcelThread;

//模号统计结构体
typedef struct _MoldNoCount
{
	UINT32 m_iMoldNoAllCount;
	UINT32 m_iMoldNoFailCount;
	UINT32 m_iMoldNoCount[100];
	UINT32 m_iMoldNoTypeCount[ERRORTYPE_MAX_COUNT][100];

	void init()
	{
		m_iMoldNoAllCount = 0;
		m_iMoldNoFailCount = 0;
		for (int i=0;i<100;i++)
		{
			m_iMoldNoCount[i] = 0;
			for (int j=0;j<ERRORTYPE_MAX_COUNT;j++)
			{
				m_iMoldNoTypeCount[j][i] = 0;
			}
		}
	}

}s_MoldNoCount;

class widget_count : public QWidget
{
	Q_OBJECT

public:
	widget_count(QWidget *parent = 0);
	~widget_count();

	enum table2UpdateType
	{
		APPEND =0,
		UPDATE
	};
	/************************************************************************
	 * @brief:获取当前显示报表的模式
	 * @return: 0 - 缺陷统计 ， 1 - 模号统计
	 ***********************************************************************/
	int GetCurrentReportMode();

private:
	void init();
	void openInNotePad(QString str);
	DWORD GetProcessIdFromName(const char*processName);
	QTime initTime(QTime pTime);

private slots:
	void slots_turnPage();
	void slot_saveRecordOK_clicked();
	void slot_saveRecordCancel_clicked();
	void slot_shiftOK_clicked();
	void slot_shiftCancel_clicked();
	void slots_ShowPie(QModelIndex);
	void slot_HistorySearch_clicked();
	void slot_OpenRecord_clicked();
	void slot_DeleteRecord_clicked();
	void slot_OpenExcel_clicked();
	void slot_SearchShift_clicked();
	void slot_MoldNoCheckBox_clicked(int id);
	void slot_ShowMoldNoCount(QList<int> nMoldNos,bool isOnlyUpdate);
	void slot_changeReport(int index);
	void slot_updateMoldNoTable2(QModelIndex);

signals:
	void updateRecordSet();
	void updateShiftSet();
	void updateMoldNo(QList<int> nMoldNos,bool isOnlyUpdate=true);

public slots:
	/**********************************************************
		只刷新总数、踢废数、踢废率， modelRate可以设-1
		只刷新读模率， total 或 failNum 设为 -1
	***********************************************************/
	void slots_updateCountInfo(int total,int failNum,float modelRate);
	/**********************************************************
		更新显示每个相机检测的缺陷数
	***********************************************************/
	void slots_UpdateTable1(cErrorInfo pCountdates);
	/**********************************************************
		显示每半个小数或一个小时数据
		删除表格已有数据行，重新添加所有数据
		若List为空，只清除表格数据
	***********************************************************/
	void slots_UpdateTable2(QList<long long> pTimes,QList<cErrorInfo> pInfos);
	/**********************************************************
		显示每半个小数或一个小时数据
		重载，只添加一个数据，插入到行尾，不删除已有表格数据
	***********************************************************/
	void slots_UpdateTable2(QString ptime ,cErrorInfo pCountdate);
	/**********************************************************
		绘制每小时缺陷占比饼图
	***********************************************************/
	void slots_ShowPieImage1(cErrorInfo pCountdates);
	/**********************************************************
		绘制每个班次缺陷占比饼图,暂时未使用
	***********************************************************/
	void slots_ShowPieImage2(QList<cErrorTypeCountInfo> pCountdates);
	/**********************************************************
		直方图：绘制每小时检测总数与踢废总数 
	***********************************************************/
	void slots_ShowShiftIamge(QString startTime,QString endTime,QList<long long> pTimes,QList<cErrorInfo> pInfos);
	/**********************************************************
		直方图：绘制班次检测总数与踢废总数 
	***********************************************************/
	void slots_ShowShiftIamge(QList<long long> pTimes,QList<cErrorInfo> pInfos);
	/**********************************************************
		模号统计表：刷新实时数据 
	***********************************************************/
	void slots_updateMoldNoCount();
	/**********************************************************
		模号统计表：刷新每小时数据 
	***********************************************************/
	void slots_updateMoldNoInfo(QList<long long> pTimes,QList<cMoldNoErrorInfo> pInfos);
	/**********************************************************
		模号统计表：显示每小时详细数据 
	***********************************************************/
	void slots_updateMoldNoInfo2(long long time,cMoldNoErrorInfo pInfo);


private:
	Ui::widget_count ui;
	QPushButton *buttonTurn;
	QStandardItemModel *table1Model;
	QStandardItemModel *table2Model;
	QStandardItemModel *table3Model;
	QStandardItemModel *table4Model;
	QStandardItemModel *table5Model;
	PieView *pieImage1;
	QStandardItemModel *PieModel_1;

	PieView *pieImage2;
	QStandardItemModel *PieModel_2;

	QList<QColor> PieItemcolors;
	ExportExcelThread *ExportThread;
	QList<QCheckBox *> m_iMoldNoCheckBoxs;
	QList<int> CurSelectMoldNos;
	QList<int> nCurErrorTypes;
	//QTimer *MoldNoTimer;
};

class ExportExcelThread :public QThread
{
	Q_OBJECT
public:
	ExportExcelThread(QObject *parent = 0);
	~ExportExcelThread();

	void run();
	//void InitShiftTime(QTime pShift1,QTime pShift2,QTime pShift3);

public:
	QDate m_Date;

private:
	QList<QTime> m_shifts;
	int curShift;
	bool iShowAllnum;
};

//自定义QCustomPlot直方图bar
class CustomBars :public QCPBars
{
public:
	explicit CustomBars(QCPAxis *keyAxis , QCPAxis *valueAxis);

	enum CustomBarsFlags{
		CustomBars_NULL = 0,
		CustomBars_Rate
	};

	Qt::Alignment textAlignment() const { return mTextAlignment;}
	double spacing() const {return mSpacing;}
	QFont font() const {return mFont;}

	void setTextAlignment(Qt::Alignment alignment);
	void setSpacing(double spacing);
	void setFont(const QFont &font);

	void setTextSuffix(QString Suffix);

protected:
	Qt::Alignment mTextAlignment;
	double mSpacing;
	QFont mFont;
	CustomBarsFlags m_Flags;
	QString m_TextSuffix;

	virtual void draw(QCPPainter *painter) Q_DECL_OVERRIDE;
};

//错误信息类-单一缺陷
class cErrorTypeCountInfo
{
public:
	cErrorTypeCountInfo()
	{
		iErrorType =0;
		iErrorTxt = "";
		iErrorFailCount = 0;
		iFailCount = 0;
		iCheckCount = 0;
		iFrontCount = 0;
		iClampCount = 0;
		iRearCount = 0;
	}
	//各相机求和
	int sum()
	{		
		return iFrontCount+iClampCount+iRearCount;
	}

	void operator= (const cErrorTypeCountInfo pErrorInfo)
	{
		iErrorType = pErrorInfo.iErrorType;
		iErrorTxt = pErrorInfo.iErrorTxt;
		iErrorFailCount = pErrorInfo.iErrorFailCount;
		iFailCount = pErrorInfo.iFailCount;
		iCheckCount = pErrorInfo.iCheckCount;
		iFrontCount = pErrorInfo.iFrontCount;
		iClampCount = pErrorInfo.iClampCount;
		iRearCount =  pErrorInfo.iRearCount;
	}

	void clear()
	{
		iErrorType =0;
		iErrorTxt = "";
		iErrorFailCount =0;
		iFailCount = 0;
		iCheckCount = 0;
		iFrontCount = 0;
		iClampCount = 0;
		iRearCount = 0;
	}

	//缺陷类型
	int iErrorType;
	//缺陷名称（中 或 英）
	QString iErrorTxt;
	//某种缺陷踢废总数
	int iErrorFailCount;
	//踢废总数
	int iFailCount;
	//检测总数
	int iCheckCount;
	//前壁踢废数
	int iFrontCount;
	//夹持踢废数
	int iClampCount;
	//后壁踢废数
	int iRearCount;
};

//错误信息类-每个小时错误
class cErrorCountbyTime
{
public:
	cErrorCountbyTime()
	{
		iTime = 0;
		iAllcount = 0;
		iErrorCount = 0;
		iFrontCount = 0;
		iClampCount = 0;
		iRearCount = 0;
	}

	//同一时间下单个相机不同缺陷相加总数
	void operator+= (const cErrorCountbyTime pCountInfo)
	{
		iFrontCount += pCountInfo.iFrontCount;
		iClampCount += pCountInfo.iClampCount;
		iRearCount  += pCountInfo.iRearCount;
	}

	void operator= (const cErrorCountbyTime pCountInfo)
	{
		iTime = pCountInfo.iTime;
		iAllcount = pCountInfo.iAllcount;
		iErrorCount = pCountInfo.iErrorCount;
		iFrontCount = pCountInfo.iFrontCount;
		iClampCount = pCountInfo.iClampCount;
		iRearCount =  pCountInfo.iRearCount;
	}

	void clear()
	{
		iTime = 0;
		iAllcount = 0;
		iErrorCount = 0;
		iFrontCount = 0;
		iClampCount = 0;
		iRearCount = 0;
	}
	//转换时间字符串格式 例：2021-07-27 16:00
	QString GetTimeStr()
	{
		QDateTime pTime = QDateTime::fromString(QString::number(iTime),"yyyyMMddhhmm");
		return pTime.toString("yyyy-MM-dd hh:mm");
	}
	//获取缺陷率
	QString GetFailRate()
	{
		double pRate=0.0;
		if (iAllcount != 0)
			pRate =(double)iErrorCount / iAllcount;
		return QString::number(pRate,'f',2)+"%";
	}

	//时间
	long long iTime;
	//检测总数
	int iAllcount;
	//缺陷总数
	int iErrorCount;
	//前壁踢废数
	int iFrontCount;
	//夹持踢废数
	int iClampCount;
	//后壁踢废数
	int iRearCount;
};

class cErrorInfo
{
public:
	cErrorInfo()
	{
		iAllCount = 0 ;
		iFailCount = 0;
		FrontCount = 0;
		ClampCount = 0;
		RearCount = 0;
		for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
		{
			iErrorByType[i] = 0;
			iFrontErrorByType[i] = 0;
			iClampErrorByType[i] = 0;
			iRearErrorByType[i] = 0;
		}
	}

	void operator+= (const cErrorInfo &pCountInfo)
	{
		iAllCount  += pCountInfo.iAllCount ;
		iFailCount += pCountInfo.iFailCount;
		FrontCount += pCountInfo.FrontCount;
		ClampCount += pCountInfo.ClampCount;
		RearCount  += pCountInfo.RearCount;
		for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
		{
			iErrorByType[i]		 += pCountInfo.iErrorByType[i];
			iFrontErrorByType[i] += pCountInfo.iFrontErrorByType[i];
			iClampErrorByType[i] += pCountInfo.iClampErrorByType[i];
			iRearErrorByType[i]  += pCountInfo.iRearErrorByType[i] ;
		}
	}

	cErrorInfo operator + (const cErrorInfo &pCountInfo)
	{
		cErrorInfo tmp;
		tmp.iAllCount = this->iAllCount   + pCountInfo.iAllCount ;
		tmp.iFailCount= this->iFailCount  + pCountInfo.iFailCount;
		tmp.FrontCount= this->FrontCount  + pCountInfo.FrontCount;
		tmp.ClampCount= this->ClampCount  + pCountInfo.ClampCount;
		tmp.RearCount = this->RearCount   + pCountInfo.RearCount;
		for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
		{
			tmp.iErrorByType[i]		 = this->iErrorByType[i]	  + pCountInfo.iErrorByType[i];
			tmp.iFrontErrorByType[i] = this->iFrontErrorByType[i] + pCountInfo.iFrontErrorByType[i];
			tmp.iClampErrorByType[i] = this->iClampErrorByType[i] + pCountInfo.iClampErrorByType[i];
			tmp.iRearErrorByType[i]  = this->iRearErrorByType[i]  + pCountInfo.iRearErrorByType[i] ;
		}
		return tmp;
	}

	void operator-= (const cErrorInfo &pCountInfo)
	{
		iAllCount  -= pCountInfo.iAllCount ;
		iFailCount -= pCountInfo.iFailCount;
		FrontCount -= pCountInfo.FrontCount;
		ClampCount -= pCountInfo.ClampCount;
		RearCount  -= pCountInfo.RearCount;
		for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
		{
			iErrorByType[i]		 -= pCountInfo.iErrorByType[i];
			iFrontErrorByType[i] -= pCountInfo.iFrontErrorByType[i];
			iClampErrorByType[i] -= pCountInfo.iClampErrorByType[i];
			iRearErrorByType[i]  -= pCountInfo.iRearErrorByType[i] ;
		}
	}

	cErrorInfo operator - (const cErrorInfo &pCountInfo)
	{
		cErrorInfo tmp;
		tmp.iAllCount = this->iAllCount   - pCountInfo.iAllCount ;
		tmp.iFailCount= this->iFailCount  - pCountInfo.iFailCount;
		tmp.FrontCount= this->FrontCount  - pCountInfo.FrontCount;
		tmp.ClampCount= this->ClampCount  - pCountInfo.ClampCount;
		tmp.RearCount = this->RearCount   - pCountInfo.RearCount;
		for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
		{
			tmp.iErrorByType[i]		 = this->iErrorByType[i]	  - pCountInfo.iErrorByType[i];
			tmp.iFrontErrorByType[i] = this->iFrontErrorByType[i] - pCountInfo.iFrontErrorByType[i];
			tmp.iClampErrorByType[i] = this->iClampErrorByType[i] - pCountInfo.iClampErrorByType[i];
			tmp.iRearErrorByType[i]  = this->iRearErrorByType[i]  - pCountInfo.iRearErrorByType[i] ;
		}
		return tmp;
	}

	void operator= (const cErrorInfo &pCountInfo)
	{
		iAllCount =	pCountInfo.iAllCount ;
		iFailCount = pCountInfo.iFailCount;
		FrontCount = pCountInfo.FrontCount;
		ClampCount = pCountInfo.ClampCount;
		RearCount = pCountInfo.RearCount;
		for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
		{
			iErrorByType[i]		 = pCountInfo.iErrorByType[i];
			iFrontErrorByType[i] = pCountInfo.iFrontErrorByType[i];
			iClampErrorByType[i] = pCountInfo.iClampErrorByType[i];
			iRearErrorByType[i]  = pCountInfo.iRearErrorByType[i] ;
		}
	}

	int GetFrontCount()
	{
		int pCount=0;
		for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
		{
			pCount += iFrontErrorByType[i];
		}
		return pCount;
	}

	int GetClampCount()
	{
		int pCount=0;
		for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
		{
			pCount += iClampErrorByType[i];
		}
		return pCount;
	}

	int GetRearCount()
	{
		int pCount=0;
		for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
		{
			pCount += iRearErrorByType[i];
		}
		return pCount;
	}

	int GetFailCount()
	{
		return GetFrontCount() + GetClampCount() + GetRearCount();
	}

	double GetFailRate()
	{
		if (iAllCount > 0)
			return (double)GetFailCount() / iAllCount ;
		else
			return 0.0;
		
	}

	int GetErrorByTypeCount(int ptype)
	{
		if(ptype>=ERRORTYPE_MAX_COUNT)
			return 0;
		return iFrontErrorByType[ptype] + iClampErrorByType[ptype] + iRearErrorByType[ptype];
	}

	void Clear()
	{
		iAllCount = 0 ;
		iFailCount = 0;
		FrontCount = 0;
		ClampCount = 0;
		RearCount = 0;
		for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
		{
			iErrorByType[i] = 0;
			iFrontErrorByType[i] = 0;
			iClampErrorByType[i] = 0;
			iRearErrorByType[i] = 0;
		}
	}

	//过检总数
	int iAllCount;
	//踢废总数
	int iFailCount;
	//前壁总数
	int FrontCount;
	//夹持总数
	int ClampCount;
	//后壁总数
	int RearCount;

	//单个缺陷类型踢废数
	int iErrorByType[ERRORTYPE_MAX_COUNT];

	//前壁单个缺陷类型踢废数
	int iFrontErrorByType[ERRORTYPE_MAX_COUNT];
	//夹持单个缺陷类型踢废数
	int iClampErrorByType[ERRORTYPE_MAX_COUNT];
	//后壁单个缺陷类型踢废数
	int iRearErrorByType[ERRORTYPE_MAX_COUNT];
};

class cMoldNoErrorInfo
{
public:
	cMoldNoErrorInfo()
	{
		m_iMoldNoAllCount = 0;
		m_iMoldNoFailCount = 0;
		for (int i=0;i<MOLDNO_MAX_COUNT;i++)
		{
			m_iMoldNoCount[i] = 0;
			for (int j=0;j<ERRORTYPE_MAX_COUNT;j++)
			{
				m_iMoldNoTypeCount[j][i] = 0;
			}
		}
	}

	void operator+= (const cMoldNoErrorInfo &pCountInfo)
	{
		m_iMoldNoAllCount += pCountInfo.m_iMoldNoAllCount;
		m_iMoldNoFailCount += pCountInfo.m_iMoldNoFailCount;
		for (int i=0;i<MOLDNO_MAX_COUNT;i++)
		{
			m_iMoldNoCount[i] += pCountInfo.m_iMoldNoCount[i];
			for (int j=0;j<ERRORTYPE_MAX_COUNT;j++)
			{
				m_iMoldNoTypeCount[j][i] += pCountInfo.m_iMoldNoTypeCount[j][i];
			}
		}
	}

	cMoldNoErrorInfo operator + (const cMoldNoErrorInfo &pCountInfo)
	{
		cMoldNoErrorInfo tmpCount;
		tmpCount.m_iMoldNoAllCount = this->m_iMoldNoAllCount + pCountInfo.m_iMoldNoAllCount;
		tmpCount.m_iMoldNoFailCount = this->m_iMoldNoFailCount + pCountInfo.m_iMoldNoFailCount;
		for (int i=0;i<MOLDNO_MAX_COUNT;i++)
		{
			tmpCount.m_iMoldNoCount[i] = this->m_iMoldNoCount[i] + pCountInfo.m_iMoldNoCount[i];
			for (int j=0;j<ERRORTYPE_MAX_COUNT;j++)
			{
				tmpCount.m_iMoldNoTypeCount[j][i] = this->m_iMoldNoTypeCount[j][i] + pCountInfo.m_iMoldNoTypeCount[j][i];
			}
		}
		return tmpCount;
	}


	void operator-= (const cMoldNoErrorInfo &pCountInfo)
	{
		m_iMoldNoAllCount -= pCountInfo.m_iMoldNoAllCount;
		m_iMoldNoFailCount -= pCountInfo.m_iMoldNoFailCount;
		for (int i=0;i<MOLDNO_MAX_COUNT;i++)
		{
			m_iMoldNoCount[i] -= pCountInfo.m_iMoldNoCount[i];
			for (int j=0;j<ERRORTYPE_MAX_COUNT;j++)
			{
				m_iMoldNoTypeCount[j][i] -= pCountInfo.m_iMoldNoTypeCount[j][i];
			}
		}
	}

	cMoldNoErrorInfo operator - (const cMoldNoErrorInfo &pCountInfo)
	{
		cMoldNoErrorInfo tmpCount;
		tmpCount.m_iMoldNoAllCount = this->m_iMoldNoAllCount - pCountInfo.m_iMoldNoAllCount;
		tmpCount.m_iMoldNoFailCount = this->m_iMoldNoFailCount - pCountInfo.m_iMoldNoFailCount;
		for (int i=0;i<MOLDNO_MAX_COUNT;i++)
		{
			tmpCount.m_iMoldNoCount[i] = this->m_iMoldNoCount[i] - pCountInfo.m_iMoldNoCount[i];
			for (int j=0;j<ERRORTYPE_MAX_COUNT;j++)
			{
				tmpCount.m_iMoldNoTypeCount[j][i] = this->m_iMoldNoTypeCount[j][i] - pCountInfo.m_iMoldNoTypeCount[j][i];
			}
		}
		return tmpCount;
	}

	UINT32 GetFailCont()
	{
		UINT32 tmpCount=0;
		for (int i=0;i<MOLDNO_MAX_COUNT;i++)
		{
			for (int j=0;j<ERRORTYPE_MAX_COUNT;j++)
			{
				tmpCount += this->m_iMoldNoTypeCount[j][i];
			}
		}
		return tmpCount;
	}

	UINT32 GetMoldNoCount(int pMoldNo)
	{
		if(pMoldNo<0 || pMoldNo >= MOLDNO_MAX_COUNT )
			return 0;
		UINT32 tmpCount=0;
		for (int j=0;j<ERRORTYPE_MAX_COUNT;j++)
		{
			tmpCount += this->m_iMoldNoTypeCount[j][pMoldNo];
		}
		return tmpCount;
	}

	void clear()
	{
		m_iMoldNoAllCount = 0;
		m_iMoldNoFailCount = 0;
		for (int i=0;i<MOLDNO_MAX_COUNT;i++)
		{
			m_iMoldNoCount[i] = 0;
			for (int j=0;j<ERRORTYPE_MAX_COUNT;j++)
			{
				m_iMoldNoTypeCount[j][i] = 0;
			}
		}
	}

	//过检总数
	UINT32 m_iMoldNoAllCount;
	//踢废总数
	UINT32 m_iMoldNoFailCount;
	//每个模号踢废数
	UINT32 m_iMoldNoCount[MOLDNO_MAX_COUNT];
	//每个模号分不同缺陷的统计
	UINT32 m_iMoldNoTypeCount[ERRORTYPE_MAX_COUNT][MOLDNO_MAX_COUNT];

};

#endif // WIDGET_COUNT_H
