#ifndef CONFIGINFO_H
#define CONFIGINFO_H

#pragma once
//#include <QList>
#include <QTime>
#include <QSet>
#include <QStringList>

#include "common.h"
#include "stateTool.h"

//IO卡信息
typedef struct _ConfigIOCardInfo
{
	short	iCardID;						//设备ID
	QString	strCardName;					//设备名称
	QString	strCardInitFile;				//设备初始化文件

	_ConfigIOCardInfo()
	{
		iCardID = 0;
		strCardName = "PIO24B";
		strCardInitFile = "PIO24B_reg_init.txt";
	}
}s_ConfigIOCardInfo;
//踢废信息
typedef struct _StatisticsInfo
{
	int totalNum;				
	int	passNum;				
	int failureNum;				

	_StatisticsInfo()
	{
		totalNum = 0;						
		passNum = 0;					
		failureNum = 0;		
	}
}s_StatisticsInfo;

typedef struct _SampleInfo
{
	int m_iSampleCount;
	int	m_iSampleTypeCount;					//错误类型个数
	BOOL m_bSampleType[ERRORTYPE_MAX_COUNT]; //图像类型，true: 表示保存，false: 表示不保存
	bool bContinuousSampling;

 //	QMutex m_mutexSample;

	_SampleInfo()
	{
		m_iSampleCount = 0;
		m_iSampleTypeCount = 0;
		bContinuousSampling = false;
		for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
		{
			m_bSampleType[i] = false;
		}
	}
}s_SampleInfo;

typedef struct _ErrorInfo
{
	BOOL m_bSaveStatInfo;					//是否保存错误信息
	int	m_iErrorTypeCount;					//错误类型个数 从配置文件中读出 [11/26/2010 GZ]
	cErrorTypeInfo m_cErrorReject;			//错误信息标准 从配置文件中读出 [11/26/2010 GZ]
	BOOL m_bErrorType[ERRORTYPE_MAX_COUNT]; //图像类型，true: 表示保存，false: 表示不保存
	int m_bErrorCountByType[ERRORTYPE_MAX_COUNT]; //图像类型，true: 表示保存，false: 表示不保存
	QVector<QString> m_vstrErrorType;		//错误类型
	int i_countErrorType[CAMERA_MAX_COUNT][ERRORTYPE_MAX_COUNT];

}s_ErrorInfo;

//图像剪切参数 ：： 此处定义，因为可能是一个相机被切割成N张图；
//一个结构体对应一个相机，而结构体中每个参数对应被切割后的图像序列
typedef struct _CorveToCamera {
	int i_ImageCount;				//相机图像被切割成小图像的个数
	int i_GrabSN[CAMERA_MAX_COUNT]; //图像对应的相机编号
	int i_ImageX[CAMERA_MAX_COUNT];
	int i_ImageY[CAMERA_MAX_COUNT];
	int i_ImageWidth[CAMERA_MAX_COUNT];
	int i_ImageHeight[CAMERA_MAX_COUNT];
} sCorveToCamera;

