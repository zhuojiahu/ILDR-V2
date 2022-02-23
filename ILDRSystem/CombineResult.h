#ifndef COMBINERESULT_H
#define COMBINERESULT_H

#include "stateTool.h"

struct sResult {
	int iResult;						//当前机器号的综合结果
	bool bSentToIOCard;						//是否已将结果发送到接口卡
	int b_Rlts[CAMERA_MAX_COUNT];		//存储结果的数组
	bool b_Checked[CAMERA_MAX_COUNT];	//已经输入的结果的个数
	Alg::s_ErrorPara *sErrorPara[CAMERA_MAX_COUNT];
 	sResult()
 	{	
		iResult = 0;		//当前机器号的综合结果
		bSentToIOCard = true;
 		for (int i = 0;i < CAMERA_MAX_COUNT; i++)
 		{
 			b_Rlts[i] = -1;
			b_Checked[i] = false;
			sErrorPara[i] = NULL;
 		}
 	}
};

class CCombineRlt
{
public:
 	QMutex m_MutexCombin;

	int i_CameraCount;
	//检测时候这个相机的结果默认好,为false时相机在工作，只是结果不再接受
	bool b_CombinCamera[CAMERA_MAX_COUNT];//对应相机是否需要综合
	
 	int m_nPrePos;//上次添加结果节点的图像号d
 	int m_iSysType;
    int m_iLatestImageNo;
	int m_iLatestErrorNo;
	sResult m_Rlts[256];
	int iErrorType[256];
	Alg::s_ErrorPara sErrorPara[256];//综合后的缺陷信息
	int iErrorCamera[256];		//综合后的缺陷最严重的相机
	int iMoldNo[256];
public:

	CCombineRlt();
	~CCombineRlt();

	void CombineRlt(int iCombineCount,int iGrabCountInQueen);
	void Inital(int iGrabCountInQueen);

	void CombineRlt(int iCombineCount,int iGrabCountInQueen,int iRltCount);
	void SetCombineCamera(int iCamera,bool bIfCombine);
	int AddResult(int iSignalNum,int CameraNum,int tmpResult);
	bool ConbineResult(int iSignalNum,int CameraCount,int &bCombineRlt);
	void RemovAllResult();
	void RemoveOneResult(int iSignalNum);	

	void AddError(int iSignalNum,int CameraNum,Alg::s_ErrorPara tempsErrorPara);
	Alg::s_ErrorPara ConbineError(int iSignalNum);
	void CCombineRlt::RemovAllError();
	void CCombineRlt::RemoveOneError(int iSignalNum);
	int ErrorType(int iImgNo);
	int ErrorCamera(int iImgNo);
	bool IsReject(int iImgNo);
	void SetReject(int iImgNo, bool bReject = true);

	void SetMoldNo(int iImgNo,int pMoldNo);
	int GetMoldNo(int iImgNo);
	void RemoveMoldNo(int iImgNo);
};

class CCheckResult
{
};

#endif
