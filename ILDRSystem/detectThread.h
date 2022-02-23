#ifndef DETECTTHREAD_H
#define DETECTTHREAD_H

#pragma once 

#include <QThread>
#include "stateTool.h"

class QDir;

class DetectThread : public QThread
{
	Q_OBJECT

public:
	DetectThread(QObject *parent,int);
	~DetectThread();
public:
	void run();
	void ProcessHanlde(int);
signals:
	void signals_updateActiveImg(int nCamNo,int nImgNo,double dCostTime,int tmpResult);
	void signals_AddErrorTableView(int nCamSN,int nSignalNo,int nErrorType);
	void signals_upDateCamera(int nCam,int nMode = 0);
	void signals_updateCameraFailureRate();
	void signals_updateImage(QImage*, QString, QString, QString, QString, QString, QList<QRect> ,int );
	void signals_showspeed(int);
	void signals_updateAlert(int);

private:
	void DetectNormal(CGrabElement *pElement);
	void DetectStress(CGrabElement *pElement);
	void rotateImage(CGrabElement *pElement);
	void checkImage(CGrabElement *pElement,int iCheckMode);
	bool getCheckResult(CGrabElement *pElement);
	void kickOutBad(int nSignalNo);
	void CountDefectIOCard(int nSignalNo,int tmpResult);
	void saveImage(CGrabElement *pElement);
	void addErrorImageList(CGrabElement *pElement);
	void upDateState(QImage *pElement, int signalNo,double costTime, int nMouldID, QList<QRect>, int);
	//获取检测模点的总数据
	void GetModelDotData(CGrabElement *pElement);
	//统计运行时的数据
	void CountRuningData(int cameraNumber);
private:
	bool m_bStopThread;					//结束检测
	int m_threadId;
	bool bCheckResult[CAMERA_MAX_COUNT];
	int iErrorType;
	int iMaxErrorType;
	int iMaxErrorArea;
	CSpendTime checkTimecost;
	QDir *dirSaveImagePath;
	//调用算法
	s_AlgCInP sAlgCInp;						//检测输入参数
	s_AlgCheckResult *pAlgCheckResult;		//返回检测结果结构体
	s_Status  sReturnStatus;				// 函数执行状态信息
	s_Loc_Ori tempOri;
public:
	int iCamera;
	int iImageNo;
    int iCard;
};

#endif