typedef struct _realCamInfo
{
	BOOL m_bSmuGrabber;			//是否是模拟采集
	BOOL m_bCameraInitSuccess;	//相机初始化是否成功
    int m_iDevType;             //相机用途 1:前后壁, 2:夹持
	int m_iGrabType;			//相机类型 0:模拟相机 1:AVT.2:BSL相机 3.Baumer 8:MER 
	int m_iImageType;			//采集图像类型0：瓶身。1：瓶口瓶底。图像综合用
    int m_iIOCardSN;			//对应踢废接口卡
    int m_iImageTargetNo;        //同一图像号的目标拍照个数
//	int m_iGrabYUVtoRGB;		//是否需要图像格式转换
	int m_iGrabPosition;		//相机采集工位
	int m_iImageAngle;			//图像旋转角度 : 切割中的小角度旋转
	int m_iImageRoAngle;		//图像旋转角度 : 90,270-配置文件，方向旋转

	int m_iShuter;
	int	m_iTrigger;

	int m_iImageWidth;
	int m_iImageHeight;
	int m_iImageSize;
	int m_iImageBitCount;

	sCorveToCamera m_sCorves;

	CGrabber* m_pGrabber;		//采集卡基类指针
	PBYTE m_pGrabYUVtoRGB;		//格式转化时的临时缓存[4/26/2011 lly]

//	QList<int> m_listCarvedCamera;

	QImage *m_pRealImage;		//实际显示图像（切割前的图像）
	
	BOOL m_bGrabIsTrigger;			//是否开启外部触发模式
	BOOL m_bGrabIsStart;			//相机是否开始
	int m_iGrabImageCount;			//相机拍照张数[3/14/2011 lly]::njc拍照总数，包括误触发
	int m_iGrabTriggerSignalCount;	//相机触发信号个数[3/22/2011 lly]::njc触发拍照总数，不包括误触发
	int m_iLastTriggerSignalCount;	//相机上一次触发时，信号的个数[3/22/2011 lly]
	int m_iImageIdxLast;			// 图像号，用于判断误触发 [12/19/2010 zhaodt]
	QMutex m_mutexmShownImage;					
	QMutex m_mutexmRealImage;					

	QString m_strErrorInfo;

	_realCamInfo()
	{
		m_bSmuGrabber = false;
		m_bCameraInitSuccess = false;
        m_iDevType = 0;
		m_iGrabType = 0;
		m_iImageType = 0;
		m_iIOCardSN = 0;
		m_iGrabPosition = 0;
		m_iImageRoAngle = 0;
		m_iImageWidth = 500;
		m_iImageHeight = 500;
		m_iImageSize = 250000;
		m_iImageBitCount = 8;
		m_iGrabImageCount = 0;
        m_iImageTargetNo = 8;
		m_iGrabTriggerSignalCount = 0;
		m_iLastTriggerSignalCount = 0;
		m_iImageIdxLast = 0;
		m_bGrabIsTrigger = TRUE;
		m_bGrabIsStart = FALSE;
		m_pGrabYUVtoRGB = NULL;
		m_pGrabber = NULL;
		m_pRealImage = NULL;
	}
}s_RealCamInfo;
//	s_AlgImageLocInfo m_AlgImageLocInfos[256];//定位信息
typedef struct _ImageLocInfo
{
	Alg::s_AlgImageLocInfo m_AlgImageLocInfos;	//定位信息
	int m_iHaveInfo;
	_ImageLocInfo()
	{
		m_iHaveInfo = 0;
	}
}s_ImageLocInfo;

typedef struct _carvedCamInfo
{
	int m_iToRealCamera;		//对应真实相机编号
	int m_iResImageWidth;
	int m_iResImageHeight;
	int m_iImageType;			//采集图像类型0：瓶身。1：瓶口瓶底。图像综合用
	int m_iIOCardSN;			//对应踢废接口卡
	int m_iErrorCount;
	int m_iGrabPosition;
	
	int m_iStress;				//0：正常图像 1：条纹图像
	int m_iToStressCamera;
	int m_iToNormalCamera;
	QList<CGrabElement *> listWaiteLoc;
	s_ImageLocInfo sImageLocInfo[256];		//定位信息

	int m_iImageAngle;			//图像旋转角度 : 切割中的小角度旋转
	int m_iImageRoAngle;		//图像旋转角度 : 90,270-配置文件，方向旋转

	int i_ImageX;
	int i_ImageY;
	int m_iImageWidth;
	int m_iImageHeight;
	int m_iImageSize;
	int m_iImageBitCount;

	PBYTE m_pGrabTemp;			//图像剪切时的临时缓存
	QImage *m_pActiveImage;		//实际显示图像（切割后的图像）

	int m_iShuter;
	int	m_iTrigger;

	QMutex m_mutexmShownImage;					//
	bool m_bResetImagSize;

	_carvedCamInfo()
	{
		m_iToRealCamera = -1;

		m_iResImageWidth = 500;
		m_iResImageHeight = 500;
		m_iImageType = 0;
		m_iIOCardSN = 0;
		m_iErrorCount = 0;
		m_iGrabPosition = 0;

		m_iStress = 0;
		m_iToStressCamera = -1;
		m_iToNormalCamera = -1;
		qDeleteAll(listWaiteLoc);
		listWaiteLoc.clear();

		m_iImageAngle = 0;
		m_iImageRoAngle = 0;

		m_iImageWidth = 500;
		m_iImageHeight = 500;
		m_iImageSize = 250000;
		m_iImageBitCount = 8;

		m_pGrabTemp = NULL;
		m_pActiveImage = NULL;
		m_bResetImagSize = false;

		i_ImageX = 0;
		i_ImageY = 0;

		m_iShuter = 20;
		m_iTrigger = 1;

	}
}s_CarvedCamInfo;

