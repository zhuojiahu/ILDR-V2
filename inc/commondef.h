#ifndef COMMON_H
#define COMMON_H
#pragma once

//���_WIN32_WINNT δ����ı��뾯��
#ifndef _WIN32_WINNT 
#define _WIN32_WINNT 0x0502
#endif

#define IMAGE_SPACE             10
#define SPPEDHIGHEST            400
#define SPPEDNORMAL             300

#include <QImage>
#include <QWaitCondition>
#include <QMutex>
#include <QMetaType>
#include <QColor>
#include <QStringList>
#include <QString>
#include "qt_windows.h"

#pragma comment(lib, "version.lib")

#pragma warning(disable:4005) //����ʾ���ض��徯��

//����������
#define CAMERA_MAX_COUNT 30
//#define ERROR_MAX_COUNT 43
//���������͸���
#define ERRORTYPE_MAX_COUNT 50
#define IOCard_MAX_COUNT 2
//����ͼ�����
#define ERROR_IMAGE_COUNT 30
//�ͻ����ģ�� ��ǰΪ90
#define MOULD_MAX_NUMBER 90
//���ģ������
#define MOLDNO_MAX_COUNT 100

//#define PASSWORD1 ""
//#define PASSWORD2 "daheng"
#define PASSWORD3 "kongping"

typedef enum{
    SQL_NEW,
    SQL_MODEFY,
    SQL_DELETE,
    SQL_RESERVE
}SQLSTATE;

// ϵͳ����
enum e_SystemType    
{
    Body = 0,           //ƿ��ϵͳ,
    FinishButtom,       //ƿ��ƿ��ϵͳ, 
    Combi14,            //һ���ϵͳ(14���)
    Combi10,            //һ���ϵͳ(10���)
    Body2Cell,          //ƿ��2���
    CombiInYaoBo,       //ҩ��һ���
    Combi21,            //ƿ����Ӧ��һ���
    VexiFinishButtom,   //����ƿ��ƿ��??
    VexiButtom          //���ϼ����
};

enum EDeviceType
{
    EDTCameraFinishBottom =1,   //ƿ��ƿ�����
    EDTCameraOvality,           //��Բ�����
    EDTCameraLineMatrix,        //�������
    EDTThickness,               //�ں��豸
};

//������־����
enum e_SaveLogType        
{
    OperationLog = 0,            //������־
    DebugLog,                    //������־(ȱͼ || �󴥷� || ������Ϊ�� ����־��Ϣ)
    AbnormityLog,               //�쳣��־(�쳣���� || �����ϲ�Ӧ���ֵ��жϴ���)

    CheckLog,                    //�����־
    MendParamLog,                //�޸Ĳ�����־
    StatInfo,                    //���ͳ����Ϣ��־
    AlgorithmDLL                //�㷨����־
};
// �ɼ�ͼ������
enum e_GrabImageType    
{
    BodyImage = 0,        // ƿ��
    BottomImage        // ƿ��
};
//����ͼ������
enum e_SaveImageType
{
    NotSave = 0,
    AllImage,
    FailureImage,
    AllImageInCount,
    FailureImageInCount,
};

//Ȩ��
typedef struct _Permission
{
    int iHome;          //������
    int iFinBot;        //ƿ��ƿ��
    int iOva;           //��Բ��
    int iThickness;     //�ں�
    int iLCamReadMold;  //���������ģ
    int iVarietyManege; //Ʒ�ֹ���
    int iSystemSet;     //ϵͳ����
    int iClear;         //��������
    int iExit;          //�˳�����
    int iStartStop;     //����ֹͣ���
    int iAlgSet;        //�㷨����
    //�ӹ���
    int iImageCarve;        //ͼ��ü�
    int iIOCard;            //IO������(����)

    int iPLCSet;            //����PLC(����ʹ��)
    _Permission()
    {
        iHome = 1;
        iFinBot = 0;
        iOva = 0;
        iThickness = 0;
        iLCamReadMold = 0;
        iClear = 0;
        iExit = 0;
        iStartStop = 0;
        iImageCarve = 0;
        iVarietyManege = 0;
        iSystemSet = 0;
        iPLCSet = 0;
        iAlgSet = 0;
    }
    void clear()
    {
        iClear = 0;
        iExit = 0;
        iStartStop = 0;
        iImageCarve = 0;
        iVarietyManege = 0;
        iSystemSet = 0;
        iPLCSet = 0;
        iAlgSet = 0;
    }
}s_Permission;

