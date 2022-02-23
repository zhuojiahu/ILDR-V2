#ifndef COMMON_H
#define COMMON_H
#pragma once

//解除_WIN32_WINNT 未定义的编译警告
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

#pragma warning(disable:4005) //不显示宏重定义警告

//最大相机个数
#define CAMERA_MAX_COUNT 30
//#define ERROR_MAX_COUNT 43
//最多错误类型个数
#define ERRORTYPE_MAX_COUNT 50
#define IOCard_MAX_COUNT 2
//错误图像个数
#define ERROR_IMAGE_COUNT 30
//客户最大模号 当前为90
#define MOULD_MAX_NUMBER 90
//最大模号数量
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

// 系统类型
enum e_SystemType    
{
    Body = 0,           //瓶身系统,
    FinishButtom,       //瓶口瓶底系统, 
    Combi14,            //一体机系统(14相机)
    Combi10,            //一体机系统(10相机)
    Body2Cell,          //瓶身2光电
    CombiInYaoBo,       //药玻一体机
    Combi21,            //瓶身单独应力一体机
    VexiFinishButtom,   //联合瓶口瓶底??
    VexiButtom          //联合检验机
};

enum EDeviceType
{
    EDTCameraFinishBottom =1,   //瓶口瓶底相机
    EDTCameraOvality,           //椭圆度相机
    EDTCameraLineMatrix,        //线阵相机
    EDTThickness,               //壁厚设备
};

//保存日志类型
enum e_SaveLogType        
{
    OperationLog = 0,            //操作日志
    DebugLog,                    //调试日志(缺图 || 误触发 || 缓存区为空 等日志信息)
    AbnormityLog,               //异常日志(异常捕获 || 理论上不应出现的判断错误)

    CheckLog,                    //检测日志
    MendParamLog,                //修改参数日志
    StatInfo,                    //检测统计信息日志
    AlgorithmDLL                //算法库日志
};
// 采集图像类型
enum e_GrabImageType    
{
    BodyImage = 0,        // 瓶身
    BottomImage        // 瓶底
};
//保存图像类型
enum e_SaveImageType
{
    NotSave = 0,
    AllImage,
    FailureImage,
    AllImageInCount,
    FailureImageInCount,
};

//权限
typedef struct _Permission
{
    int iHome;          //主界面
    int iFinBot;        //瓶口瓶底
    int iOva;           //椭圆度
    int iThickness;     //壁厚
    int iLCamReadMold;  //线阵相机读模
    int iVarietyManege; //品种管理
    int iSystemSet;     //系统设置
    int iClear;         //计数清零
    int iExit;          //退出程序
    int iStartStop;     //启动停止检测
    int iAlgSet;        //算法设置
    //子功能
    int iImageCarve;        //图像裁剪
    int iIOCard;            //IO卡操作(备用)

    int iPLCSet;            //设置PLC(不再使用)
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
    short iGrabCount;                //图像号
    short iGrabDelay;                //采集延时
    _IOSignal()
    {
        iGrabCount = 0;
        iGrabDelay = 0;
    }
}s_IOSignal;

//设置给IO卡的结果,当前只用了图像号和结果
typedef struct _ResultInfo
{
public:
    int nIOCardNum;     //IO卡号
    int nCamNo;         //相机编号
    int nImgNo;         //图像号
    double dCostTime;   //花费时间
    int tmpResult;      //结果

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
    QString sImgPath;       //图像路径
    int iWidth;             //宽度
    int iHeight;            //高度
    int iPerPxByte;         //每像素字节
    int iAcqSpeed;          //采集速度
    int iPreRead;           //是否采用预读模式
    int iMultiChannel;      //是否分通道
    int iCircleAcq;         //是否循环采集

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

//水星相机初始化标准参数结构体
class CameraConfig{
public:
    int iWidth;             //宽度        使用
    int iHeight;            //高度        使用
    int iOffsetX;           //X偏移
    int iOffsetY;           //Y偏移
    int iExposureMode;      //曝光模式
    int iExposureTime;      //曝光时间
    int iExposureAuto;      //是否自动曝光
    int iGain;              //信号增益
    int iGainAuto;          //信号增益是否自动
    int iBlackLevel;        //绝对黑电平
    int iBlackLevelAuto;    //黑电平是否自动
    double fBalanceRatio;   //白平衡因子
    int iBalanceRatioAuto;  //是否自动白平衡
    int iTriggerMode;       //触发模式
    int iTriggerActivation; //触发方式(上升沿|下降沿)
    int iAcqSpeedLevel;     //捕获速度级

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

//壁厚配置
typedef struct _ThicknessConfig
{
    int SensorTotal;            //设备个数
    QString SensorName;         //设备类型 {SIM:模拟,SIXIAN:思显,STIL}
    int s_FilterWidth;          //中值滤波的窗口大小
    bool isAutoRestart;         //是否自动重启(用于使能判定图像号两次一样的情况下重启线程的功能)
    bool isDivKickCount;        //开启或关闭分开踢废功能 暂不支持
    bool DataAllZeroisKickout;  //全为0是否剔废
    //for UI
    int m_Units;            //单位：0 -> um , 1 -> mm ;

    int  s_TriggerMode;  //触发模式  不再使用
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

//壁厚传感器参数
typedef struct _ThicknessParameter  //传感器的参数
{
    int m_CheckStation;         //检测位置:OUTN 默认1张卡 
    int m_cardID;               //接口卡索引
    QString m_ConnStr;          //连接参数,思显设备为IP地址,STIL设备为SID数字部分的末尾4位
    bool Enable;                //是否使能该设备
    int m_PresentRate;          //采集频率
    //壁厚功能参数
    double m_CheckNess;         //筛选厚度 小于此值表示不值不合法,直接抛弃
    bool m_bThickness;          //是否使能壁厚剔废
    double m_nThicknessOffset;  //踢废补偿量，用于实验室和实际不一样的数据
    //壁厚剔废规则:小于最小厚度的值的数量大于规定数量,或者厚薄比大于设定值
    double m_MinNess;           //最小厚度
    int m_MinNessNo;            //最小厚度数量
    float m_ThinkNessRate;      //厚薄比
    //椭圆度:最大值-最小值的差值
    bool m_bOvaEnable;          //是否使能椭圆度
    bool m_bOvalization;        //是否使能椭圆度剔废
    int m_iSaveOvaData;         //是否保存椭圆度数据: 0:不保存,1:保存不合格数据,2:保存所有数据
    double m_maxDistance;       //距离的最大合法值(mm)
    double m_minDistance;       //距离的最小合法值(mm)
    double m_OvaFactor;         //椭圆度校正因子
    double m_ovalizationLimit;  //椭圆度差值限制 超过这个值表示不合格(单位mm)

    //for UI
    int m_Allresult;            //采集总数 界面曲线显示值的数量
    bool m_bShowCurve;          //显示曲线
    QColor  curveColor;         //曲线显示颜色

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

//此为每个瓶子的的最终检测数据,QList的大小表示传感器的数量
typedef struct s_ThicknessResult  
{
    QList<int> s_bThick;            //剔废结果标志 位标志:第0位:最小厚度,第1位:厚薄比,第2位:椭圆度 对应位为1表示数据不合格,需剔废
    QList<int> s_nMinThickness;     //最小厚度
    QList<double> s_fThickRate;      //薄厚比
    QList<double> s_fOvalization;   //椭圆度
    int m_imageNumber;              //图像号,每个瓶子的图像号应该是一样的

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

//总配置文件
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

//统计相关文件及路径
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