typedef struct _SystemInfo
{
	QMutex m_mutexSystemInfo;
	int m_iTest;
	bool m_bDebugMode;	
	short iRealCamCount;		//真实相机个数
	short iCamCount;			//切割后相机个数
	bool IsCarve;
	BOOL m_bIsIOCardOK;			//是否可用接口卡
	BOOL m_bIsStopNeedPermission;	//停止检测是否需要权限
	
	BOOL m_bIsTest;				//是否测试模式 （未使用）
//	BOOL m_bUseHighClock;		//是否使用高精度时间
	
    int iIOCardCount;			//接口卡个数
    int iIOCardID;              //一体机在一台机器上运行两个程序时,用于初始化自己接口卡使用的接口卡ID配置

	int iIsButtomStress;		//是否有瓶底应力
	int iIsSample;				//是否有取样功能
	int iIsSaveCountInfoByTime;	//是否保存指定时间段内的统计信息
	int iIsCameraCount;			//是否保存各个相机的统计信息
	QString LastModelName;		//上次使用模板
	QString m_strModelName;		//模板名
	int m_iSaveNormalErrorImageByTime;
	int m_iSaveStressErrorImageByTime;
	bool m_bSaveCamera[CAMERA_MAX_COUNT];
	bool m_bSaveErrorType[ERRORTYPE_MAX_COUNT];

	bool m_iStopOnConveyorStoped;	//输送带停止是否停止检测

	int nLoginHoldTime;	//0:不统计，1：按时间统计，2：按个数统计	
	int m_iIsTrackStatistics;	//0:不统计，1：按时间统计，2：按个数统计	
	int m_iTrackTime;
	int m_iTrackNumber;
	int m_iTrackAlertRateMax[ERRORTYPE_MAX_COUNT];//0为总，其他对应缺陷类型
	int m_iTrackAlertRateMin[ERRORTYPE_MAX_COUNT];//0为总，其他对应缺陷类型
//	int m_iIsTrackErrorType;	
 	int m_iIsTrackErrorType[ERRORTYPE_MAX_COUNT];	
	int m_NoKickIfNoFind;
	int m_NoKickIfROIFail;

	int m_iImageStretch;		//是否缩放图像
	int m_iNoRejectIfNoOrigin[CAMERA_MAX_COUNT];			//找不到原点是否踢废	
	int m_iNoRejectIfROIfail[CAMERA_MAX_COUNT];			//找不到原点是否踢废	
	int m_iNoStaticIfNoOrigin[CAMERA_MAX_COUNT];			//找不到原点是否统计

	int m_iGrabCountInQueen;	//同时触发相机个数  未使用	
	QString m_strWindowTitle;	//软件名

	int IOCardiCamCount[IOCard_MAX_COUNT];
	//相机不同尺寸时的最大值，用于初始化算法
	int m_iMaxCameraImageWidth;
	int m_iMaxCameraImageHeight;
	int m_iMaxCameraImageSize;
	int m_iMaxCameraImagePixelSize;
	s_ConfigIOCardInfo m_sConfigIOCardInfo[IOCard_MAX_COUNT];

	BOOL m_bLoadModel;// 是否导入模板成功 [8/4/2010 GZ]

	bool bSaveRecord;
	int iSaveRecordInterval;
	bool bAutoSetZero;
	QTime shift1Time;
	QTime shift2Time;
	QTime shift3Time;

	double fPressScale;			//瓶身应力增强系数
	double fBasePressScale;		//瓶底应力增强系数

	bool bCameraOffLineSurveillance;
	bool bCameraContinueRejectSurveillance;

	int iCamOfflineNo;
	int iCamContinueRejectNumber;
	int iIOCardOffSet;
	int iReadMouldIDCamNo ;  //读取模号的相机号 默认是8号相机
	int iMaxReadMouldIDCount; //最大读模数

    bool bKickReadFailed;   //是否剔废识别失败的瓶子
    bool bKickMarked;       //是否剔废标记项
    QSet<QString> markedID;     //标记剔废的ID列表
    QSet<QString> markedCavity; //标记剔废的瓶子来源产线

    QStringList idlist;         //设备生产的ID列表
    QStringList cavityList;     //设备过瓶的来源产线列表

	QString i_IP1;
	QString i_IP2;
    
	_SystemInfo()
    {
		m_iTest =0;
		iRealCamCount = 1;
		iCamCount = 1;
		m_bIsIOCardOK = FALSE;
		m_iGrabCountInQueen = 1;
		m_bIsTest = FALSE;
//		m_bUseHighClock = TRUE;
        iIOCardCount = 1;
        iIOCardID = 0;
//		m_bIfNoFind = FALSE;
        for(int i = 0;  i < IOCard_MAX_COUNT;i++)
        {
            IOCardiCamCount[i] = 0;
        }
		m_NoKickIfNoFind = 0;
		m_NoKickIfROIFail = 0;
		m_iStopOnConveyorStoped = false;

		m_iImageStretch = 1;

		m_bLoadModel=FALSE;// 是否导入模板成功 [8/4/2010 GZ]

		bSaveRecord = true;
		iSaveRecordInterval = 60;

		bAutoSetZero = true;
		shift1Time.setHMS(0,0,0);
		shift2Time.setHMS(0,0,0);
		shift3Time.setHMS(0,0,0);

		fPressScale = 1;
		fBasePressScale = 1;

		bCameraOffLineSurveillance = false;
		bCameraContinueRejectSurveillance = false;

		iCamOfflineNo = 1;
		iCamContinueRejectNumber = 15;
		iIOCardOffSet = 0;
		iReadMouldIDCamNo =-1;

		i_IP1 = "192.168.250.200";
		i_IP2 = "192.168.250.201";
		for (int i=0; i<CAMERA_MAX_COUNT;i++)
		{
			m_iNoRejectIfNoOrigin[i] = 0;
			m_iNoRejectIfROIfail[i] = 0;
			m_iNoStaticIfNoOrigin[i] = 0;
		}

        bKickReadFailed = false;
	}
}s_SystemInfo;