typedef struct _IOSignal 
{
    short iGrabCount;                //ͼ���
    short iGrabDelay;                //�ɼ���ʱ
    _IOSignal()
    {
        iGrabCount = 0;
        iGrabDelay = 0;
    }
}s_IOSignal;

//���ø�IO���Ľ��,��ǰֻ����ͼ��źͽ��
typedef struct _ResultInfo
{
public:
    int nIOCardNum;     //IO����
    int nCamNo;         //������
    int nImgNo;         //ͼ���
    double dCostTime;   //����ʱ��
    int tmpResult;      //���

    _ResultInfo()
    {
        nIOCardNum  = 0;
        nCamNo = 0;
        nImgNo = 0;
        dCostTime = 0;
        tmpResult = 0;
    }
}s_ResultInfo;

typedef struct _VEXIIOCardInfo{
    int nCamera1Interval;
    int nCamera2Interval;
    int nCamera3Interval;
    int nCamera4Interval;
    int nCamera5Interval;
    int nCamera6Interval;
    int nCamera7Interval;

    int nCamera1TrggerTimes;
    int nCamera2TrggerTimes;
    int nCamera3TrggerTimes;
    int nCamera4TrggerTimes;
    int nCamera5TrggerTimes;
    int nCamera6TrggerTimes;
    int nCamera7TrggerTimes;

    int nCamera1TrggerDelay;
    int nCamera2TrggerDelay;
    int nCamera3TrggerDelay;
    int nCamera4TrggerDelay;
    int nCamera5TrggerDelay;
    int nCamera6TrggerDelay;
    int nCamera7TrggerDelay;

    int nCamera1Frame;
    int nCamera2Frame;
    int nCamera3Frame;
    int nCamera4Frame;
    int nCamera5Frame;
    int nCamera6Frame;
    int nCamera7Frame;

    int nRejectDelay;
    int nRejectWidth;
    int bDoubleReject;
    int nDoubleRejectDelay;
    int nDoubleRejectWidth;

    int bCleanLightSource;
    int nCleanLightSourceInterval;
    int nCleanLightSourceWidth;

    _VEXIIOCardInfo()
    {
        nCamera1Interval = 0;
        nCamera2Interval = 0;
        nCamera3Interval = 0;
        nCamera4Interval = 0;
        nCamera5Interval = 0;
        nCamera6Interval = 0;
        nCamera7Interval = 0;

        nCamera1TrggerTimes = 0;
        nCamera2TrggerTimes = 0;
        nCamera3TrggerTimes = 0;
        nCamera4TrggerTimes = 0;
        nCamera5TrggerTimes = 0;
        nCamera6TrggerTimes = 0;
        nCamera7TrggerTimes = 0;

        nCamera1TrggerDelay = 0;
        nCamera2TrggerDelay = 0;
        nCamera3TrggerDelay = 0;
        nCamera4TrggerDelay = 0;
        nCamera5TrggerDelay = 0;
        nCamera6TrggerDelay = 0;
        nCamera7TrggerDelay = 0;

        nCamera1Frame = 0;
        nCamera2Frame = 0;
        nCamera3Frame = 0;
        nCamera4Frame = 0;
        nCamera5Frame = 0;
        nCamera6Frame = 0;
        nCamera7Frame = 0;

        nRejectDelay = 0;
        nRejectWidth = 0;
        bDoubleReject = 0;
        nDoubleRejectDelay = 0;
        nDoubleRejectWidth = 0;

        bCleanLightSource = 0;
        nCleanLightSourceInterval = 0;
        nCleanLightSourceWidth = 0;
    }
    void operator = (const _VEXIIOCardInfo s)
    {
        nCamera1Interval = s.nCamera1Interval;
        nCamera2Interval = s.nCamera2Interval;
        nCamera3Interval = s.nCamera3Interval;
        nCamera4Interval = s.nCamera4Interval;
        nCamera5Interval = s.nCamera6Interval;
        nCamera6Interval = s.nCamera6Interval;
        nCamera7Interval = s.nCamera7Interval;

        nCamera1TrggerTimes = s.nCamera1TrggerTimes;
        nCamera2TrggerTimes = s.nCamera2TrggerTimes;
        nCamera3TrggerTimes = s.nCamera3TrggerTimes;
        nCamera4TrggerTimes = s.nCamera4TrggerTimes;
        nCamera5TrggerTimes = s.nCamera5TrggerTimes;
        nCamera6TrggerTimes = s.nCamera6TrggerTimes;
        nCamera7TrggerTimes = s.nCamera7TrggerTimes;

        nCamera1TrggerDelay = s.nCamera1TrggerDelay;
        nCamera2TrggerDelay = s.nCamera2TrggerDelay;
        nCamera3TrggerDelay = s.nCamera3TrggerDelay;
        nCamera4TrggerDelay = s.nCamera4TrggerDelay;
        nCamera5TrggerDelay = s.nCamera5TrggerDelay;
        nCamera6TrggerDelay = s.nCamera6TrggerDelay;
        nCamera7TrggerDelay = s.nCamera7TrggerDelay;

        nCamera1Frame = s.nCamera1Frame;
        nCamera2Frame = s.nCamera2Frame;
        nCamera3Frame = s.nCamera3Frame;
        nCamera4Frame = s.nCamera4Frame;
        nCamera5Frame = s.nCamera5Frame;
        nCamera6Frame = s.nCamera6Frame;
        nCamera7Frame = s.nCamera7Frame;

        nRejectDelay = s.nRejectDelay;
        nRejectWidth = s.nRejectWidth;
        bDoubleReject = s.bDoubleReject;
        nDoubleRejectDelay = s.nDoubleRejectDelay;
        nDoubleRejectWidth = s.nDoubleRejectWidth;

        bCleanLightSource = s.bCleanLightSource;
        nCleanLightSourceInterval = s.nCleanLightSourceInterval;
        nCleanLightSourceWidth = s.nCleanLightSourceWidth;
    }
}s_VEXIIOCardInfo;

