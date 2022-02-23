/************************************************************************/
/* Copyright (c) 2012, 北京大恒图像视觉有限公司  
/* All rights reserved.													
/*																		
/* 文件名称： DHGrabberForAVT.h	
/* 摘要： 针对AVT相机FGCamera版本(3.00) 如有更高版本则需要升级
/*
/* 当前版本： 6.0.0.1Beta1
/* 修改者： 孙国强
/* 修改内容：重整框架
/* 完成日期： 2012-07-31
/************************************************************************/
#include "DHGrabberBase.h"

#if !defined(DHGrabberAVT_h_)
#define DHGrabberAVT_h_

//////////////////////////////////////////////////////////////////////////
//采集类

enum AVTParamID	//AVT相机专有参数
{
	AVTColor,               //相机类型  黑白和彩色 
	AVTBrightness,			//相机亮度大小
	AVTShutter,				//相机快门值
	AVTGain,				//相机增益大小
	AVTU,					//相机U值
	AVTV,					//相机V值
	AVTTriggerOnOff,		//相机外触发是否开启
	AVTTemperature			//相机温度
};


// AVT相机专有接口
enum AVTParamVal
{
	AVTTriggerOff = 0,		//外触发关闭//内部触发,连续采集
	AVTTriggerOn,	    	//上升沿触发//外触发开启
	AVTTriggerMode2,		//下降沿触发
	AVTTriggerMode3,		//高电平触发
	AVTTriggerMode4,		//低电平触发
	AVTTriggerMode5,		//编程模式上升沿触发
	AVTTriggerMode6,		//编程模式下降沿触发	
};

class _declspec(dllexport) CDHGrabberAVT : public CGrabber
{
	//操作
public:
	CDHGrabberAVT();
	virtual ~CDHGrabberAVT();

	//初始化
	BOOL Init(const s_GBINITSTRUCT* pInitParam);

	//关闭
	BOOL Close();

	//开始采集
	BOOL StartGrab();

	//停止采集
	BOOL StopGrab();

	//单贞采集
	BOOL Snapshot();

	//设置参数
	BOOL SetParamInt(GBParamID Param, int nInputVal);

	//得到参数
	BOOL GetParamInt(GBParamID Param, int &nOutputVal);

	//调用参数对话框
	void CallParamDialog();

	//得到出错信息
	void GetLastErrorInfo(s_GBERRORINFO *pErrorInfo);

	BOOL AVTSetParamPro(AVTParamID Param, int ParamValInput);

	BOOL AVTSetTriggerParam(AVTParamVal ParamValInput);

	//////////////////////////////////////////////////////////////////////////
	//属性
protected:
	CGrabber *m_pGrabber;
};

#endif// !defined(DHGrabberAVT_h_)

//////////////////////////////////////////////////////////////////////////
//使用说明:   目前采集库基于AVT相机的驱动版本为2.10  
//适用相机类型：Guppy、Stingray、Marlin系列
//1. 将原来的ini相机参数配置文件直接更换为相机默认的参数配置文件XML
//2. 将相机配置修改如下：
//   设备A序号=0
//   设备A名称=6735372523180860609 (可以自己定义一个名称)
//   设备A标识=6735372523180860609
//   设备A初始化文件名=GuppyConfig.xml   //将用AVT 的SmartView保存的xml文件名称更换到这里
//3. 另外将保存的XML文件拷贝到Config文件夹下(即原来的GuppyConfig.ini同级目录下)
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//修改记录
/*
2010.12.7  V4.0.0.3  杨富森**目的：修改多个相机读写XML文件文件***
修改内容：
1. 解决多个相机时，修改参数保存到XML文件的问题；
2. 相机配置序列号，可以只是写入前面的15位(6+9),将相机配置修改如下：
设备A序号=0
设备A名称=AVT
设备A标识=673537252318086
设备A初始化文件名=GuppyConfig.xml   //将用AVT 的SmartView保存的xml文件名称更换到这里
另外还兼容后面带有0609版本
3. 修改接口AVTGetCamGuid名称，由原来的AVTGetCamGaid修改成AVTGetCamGuid
4. 程序退出默认保存XML文件

2010.5.24  V4.0.0.1Beta1  **目的：从原有的ini配置文件升级直接使用相机保存的xml文件***
修改内容：
1. 将原来的ini相机参数配置文件直接更换为相机默认的参数配置文件XML；
2. 将相机配置修改如下：
设备A序号=0
设备A名称=6735372523180860609
设备A标识=6735372523180860609
设备A初始化文件名=GuppyConfig.xml   //将用AVT 的SmartView保存的xml文件名称更换到这里
3. 另外将保存的XML文件拷贝到Config文件夹下(即原来的GuppyConfig.ini同级目录下)

2010.5.26  V4.0.0.1Beta2  **目的：在停止采集时，可以在相机参数对话框上，将修改的相机参数保存为XML**
修改内容：
1. 可以在相机参数中保存为XML；Beta2
2. 添加获取相机类型(黑白和彩色)功能；通过AVTColor参数获得；

2010.6.21  V4.0.0.1Beta3  **目的：方便在检测过程中(相机正在采集时)调整的相机参数，用于下一次使用**
修改内容：
1. 相机退出时再保存一次XML。

2010.6.23  V4.0.0.1Beta4  
修改内容：
1. 解决多个AVT相机采集的问题；
*/
//////////////////////////////////////////////////////////////////////////