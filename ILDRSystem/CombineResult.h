#ifndef COMBINERESULT_H
#define COMBINERESULT_H

#include "stateTool.h"

struct sResult {
	int iResult;						//��ǰ�����ŵ��ۺϽ��
	bool bSentToIOCard;					//�Ƿ��ѽ�������͵��ӿڿ�
	int b_Rlts[CAMERA_MAX_COUNT];		//�洢���������
	bool b_Checked[CAMERA_MAX_COUNT];	//�Ѿ�����Ľ���ĸ���
	Alg::s_ErrorPara *sErrorPara[CAMERA_MAX_COUNT];
 	sResult()
 	{	
		iResult = 0;		//��ǰ�����ŵ��ۺϽ��
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
	//���ʱ���������Ľ��Ĭ�Ϻ�,Ϊfalseʱ����ڹ�����ֻ�ǽ�����ٽ���
	bool b_CombinCamera[CAMERA_MAX_COUNT];//��Ӧ����Ƿ���Ҫ�ۺ�
	
 	int m_nPrePos;//�ϴ���ӽ���ڵ��ͼ���d
 	int m_iSysType;
    int m_iLatestImageNo;
	int m_iLatestErrorNo;
	sResult m_Rlts[256];
	int iErrorType[256];
	Alg::s_ErrorPara sErrorPara[256];//�ۺϺ��ȱ����Ϣ
	int iErrorCamera[256];		//�ۺϺ��ȱ�������ص����
	int iMoldNo[256];
public:

	CCombineRlt();
	~CCombineRlt();

    //�ۺ��豸����
	void Inital(int iGrabCountInQueen);

    void CombineRlt(int iCombineCount,int iGrabCountInQueen);
	void CombineRlt(int iCombineCount,int iGrabCountInQueen,int iRltCount);
    //�����豸�Ƿ��ۺ�
	void SetCombineCamera(int iCamera,bool bIfCombine);
    //����ĳ������Ľ��
	int AddResult(int iSignalNum,int CameraNum,int tmpResult);
    //����Ƿ�������ۺ�,����Ƿ����ۺϵĽ��
	bool ConbineResult(int iSignalNum,int CameraCount,int &bCombineRlt);
    //��������ͼ��ŵ��豸״̬
	void RemovAllResult();
    //�Ƴ�ͼ��ŵĽ��,����ͼ��ŵ������豸״̬
	void RemoveOneResult(int iSignalNum);	
    //���ָ��ͼ����µ�ָ������Ĵ�����
	void AddError(int iSignalNum,int CameraNum,Alg::s_ErrorPara tempsErrorPara);
    //��ȡָ��ͼ��ŵĴ�����Ϣ
	Alg::s_ErrorPara ConbineError(int iSignalNum);
    //������еĴ�����Ϣ
	void CCombineRlt::RemovAllError();
    //���ָ��ͼ��ŵĴ�����Ϣ
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