class SimuCamConfig
{
public:
    QString sImgPath;       //ͼ��·��
    int iWidth;             //���
    int iHeight;            //�߶�
    int iPerPxByte;         //ÿ�����ֽ�
    int iAcqSpeed;          //�ɼ��ٶ�
    int iPreRead;           //�Ƿ����Ԥ��ģʽ
    int iMultiChannel;      //�Ƿ��ͨ��
    int iCircleAcq;         //�Ƿ�ѭ���ɼ�

    SimuCamConfig()
    {
        sImgPath = "";
        iWidth = 0;
        iHeight = 0;
        iPerPxByte = 1;
        iAcqSpeed = 1000;
        iPreRead = 0;
        iMultiChannel = 0;
        iCircleAcq = 1;
    }

};
Q_DECLARE_METATYPE(SimuCamConfig)

//ˮ�������ʼ����׼�����ṹ��
class CameraConfig{
public:
    int iWidth;             //���        ʹ��
    int iHeight;            //�߶�        ʹ��
    int iOffsetX;           //Xƫ��
    int iOffsetY;           //Yƫ��
    int iExposureMode;      //�ع�ģʽ
    int iExposureTime;      //�ع�ʱ��
    int iExposureAuto;      //�Ƿ��Զ��ع�
    int iGain;              //�ź�����
    int iGainAuto;          //�ź������Ƿ��Զ�
    int iBlackLevel;        //���Ժڵ�ƽ
    int iBlackLevelAuto;    //�ڵ�ƽ�Ƿ��Զ�
    double fBalanceRatio;   //��ƽ������
    int iBalanceRatioAuto;  //�Ƿ��Զ���ƽ��
    int iTriggerMode;       //����ģʽ
    int iTriggerActivation; //������ʽ(������|�½���)
    int iAcqSpeedLevel;     //�����ٶȼ�

