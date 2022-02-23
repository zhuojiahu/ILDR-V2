/**************
文件名：CIOCard.h
功能: IO卡类
日期：2014/4/17
作者：njc
***************/
#ifndef CIOCARD_H
#define CIOCARD_H

#include "ConfigInfo.h"

#include "PIO24BLib.h"
#pragma comment(lib,"PIO24B_DLL.lib")

#include <QMutex>
#include <QObject>


//返回结果
typedef struct _IOCardErrorInfo
{
	bool bResult;					//结果：是否成功
	QString strSummaryInfo;			//概要信息
	QString strDetailedInfo;		//详细信息
	_IOCardErrorInfo()
	{
		bResult = true;
		strSummaryInfo = "";
		strDetailedInfo = "";
	}
}s_IOCardErrorInfo;

typedef struct _ResultSignal
{
	bool bResult;					//结果：是否成功
	short iGrabCount;				//图像号
	_ResultSignal()
	{
		bResult = true;
		iGrabCount = 0;
	}
}s_ResultSignal;

class CIOCard : public QObject
{	
	Q_OBJECT

public:
	CIOCard(s_ConfigIOCardInfo sConfigIOCardInfo,int index);
	~CIOCard();
	CPIO24BLib m_Pio24b;						//接口卡对象

private:
	bool m_bIsPopMessagebox;					//是否在主线程提示对话框
	QString strMessageboxTittle;				//弹窗标题

private:
	bool m_bIsIOCardOK;							//IO卡是否正常
private:
	QVector<s_IOSignal> m_vSignalQueue;			//信号队列
	QVector<s_ResultSignal> m_vResultSignal;	//结果队列

	s_ConfigIOCardInfo m_sConfigIOCardInfo;		//配置文件中IO卡信息
public:
	QMutex m_mutexmIOCard;						//IO卡操作互斥
	bool isIOCardOK(){return m_bIsIOCardOK;}
	void enable(bool bSign);
	s_IOCardErrorInfo InitIOCard();				//初始化IO卡
	s_IOCardErrorInfo CloseIOCard();			//释放IO卡
	int ReadImageSignal(int iStation);			//读取图像号
	int ReadCounter(int param);					//读取计数
	void SendResult(s_ResultInfo sResultInfo);
	void GetSample(s_ResultInfo sResultInfo);
	void TestOutPut(int ComNo);
	void SetOutHighLevel(int ComNo);
	void SetOutLowLevel(int ComNo);
    /*******************************************************************************************
    * @brief SetOutMode: 设置Out空输出模式，当前只支持Out0和Out1
    * @detail 
    *
    * @param: int port 0或1
    * @param: int mode 模式0:持续坏 1:持续好 2：正常
    *******************************************************************************************/
    void SetOutMode(int port, int mode);
	void writeParam(int paramIndex, unsigned short data);
	unsigned short readParam(int paramIndex);
	unsigned short readStatus(int counterIndex);
	int Show_PIO24B_DebugDialog(void *pParentWnd=NULL);
private:
	void ReadQeueu();							//读取队列
signals:
	void emitMessageBoxMainThread(s_MSGBoxInfo);	//通知主线程弹出提示对话框

};

#endif // CIOCARD_H