typedef struct _RunningInfo
{
	QMutex m_mutexRunningInfo;
	QMutex m_mutexTestInfo;
	int m_iLastIOCard1IN0;
	int m_iLastIOCard2IN0;
	int m_iPermission;//权限
    //软件归一统计 以第二张卡为准(设置)
    int refCardSN;      //参考卡
    int m_checkedNum;   //过检总数
    int m_failureNum;   //不合格数
	int m_passNum;      //本次检测过瓶数
    int m_kickoutNumber;//本次剔废数

	bool m_bCheck;//是否正在检测，0停止检测，1：正在检测。
    int iTestCamera;
	bool m_bIsCheck[CAMERA_MAX_COUNT];//此相机是否正在检测

	int nModelReadFailureNumber;//nMouldID = -1数量，未设置为0
	//int nModelChenkedNumber[100];//nMouldID为0-99的数量统计
	int nModelCheckedCount;	// nMouldID总数
	
	QString strSpeed;

	//计数统计
	int m_iErrorCamCount[CAMERA_MAX_COUNT];// 每个相机的错误计数
	double m_iErrorCamRate[CAMERA_MAX_COUNT];// 每个相机的错误率
	int m_iErrorTypeCount[ERRORTYPE_MAX_COUNT];//每个错误类型的错误计数
    cErrorTypeInfo m_cErrorTypeInfo[CAMERA_MAX_COUNT];//统计错误信息
    //卡相关参数
    int m_iKickMode[IOCard_MAX_COUNT];//踢废模式0:连续踢 1:持续好 2：正常踢
    //总数统计,每张卡分开统计
    int nTotal[IOCard_MAX_COUNT];                   //综合过检总数
    int nKick[IOCard_MAX_COUNT];                    //综合剔废总数
    int nCompensateKick[IOCard_MAX_COUNT];          //补踢过检总数(应该发给卡的补踢数)
    int nCompensateCardKick[IOCard_MAX_COUNT];      //补踢剔废总数(实际发给卡的补踢数),可能由于不用卡导致不发
    int nCardCompensateKick[IOCard_MAX_COUNT];      //卡补踢数(卡勾选补踢功能后,卡执行的补踢数)
	//保存上次页面信息
	//e_CurrentMainPage m_eLastMainPage;
//	e_CurrentSettingPage m_eLastSettingPage;


	e_SaveImageType m_eSaveImageType;
	int m_iSaveImgCount[CAMERA_MAX_COUNT];
   
	int iStatisticsInfoSaveMode;  //0按时间，1按数量
	int m_iCamContinueReject[CAMERA_MAX_COUNT];// 相机连剔监视
	//GSOAP独立计数 阴同添加
	int nGSOAP_PassCount;//过瓶总数
	int nGSOAP_KickCount;//踢废总数
	int nGSOAP_CheckCount;//检测总数
	int nGSoap_ErrorCamCount[CAMERA_MAX_COUNT];// 每个相机的错误计数
	int nGSoap_ErrorTypeCount[ERRORTYPE_MAX_COUNT];//每个错误类型的错误计数

	int m_GSoap_Last_failureNumFromIOcard;//用于保存上一次踢废计数
	int m_GSoap_Last_checkedNum;//用于保存上一次检测总数

    //当前统计
    int nTotalBot;  //过瓶总数
    int nRejectBot; //剔废数
    int nRead;      //成功识别数
    int nInTime;    //及时识别数
    int nOverTime;  //超时个数 使用接口卡补踢计数统计
    int nEngraved;  //???

	_RunningInfo()
	{
		nGSOAP_PassCount = 0;//阴同添加
        nGSOAP_KickCount = 0;//阴同添加
		nGSOAP_CheckCount = 0;//阴同添加
		m_GSoap_Last_failureNumFromIOcard = 0;//阴同添加
		m_GSoap_Last_checkedNum = 0;//阴同添加

		m_iPermission = 0;

		m_iLastIOCard1IN0 = 0;
		m_iLastIOCard2IN0 = 0;
		m_bCheck = false;
		m_checkedNum = 0;
		m_passNum = 0;
		m_failureNum = 0;
        refCardSN = 0;
		iStatisticsInfoSaveMode = 0;
		nModelCheckedCount = 0;
		for (int i=0;i<CAMERA_MAX_COUNT;i++)
		{
			nGSoap_ErrorCamCount[i] = 0;//阴同添加

			m_iErrorCamCount[i] = 0;
			m_iErrorCamRate[i] = 0;
			m_bIsCheck[i] = true;
			m_iSaveImgCount[i] = 0;
			m_iCamContinueReject[i] = 0;

		}
		for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
		{
			nGSoap_ErrorTypeCount[i] = 0;//阴同添加
			m_iErrorTypeCount[i] = 0;
		}
        for(int i = 0; i < IOCard_MAX_COUNT;i++)
        {
            m_iKickMode[i] = 2;
            nTotal[i] = 0;
            nKick[i] = 0;
            nCompensateKick[i] = 0;
            nCompensateCardKick[i] = 0;
            nCardCompensateKick[i] = 0;
        }

		m_eSaveImageType = NotSave;

		strSpeed = "0";

        nTotalBot = 0; 
        nRejectBot = 0;
        nRead = 0;     
        nInTime = 0;   
        nEngraved = 0; 
	}

}s_RunningInfo;