    CameraConfig()
    {
        iWidth = 0;
        iHeight = 0;
        iOffsetX = 0;
        iOffsetY = 0;
        iExposureMode = 0;
        iExposureTime = 1000;
        iExposureAuto = 0;
        iGain = 0;
        iGainAuto = 0;
        iBlackLevel = 0;
        iBlackLevelAuto = 0;
        fBalanceRatio = 0;
        iBalanceRatioAuto = 0;
        iTriggerMode = 1;
        iTriggerActivation = 0;
        iAcqSpeedLevel = 3;
    }

};
Q_DECLARE_METATYPE(CameraConfig)

//�ں�����
typedef struct _ThicknessConfig
{
    int SensorTotal;            //�豸����
    QString SensorName;         //�豸���� {SIM:ģ��,SIXIAN:˼��,STIL}
    int s_FilterWidth;          //��ֵ�˲��Ĵ��ڴ�С
    bool isAutoRestart;         //�Ƿ��Զ�����(����ʹ���ж�ͼ�������һ��������������̵߳Ĺ���)
    bool isDivKickCount;        //������رշֿ��߷Ϲ��� �ݲ�֧��
    bool DataAllZeroisKickout;  //ȫΪ0�Ƿ��޷�
    //for UI
    int m_Units;            //��λ��0 -> um , 1 -> mm ;

    int  s_TriggerMode;  //����ģʽ  ����ʹ��
    _ThicknessConfig()
    {
        SensorTotal=0;
        SensorName="";
        m_Units = 0;
        isAutoRestart = false;
        isDivKickCount = false;
        DataAllZeroisKickout = false;

        s_FilterWidth = 0;
    }
}ThicknessConfig;

//�ں񴫸�������
typedef struct _ThicknessParameter  //�������Ĳ���
{
    int m_CheckStation;         //���λ��:OUTN Ĭ��1�ſ� 
    int m_cardID;               //�ӿڿ�����
    QString m_ConnStr;          //���Ӳ���,˼���豸ΪIP��ַ,STIL�豸ΪSID���ֲ��ֵ�ĩβ4λ
    bool Enable;                //�Ƿ�ʹ�ܸ��豸
    int m_PresentRate;          //�ɼ�Ƶ��
    //�ں��ܲ���
    double m_CheckNess;         //ɸѡ��� С�ڴ�ֵ��ʾ��ֵ���Ϸ�,ֱ������
    bool m_bThickness;          //�Ƿ�ʹ�ܱں��޷�
    double m_nThicknessOffset;  //�߷ϲ�����������ʵ���Һ�ʵ�ʲ�һ��������
    //�ں��޷Ϲ���:С����С��ȵ�ֵ���������ڹ涨����,���ߺ񱡱ȴ����趨ֵ
    double m_MinNess;           //��С���
    int m_MinNessNo;            //��С�������
    float m_ThinkNessRate;      //�񱡱�
    //��Բ��:���ֵ-��Сֵ�Ĳ�ֵ
    bool m_bOvaEnable;          //�Ƿ�ʹ����Բ��
    bool m_bOvalization;        //�Ƿ�ʹ����Բ���޷�
    int m_iSaveOvaData;         //�Ƿ񱣴���Բ������: 0:������,1:���治�ϸ�����,2:������������
    double m_maxDistance;       //��������Ϸ�ֵ(mm)
    double m_minDistance;       //�������С�Ϸ�ֵ(mm)
    double m_OvaFactor;         //��Բ��У������
    double m_ovalizationLimit;  //��Բ�Ȳ�ֵ���� �������ֵ��ʾ���ϸ�(��λmm)

    //for UI
    int m_Allresult;            //�ɼ����� ����������ʾֵ������
    bool m_bShowCurve;          //��ʾ����
    QColor  curveColor;         //������ʾ��ɫ

    _ThicknessParameter()
    {
        m_cardID = 0;
        m_iSaveOvaData = 0;
        Enable =true;
        m_MinNess=0.0;
        m_MinNessNo=0;
        m_PresentRate=0;
        m_ThinkNessRate = 1.0;
        m_CheckNess=0.0;
        m_nThicknessOffset = 0.0;
        m_bShowCurve = true;
        m_ovalizationLimit = 0;
        m_bOvalization = false;
        m_maxDistance = 20000;
        m_minDistance = 0;
        m_OvaFactor = 1;
        m_bThickness = true;
        m_bOvaEnable = true;
    }
}ThicknessParameter;


