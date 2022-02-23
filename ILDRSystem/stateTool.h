#ifndef STATETOOL_H
#define STATETOOL_H
#pragma once

#include "common.h"
#include "setDebugNew.h"

//相机支持相关相机类型
#include "DHGrabberBase.h"
#include "DHGrabberForSG.h"
#include "DHGrabberForMER.h"
#include "DHGrabberForAVT.h"
//算法库
#include "RotEmptyBottle.h"

#include "alg_struct.h"

using namespace Alg;  // 使用命名空间 [2009-9-15-yangfs]

class StateTool
{

public:
	static bool WritePrivateProfileQString(QString strSectionName, QString strKeyName, QString strValue, QString strFileName);
	static bool writeInit(QString path, QString user_key, QString user_value);
	static bool readInit(QString path, QString user_key, QString &user_value);
	static bool updateText(QString text, int max_width, QString &elided_text);
	static QString getSkinName();

};

//错误信息类
class cErrorTypeInfo
{
public:
	int m_iErrorTypeCount;//总错误类型个数
	int iErrorCountByType[ERRORTYPE_MAX_COUNT];

	void operator = (const cErrorTypeInfo cError);
public:
	cErrorTypeInfo();
	~cErrorTypeInfo();
	void Clear();
	BOOL ErrorTypeJudge(int &iErrorType);
};

// 机器信号 ：： 其中仅适用接口卡图像信号数m_iImageCount
class CMachineSignal  //根据具体现场的需求来定义机器信号内容
{
public:	
	int m_iSensorCount;	    // 传感器，帧计数 即到位信号计数 最大
	int m_iImageCount;		// 接口卡图像信号计数
	int m_iKickCount;       // 踢废计数
public:

	CMachineSignal();

	virtual ~CMachineSignal();

	CMachineSignal(const CMachineSignal &MachineSignal);

	const CMachineSignal& operator=(const CMachineSignal &MachineSignal);
};

//采集图像信息
class CGrabElement
{ 
public:
	CGrabElement();
	~CGrabElement();
public:	
	QImage*				myImage;	        // 具体的图像信息  宽度、高度、数据区等
	s_AlgImageLocInfo   sImgLocInfo;		// 定位信息
	QList<s_ErrorPara>  cErrorParaList;		// 算法返回的错误信息链表
	QList<QRect>		cErrorRectList;		// 算法返回的错误矩形框链表
	int					nCamSN;				// 相机序号
	double				dCostTime;			// 检测耗时	
	BOOL				bHaveImage;			// 是否有图像
	int					nCheckRet;			// 检测结果 对应错误列表
	int                 nSignalNo;			// 接口卡信号个数 255
	int					initID;
	int					nMouldID;			// 瓶底模点
	QImage*				SourceImage;
	int					nWidth;
	int					nHeight;
};

class CDetectElement
{ 
public:
	CDetectElement();
	~CDetectElement();
	void AddNormalImage(CGrabElement*	sImageNormal);
	void AddStressImage(CGrabElement*	sImageStress);
	bool IsImageNormalCompelet();
	bool IsImageStressCompelet();
	void setType(int type);
	int SignalNoNormal();	
	int SignalNoStress();
	void copyDatato(CDetectElement &cDes);
	void clear();
public:	
	CGrabElement*	ImageNormal;	    // 图像
	CGrabElement*   ImageStress;		// 应力图像
	int				iType;				// 0:保存正常图像  1：保存应力图像
	int				iCameraNormal;
	int				iCameraStress;
	int				iSignalNoNormal;
	int				iSignalNoStress;
	bool			bIsImageNormalCompelet;
	bool			bIsImageStressCompelet;
};

//互斥伪实现消息等待
class GrabberEvent
{
public:
	GrabberEvent();

	void postMessage(int iCamera);
	int waitMessage(unsigned long time = ULONG_MAX);
private:
	QWaitCondition waitConditionM;
	QMutex mutexM;
	int iTimpCheck[CAMERA_MAX_COUNT];
};
//互斥伪实现消息等待
class IOCardEvent
{
public:
	IOCardEvent()
	{
		int bRtn = 0;
	}

	void postMessage()
	{
		mutexM.lock();
//		iTimpCheck[iCamera]++;
		waitConditionM.wakeOne();
		mutexM.unlock();
	}
	bool waitMessage(unsigned long time = ULONG_MAX)
	{
		int bRtn = 0;
		mutexM.lock();
		bRtn = waitConditionM.wait(&mutexM,time);
		mutexM.unlock();
		return bRtn;
	}
private:
	QWaitCondition waitConditionM;
	QMutex mutexM;
//	int iTimpCheck[CAMERA_MAX_COUNT];
};


//计时器类
class CSpendTime
{
public:
	LARGE_INTEGER litmp;
	LONGLONG Qpart1,Qpart2,Useingtime,Useingtime_us;
	double dfMinus,dfFreq,dfTime;

	virtual ~CSpendTime();
	CSpendTime();
	void StartSpeedTest();
	void StopSpeedTest();
};
//对话框信息-提示对话框只能在主线程弹出，将弹出信息传递给主线程
typedef struct _MSGBoxInfo
{
	QString strMsgtitle;
	QString strMsgInfo;
}s_MSGBoxInfo;

#endif