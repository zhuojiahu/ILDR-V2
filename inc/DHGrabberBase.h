#ifndef DHGrabber_h_
#define DHGrabber_h_

#include <WTypes.h>

#pragma once

/*
采集卡类型编号定义如下
0:模拟采集卡
1:DALSA X64采集卡
2:AVT相机
3:千兆网相机
4:公司CG300模拟采集卡
5:公司1394相机
6:EURESYS 采集卡
7:公司USB相机
8:公司水星系列相机
9:线阵相机DALSA
*/
enum EGraberType
{
    EGT_Simu = 0,       //模拟采集卡
    EGT_Dalsa_X64,      //DALSA X64采集卡
    EGT_AVT,            //AVT相机
    EGT_GigE,           //千兆网相机
    EGT_DH_CG300,       //公司CG300模拟采集卡
    EGT_DH_1394,        //公司1394相机
    EGT_Euresys_Card,   //EURESYS 采集卡
    EGT_DH_USB,         //公司USB相机
    EGT_DH_MER,         //公司水星系列相机
    EGT_LCam_DALSA,     //线阵相机DALSA
};

const int GBMaxFileNameLen = 255;
const int GBMaxTextLen = 255;
const int MaxSNLen  = 32;

typedef struct _GBSignalInfoStruct//回调信息结构
{
    PVOID Context;                                //存放拥有回调函数的对象的this指针
    int iGrabberTypeSN;                            // 采集卡类型编号
    int nGrabberSN;                                //采集卡序号
    int nErrorCode;                                //错误代码（0：正常）
    int nFrameCount;                            //已采集的帧记数
    char strDescription[GBMaxTextLen];            //描述信息

    int iReserve1;                                // 备用字段1
    int iReserve2;                                // 备用字段2
    
}s_GBSIGNALINFO;//回调信息结构

//回调函数指针声明
typedef void (WINAPI* PGBCALLBACK)(const s_GBSIGNALINFO* SignalInfo);

typedef struct _GBInitStruct//采集卡初始化结构
{
    EGraberType iGrabberTypeSN;                 //相机类型
    int nGrabberSN;                             //采集卡序号
    char strDeviceName[GBMaxTextLen];           //设备名称
    char strDeviceMark[GBMaxTextLen];           //设备标识
    char strGrabberFile[GBMaxFileNameLen];      //用于初始化的文件
    PGBCALLBACK CallBackFunc;                   //回调函数指针
    PVOID Context;                              //存放拥有回调函数的对象的this指针
    
    int iReserve1;                              // 备用字段1
    int iReserve2;                              // 备用字段2

}s_GBINITSTRUCT;//采集卡初始化结构

typedef struct _GBErrorInfoStruct//错误信息结构
{
       int nErrorCode;                            //错误代码（0：正常）
    char strErrorDescription[GBMaxTextLen];    //错误描述
    char strErrorRemark[GBMaxTextLen];        //附加信息

}s_GBERRORINFO;//错误信息结构

//公共参数
enum GBParamID
{
    GBImageWidth = 0,           //图像宽度 (单位:字节)
    GBImageHeight,              //图像高度 (单位:字节)
    GBImagePixelSize,           //图像象素大小 (单位:字节)
    GBImageBufferSize,          //图像缓冲区大小 (单位:字节)
    GBImageBufferAddr,          //图像缓冲区地址
    GBGrabberTypeSN,            //采集卡类型编号
    GBImageBufferAddr2,         //图像缓冲区高地址(64位库使用）
};

//错误代码定义
const int GBOK = 0;

class CGrabber
{
//操作
public:
    CGrabber()
    {
    }
    virtual ~CGrabber()
    {
    }
    
    //初始化
    virtual BOOL Init(const s_GBINITSTRUCT* pInitParam) = 0;

    //关闭
    virtual BOOL Close() = 0;

    //开始采集
    virtual BOOL StartGrab() = 0;

    //停止采集
    virtual BOOL StopGrab() = 0;
    
    //单帧采集
    virtual BOOL Snapshot() = 0;

    //设置参数
    virtual BOOL SetParamInt(GBParamID Param, int nInputVal) = 0;
    
    //得到参数
    virtual BOOL GetParamInt(GBParamID Param, int &nOutputVal) = 0;
    
    //调用参数对话框
    virtual void CallParamDialog() = 0;
    
    //得到出错信息
    virtual void GetLastErrorInfo(s_GBERRORINFO *pErrorInfo) = 0;

};

#endif//DHGrabber_h_