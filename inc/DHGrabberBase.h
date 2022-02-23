#ifndef DHGrabber_h_
#define DHGrabber_h_

#include <WTypes.h>

#pragma once

/*
�ɼ������ͱ�Ŷ�������
0:ģ��ɼ���
1:DALSA X64�ɼ���
2:AVT���
3:ǧ�������
4:��˾CG300ģ��ɼ���
5:��˾1394���
6:EURESYS �ɼ���
7:��˾USB���
8:��˾ˮ��ϵ�����
9:�������DALSA
*/
enum EGraberType
{
    EGT_Simu = 0,       //ģ��ɼ���
    EGT_Dalsa_X64,      //DALSA X64�ɼ���
    EGT_AVT,            //AVT���
    EGT_GigE,           //ǧ�������
    EGT_DH_CG300,       //��˾CG300ģ��ɼ���
    EGT_DH_1394,        //��˾1394���
    EGT_Euresys_Card,   //EURESYS �ɼ���
    EGT_DH_USB,         //��˾USB���
    EGT_DH_MER,         //��˾ˮ��ϵ�����
    EGT_LCam_DALSA,     //�������DALSA
};

const int GBMaxFileNameLen = 255;
const int GBMaxTextLen = 255;
const int MaxSNLen  = 32;

typedef struct _GBSignalInfoStruct//�ص���Ϣ�ṹ
{
    PVOID Context;                                //���ӵ�лص������Ķ����thisָ��
    int iGrabberTypeSN;                            // �ɼ������ͱ��
    int nGrabberSN;                                //�ɼ������
    int nErrorCode;                                //������루0��������
    int nFrameCount;                            //�Ѳɼ���֡����
    char strDescription[GBMaxTextLen];            //������Ϣ

    int iReserve1;                                // �����ֶ�1
    int iReserve2;                                // �����ֶ�2
    
}s_GBSIGNALINFO;//�ص���Ϣ�ṹ

//�ص�����ָ������
typedef void (WINAPI* PGBCALLBACK)(const s_GBSIGNALINFO* SignalInfo);

typedef struct _GBInitStruct//�ɼ�����ʼ���ṹ
{
    EGraberType iGrabberTypeSN;                 //�������
    int nGrabberSN;                             //�ɼ������
    char strDeviceName[GBMaxTextLen];           //�豸����
    char strDeviceMark[GBMaxTextLen];           //�豸��ʶ
    char strGrabberFile[GBMaxFileNameLen];      //���ڳ�ʼ�����ļ�
    PGBCALLBACK CallBackFunc;                   //�ص�����ָ��
    PVOID Context;                              //���ӵ�лص������Ķ����thisָ��
    
    int iReserve1;                              // �����ֶ�1
    int iReserve2;                              // �����ֶ�2

}s_GBINITSTRUCT;//�ɼ�����ʼ���ṹ

typedef struct _GBErrorInfoStruct//������Ϣ�ṹ
{
       int nErrorCode;                            //������루0��������
    char strErrorDescription[GBMaxTextLen];    //��������
    char strErrorRemark[GBMaxTextLen];        //������Ϣ

}s_GBERRORINFO;//������Ϣ�ṹ

//��������
enum GBParamID
{
    GBImageWidth = 0,           //ͼ���� (��λ:�ֽ�)
    GBImageHeight,              //ͼ��߶� (��λ:�ֽ�)
    GBImagePixelSize,           //ͼ�����ش�С (��λ:�ֽ�)
    GBImageBufferSize,          //ͼ�񻺳�����С (��λ:�ֽ�)
    GBImageBufferAddr,          //ͼ�񻺳�����ַ
    GBGrabberTypeSN,            //�ɼ������ͱ��
    GBImageBufferAddr2,         //ͼ�񻺳����ߵ�ַ(64λ��ʹ�ã�
};

//������붨��
const int GBOK = 0;

class CGrabber
{
//����
public:
    CGrabber()
    {
    }
    virtual ~CGrabber()
    {
    }
    
    //��ʼ��
    virtual BOOL Init(const s_GBINITSTRUCT* pInitParam) = 0;

    //�ر�
    virtual BOOL Close() = 0;

    //��ʼ�ɼ�
    virtual BOOL StartGrab() = 0;

    //ֹͣ�ɼ�
    virtual BOOL StopGrab() = 0;
    
    //��֡�ɼ�
    virtual BOOL Snapshot() = 0;

    //���ò���
    virtual BOOL SetParamInt(GBParamID Param, int nInputVal) = 0;
    
    //�õ�����
    virtual BOOL GetParamInt(GBParamID Param, int &nOutputVal) = 0;
    
    //���ò����Ի���
    virtual void CallParamDialog() = 0;
    
    //�õ�������Ϣ
    virtual void GetLastErrorInfo(s_GBERRORINFO *pErrorInfo) = 0;

};

#endif//DHGrabber_h_