typedef struct _ConfigInfo
{
	//s_SystemInfo sSystemInfo;
	//s_CamInfo sCamInfo;
	//s_ErrorInfo sErrorInfo;
	QString m_strAppPath;
	QString m_strConfigPath;
	QString m_strDataPath;
	QString m_strGrabInfoPath;
	QString m_strErrorTypePath;
	QString m_strModelInfoPath;
	QString m_strPLCStatusTypePath;
	QString m_sAlgFilePath;// 算法路径 [10/26/2010 GZ]
	QString m_sRuntimePath;
///	QString m_strModelName;

	_ConfigInfo()
	{
//		sSystemInfo = s_SystemInfo();
	}

}s_ConfigInfo;

typedef struct _RuntimeInfo
{
	bool isEnable;
	int total;
	QList<bool> AlarmsEnable;
	QList<int>  AlarmsDays;
	QStringList AlarmsInfo;

}s_RuntimeInfo;

class BottleResult
{
public:
    BottleResult()
    {
        imgNO = -1;
        reset();
    }
    void reset()
    {
        idLine = "";
        cavityNum = -1;
        rate = 0;
        camNO = -1;
    }
    bool operator==(const BottleResult& t)const{
        return idLine == t.idLine &&
            dtStamp == t.dtStamp &&
            cavityNum == t.cavityNum;
    }

    bool isHigher(const BottleResult& t)const{
        return rate > t.rate;
    }

    int imgNO;              //瓶子对应的图像号
    int camNO;              //结果对应的相机ID
    int rate;               //算法得出的准确率 越大则越可信,综合使用此值来判定是否采用
    //码中信息
    QString idLine;         //ID 失败则为空
    QDateTime dtStamp;      //时间戳
    QString cavityNum;          //线体编号

};

#endif // COMMON_H