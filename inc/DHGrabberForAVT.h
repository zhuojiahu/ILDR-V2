/************************************************************************/
/* Copyright (c) 2012, �������ͼ���Ӿ����޹�˾  
/* All rights reserved.													
/*																		
/* �ļ����ƣ� DHGrabberForAVT.h	
/* ժҪ�� ���AVT���FGCamera�汾(3.00) ���и��߰汾����Ҫ����
/*
/* ��ǰ�汾�� 6.0.0.1Beta1
/* �޸��ߣ� ���ǿ
/* �޸����ݣ��������
/* ������ڣ� 2012-07-31
/************************************************************************/
#include "DHGrabberBase.h"

#if !defined(DHGrabberAVT_h_)
#define DHGrabberAVT_h_

//////////////////////////////////////////////////////////////////////////
//�ɼ���

enum AVTParamID	//AVT���ר�в���
{
	AVTColor,               //�������  �ڰ׺Ͳ�ɫ 
	AVTBrightness,			//������ȴ�С
	AVTShutter,				//�������ֵ
	AVTGain,				//��������С
	AVTU,					//���Uֵ
	AVTV,					//���Vֵ
	AVTTriggerOnOff,		//����ⴥ���Ƿ���
	AVTTemperature			//����¶�
};


// AVT���ר�нӿ�
enum AVTParamVal
{
	AVTTriggerOff = 0,		//�ⴥ���ر�//�ڲ�����,�����ɼ�
	AVTTriggerOn,	    	//�����ش���//�ⴥ������
	AVTTriggerMode2,		//�½��ش���
	AVTTriggerMode3,		//�ߵ�ƽ����
	AVTTriggerMode4,		//�͵�ƽ����
	AVTTriggerMode5,		//���ģʽ�����ش���
	AVTTriggerMode6,		//���ģʽ�½��ش���	
};

class _declspec(dllexport) CDHGrabberAVT : public CGrabber
{
	//����
public:
	CDHGrabberAVT();
	virtual ~CDHGrabberAVT();

	//��ʼ��
	BOOL Init(const s_GBINITSTRUCT* pInitParam);

	//�ر�
	BOOL Close();

	//��ʼ�ɼ�
	BOOL StartGrab();

	//ֹͣ�ɼ�
	BOOL StopGrab();

	//����ɼ�
	BOOL Snapshot();

	//���ò���
	BOOL SetParamInt(GBParamID Param, int nInputVal);

	//�õ�����
	BOOL GetParamInt(GBParamID Param, int &nOutputVal);

	//���ò����Ի���
	void CallParamDialog();

	//�õ�������Ϣ
	void GetLastErrorInfo(s_GBERRORINFO *pErrorInfo);

	BOOL AVTSetParamPro(AVTParamID Param, int ParamValInput);

	BOOL AVTSetTriggerParam(AVTParamVal ParamValInput);

	//////////////////////////////////////////////////////////////////////////
	//����
protected:
	CGrabber *m_pGrabber;
};

#endif// !defined(DHGrabberAVT_h_)

//////////////////////////////////////////////////////////////////////////
//ʹ��˵��:   Ŀǰ�ɼ������AVT����������汾Ϊ2.10  
//����������ͣ�Guppy��Stingray��Marlinϵ��
//1. ��ԭ����ini������������ļ�ֱ�Ӹ���Ϊ���Ĭ�ϵĲ��������ļ�XML
//2. ����������޸����£�
//   �豸A���=0
//   �豸A����=6735372523180860609 (�����Լ�����һ������)
//   �豸A��ʶ=6735372523180860609
//   �豸A��ʼ���ļ���=GuppyConfig.xml   //����AVT ��SmartView�����xml�ļ����Ƹ���������
//3. ���⽫�����XML�ļ�������Config�ļ�����(��ԭ����GuppyConfig.iniͬ��Ŀ¼��)
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//�޸ļ�¼
/*
2010.12.7  V4.0.0.3  �ɭ**Ŀ�ģ��޸Ķ�������дXML�ļ��ļ�***
�޸����ݣ�
1. ���������ʱ���޸Ĳ������浽XML�ļ������⣻
2. ����������кţ�����ֻ��д��ǰ���15λ(6+9),����������޸����£�
�豸A���=0
�豸A����=AVT
�豸A��ʶ=673537252318086
�豸A��ʼ���ļ���=GuppyConfig.xml   //����AVT ��SmartView�����xml�ļ����Ƹ���������
���⻹���ݺ������0609�汾
3. �޸Ľӿ�AVTGetCamGuid���ƣ���ԭ����AVTGetCamGaid�޸ĳ�AVTGetCamGuid
4. �����˳�Ĭ�ϱ���XML�ļ�

2010.5.24  V4.0.0.1Beta1  **Ŀ�ģ���ԭ�е�ini�����ļ�����ֱ��ʹ����������xml�ļ�***
�޸����ݣ�
1. ��ԭ����ini������������ļ�ֱ�Ӹ���Ϊ���Ĭ�ϵĲ��������ļ�XML��
2. ����������޸����£�
�豸A���=0
�豸A����=6735372523180860609
�豸A��ʶ=6735372523180860609
�豸A��ʼ���ļ���=GuppyConfig.xml   //����AVT ��SmartView�����xml�ļ����Ƹ���������
3. ���⽫�����XML�ļ�������Config�ļ�����(��ԭ����GuppyConfig.iniͬ��Ŀ¼��)

2010.5.26  V4.0.0.1Beta2  **Ŀ�ģ���ֹͣ�ɼ�ʱ����������������Ի����ϣ����޸ĵ������������ΪXML**
�޸����ݣ�
1. ��������������б���ΪXML��Beta2
2. ��ӻ�ȡ�������(�ڰ׺Ͳ�ɫ)���ܣ�ͨ��AVTColor������ã�

2010.6.21  V4.0.0.1Beta3  **Ŀ�ģ������ڼ�������(������ڲɼ�ʱ)���������������������һ��ʹ��**
�޸����ݣ�
1. ����˳�ʱ�ٱ���һ��XML��

2010.6.23  V4.0.0.1Beta4  
�޸����ݣ�
1. ������AVT����ɼ������⣻
*/
//////////////////////////////////////////////////////////////////////////