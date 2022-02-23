/************************************************************************/
/* Copyright (c) 2013, �й���㼯�ű���ͼ���Ӿ������ֹ�˾�Ӿ�ϵͳ��  
/* All rights reserved.													
/*																		
/* �ļ����ƣ� DHGrabberForMER.h	
/* ժҪ�� �����ɼ����ࣨ��˾ˮ�������

/* ��ǰ�汾��1.0.0.1
/* �޸��ߣ���ͬ
/* ��Ҫ�޸����ݣ�ʵ�ֻ����ĳ�ʼ���������ɼ���ֹͣ�ɼ�����֡�ɼ��Ͳ������ý���
/* ������ڣ�2013��10��12��

/************************************************************************/

#include "DHGrabberBase.h"

////////////////////����///////////////////////
#if !defined(DHGrabberMER_h_)
#define DHGrabberMER_h_

// ˮ�����ר�в���
enum MERParamID
{
	MERSnapMode,				// ͼ��ɼ�ģʽ
	MERGain,					// ����
	MERExposure,				// �ع�ʱ��
	MERBlacklevel,			    // �ڵ�ƽ
	MERBalanceRatio,            // ��ƽ��
	MERDeviceVersion            //�汾��
};

//�ɼ���
class _declspec(dllexport) CDHGrabberMER : public CGrabber
{
	//����
public:
	CDHGrabberMER();
	virtual ~CDHGrabberMER();
	
	//��ʼ��
	BOOL Init(const s_GBINITSTRUCT* pInitParam);
	
	//�ر�
	BOOL Close();
	
	//��ʼ�ɼ�
	BOOL StartGrab();
	
	//ֹͣ�ɼ�
	BOOL StopGrab();
	
	// ��֡�ɼ� 
	BOOL Snapshot();
	
	//���ò���
	BOOL SetParamInt(GBParamID Param, int nInputVal);
	
	//�õ�����
	BOOL GetParamInt(GBParamID Param, int &nOutputVal);
	
	//���ò����Ի���
	void CallParamDialog();
	
	//�õ�������Ϣ
	void GetLastErrorInfo(s_GBERRORINFO *pErrorInfo);
	
	//////////////////////////////////////////////////////////////////////////
	// ��˾���ר�нӿ�	
	BOOL MERSetParamInt(MERParamID Param, int nInputVal1,int nInputVal2 = 0,int nInputVal3 = 0,int nInputVal4 = 0,int nInputVal5 = 0,void *sInputVal6 = NULL);
	BOOL MERGetParamInt(MERParamID Param, int &nOutputVal1,int &nOutputVal2,int &nOutputVal3);	
	BOOL MERGetParamString(MERParamID Param, char* sOutputString,int size);	
	int  MERSetOutputParamInt(int nOutputPort,int nOutputVal);//��������Զ���������� 
	//����ֵ 0:�ɹ� 1:��֧���Զ����ź� 2:�����Զ����ź�ʧ�� 3:�򿪶˿�ʧ�� 4:д���ֵʧ�� 
    //��ȡ�������
	BOOL MERGetCameraCount(int &nCameraCount);
	//����Ż�ȡ������к�,��Ŵ�0��ʼ
    BOOL MerGetCameraSN(int nCameraNumber,char sCameraSN[MaxSNLen]);
	
	void SaveParamToINI();		// �������������Ϣ
	
	//
	//////////////////////////////////////////////////////////////////////////
	
	//����
protected:
	CGrabber *m_pGrabber;
};

#endif// !defined(DHGrabberMER_h_)