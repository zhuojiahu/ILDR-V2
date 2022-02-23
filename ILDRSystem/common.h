#ifndef COMMON_H
#define COMMON_H
#pragma once

//解除_WIN32_WINNT 未定义的编译警告
#ifndef _WIN32_WINNT 
#define _WIN32_WINNT 0x0502
#endif

#define SHADOW_WIDTH            0
#define IMAGE_SPACE             10
#define TITEL_HEIGHT            110
#define SPPEDHIGHEST            400
#define SPPEDNORMAL             300

#define DEFAULT_SKIN            ":/skin/1b"
#include <qimage.h>
#include <QWaitCondition>
#include <QMutex>

//最大相机个数
#define CAMERA_MAX_COUNT 50
//#define ERROR_MAX_COUNT 43
//最多错误类型个数
#define ERRORTYPE_MAX_COUNT 50
//最大模号数量
#define MOLDNO_MAX_COUNT 100
//IO卡数量
#define IOCard_MAX_COUNT 2
//错误图像个数
#define ERROR_IMAGE_COUNT 30

//#define PASSWORD1 ""
//#define PASSWORD2 "daheng"
#define PASSWORD3 "kongping"

typedef enum{
	UI_EN,
	UI_ZH
}LANGUAGE;
typedef enum{
	SQL_NEW,
	SQL_MODEFY,
	SQL_DELETE,
	SQL_RESERVE
}SQLSTATE;

//typedef enum{
//	NORMAL,
//	ENTER,
//	PRESS,
//	NOSTATUS
//}ButtonStatus;


//页面导航索引定义
const int ENaviPageEndIndex = 7;        //页面结束索引
enum ENavigation   
{
    ENaviPageOperation = 0,     //操作界面
    ENaviPageAlgSet,            //算法设置界面
    ENaviPageArticle,           //模板设置界面
    ENaviPageSettings,          //系统设置界面
    ENaviPageHistory,           //历史界面
    ENaviPageInOut,             //输入输出界面
    ENaviPageAlarm,             //报警界面
    ENaviPageLight,             //灯光检查界面
    ENaviToolResetCount,        //重置计数按钮
    ENaviToolStart,             //启动按钮
    ENaviToolLock,              //锁定按钮
    ENaviToolExit,              //退出按钮
};


enum e_ToolButton	
{
	ImageButton = 0,
	InfoButton,
	CarveSettingButton,
	ManagementSettingButton,
	TestButton,
	CountButton,
	AlgButton
};

//保存日志类型
enum e_SaveLogType	    
{
	OperationLog = 0,			//操作日志
	DebugLog,					//调试日志(缺图 || 误触发 || 缓存区为空 等日志信息)
	AbnormityLog,               //异常日志(异常捕获 || 理论上不应出现的判断错误)

	CheckLog,		            //检测日志
	MendParamLog,			    //修改参数日志
	StatInfo,					//检测统计信息日志
	AlgorithmDLL,				//算法库日志
	AlarmLog
};
// 采集图像类型
enum e_GrabImageType	
{
	BodyImage = 0,	    // 瓶身
	BottomImage		// 瓶底
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

// 相机序号
enum e_CameraType	
{
	Camera1 = 0,	 
	Camera2,
	Camera3,
	Camera4		
};

//当前绘制状态
enum e_DrawStatus	
{
	DRAW_Status_NULL = 0,			//无操作
	DRAW_Status_MOVE,				//移动
	DRAW_Status_ZOOM,				//缩放
	DRAW_Status_MODIFY,				//修改
};


//当前鼠标位置所在点
enum e_DrawSelect	
{
	DRAW_Select_NULL = 0,			//无操作点
	DRAW_Select_MOVE,				//移动点
	DRAW_Select_ZOOM,				//缩放点
	DRAW_Select_MODIFY,				//修改点
};

//权限
typedef struct _Permission
{
	int iClear;
	int iExit;
	int iStartStop;
	int iImageCarve;
	int iVarietyManege;
	int iSystemSet;
	int iPLCSet;
	int iAlgSet;
	_Permission()
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
//选取点
typedef struct _SelectPoint
{
	int iX;
	int iY;
	int iOffset;
	_SelectPoint()
	{
		iX = 0;
		iY = 0;
		iOffset = 5;
	}
}s_SelectPoint;

//矩形结构体
typedef struct _RectangleShape
{
	int ileftupX;
	int ileftupY;
	int iWidth;
	int iHeight;
	_RectangleShape()
	{
		ileftupX = 0;
		ileftupY = 0;
		iWidth = 1;
		iHeight = 1;
	}
	_RectangleShape(QRect rt)
	{
		ileftupX = rt.x();
		ileftupY = rt.y();
		iWidth = rt.width();
		iHeight = rt.height();
	}
}s_RectangleShape;

typedef struct _IOSignal 
{
	short iGrabCount;				//图像号
	short iGrabDelay;				//采集延时
	_IOSignal()
	{
		iGrabCount = 0;
		iGrabDelay = 0;
	}
}s_IOSignal;

typedef struct _ResultInfo
{
public:
	int nIOCardNum;
	int nCamNo;
	int nImgNo;
	double dCostTime;
	int tmpResult;

	_ResultInfo()
	{
		nIOCardNum  = 0;
		nCamNo = 0;
		nImgNo = 0;
		dCostTime = 0;
		tmpResult = 0;
	}
}s_ResultInfo;

#endif // COMMON_H