inline QColor StringToColor(QString str)
{
    QStringList strList = str.split(",");
    int pos = strList[0].indexOf("(");
    int r = strList[0].right(pos).toInt();
    int g = strList[1].toInt();
    pos = strList[2].indexOf(")");
    int b = strList[2].left(pos).toInt();
    return QColor(r,g,b);
}

//��Ϊÿ��ƿ�ӵĵ����ռ������,QList�Ĵ�С��ʾ������������
typedef struct s_ThicknessResult  
{
    QList<int> s_bThick;            //�޷Ͻ����־ λ��־:��0λ:��С���,��1λ:�񱡱�,��2λ:��Բ�� ��ӦλΪ1��ʾ���ݲ��ϸ�,���޷�
    QList<int> s_nMinThickness;     //��С���
    QList<double> s_fThickRate;      //�����
    QList<double> s_fOvalization;   //��Բ��
    int m_imageNumber;              //ͼ���,ÿ��ƿ�ӵ�ͼ���Ӧ����һ����

    void ResultClear()
    {
        s_bThick.clear();
        s_nMinThickness.clear();
        s_fThickRate.clear();
        s_fOvalization.clear();
        m_imageNumber = -1;
    }
    void ResultReset()
    {
        for(int i = 0; i < s_bThick.size();i++)
        {
            s_bThick[i] = -1;
            s_nMinThickness[i] = -1;
            s_fThickRate[i] = -1;
            s_fOvalization[i] = -1;
        }
    }
}sThicknessResult;

//�������ļ�
#define ConfigFileSuffix        ".ini"
#define ConfigDir               "Config/"
#define ConfigFileName              "Config.ini"
#define ConfigTempFile          "Config_template.ini"
#define ErrorDefFileName        "ErrorType"
#define ModelConfigDIR          "ModelInfo/"
#define TranslationWords        "Translations"

#define ThicknessCfgFile        "ThickNessCfg.ini"
#define LCameraCfgFile          "ConfigLCamera.ini"
#define LCamReadMouldFile       "LCamReadMould.ini"
#define OvalityFile             "Ovality.ini"

#define DATAFILE                "Data.ini"

//ͳ������ļ���·��
#define CountDir                "CountInfo/"
#define AllCountDir             "AllCount/"
#define CamCountDir             "CameraCount/"
#define HalfCountDir            "HalfCount/"
#define HourCountDir            "HourCount/"
#define ThicknessCountDir       "Thickness/"

#define CountByTimeDir          "CountInfoByTime/"

#define LastDataFile            "LastData.ini"
#define CountFileSuffix         ".txt"

//#define  NO_DALSA_CAM
//#define NO_MOULD_COUNT

#endif // COMMON_H