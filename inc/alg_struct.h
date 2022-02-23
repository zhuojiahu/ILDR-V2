/**************
文件名：alg_struct.h
功能: 提供算法动态库相关结构体的定义，供系统程序调用
日期：2013/3/1
作者：tyx
***************/

#ifndef ALG_STRUCT_H
#define ALG_STRUCT_H

#include "vector"

#include "windows.h"

#define	OUTPUT_MAXERRNUM 20
#define	BOTTLEXLD_POINTNUM 50  //2013.9.22 nanjc 提取轮廓骨架点最大个数

namespace Alg
{
	//图像结构体
	typedef struct _ImagePara
	{
		int nWidth;   //图像宽
		int nHeight;  //图像高
		int nChannel;
		char *pcData; //图像数据
	}s_ImagePara;

	//函数执行状态信息
	typedef struct _Status
	{
		int nErrorID;					//错误代码ID号
		char chErrorInfo[MAX_PATH];		//错误描述
		char chErrorContext[MAX_PATH];  //错误附加信息，具体某个函数出错	
	}s_Status;

	// 检测初始化参数
	typedef struct _AlgInitParam
	{
		int    nCamIndex;        //相机序号,每个相机对应一个检测实例
		int    nModelType;       //检测类型:0 瓶口 1 瓶脖 2 瓶底 ...弃用
		int    nWidth;           //图像宽度
		int    nHeight;	         //图像高度
		char   chCurrentPath[MAX_PATH]; //run\AlgrithmFiles目录路径
		char   chModelName[MAX_PATH];   //模板名称
	}s_AlgInitParam;

	//2013.9.22 nanjc 用于正常图像定位应力图像
	typedef struct _Loc_Ori
	{
		float modelRow;
		float modelCol;
		float modelAngle;
		float modelRadius;
		float fParam;//保留
		_Loc_Ori()
		{
			modelRow = 0;
			modelCol = 0;
			modelAngle = 0;	
			modelRadius = 0;
			fParam = 0;
		}
	}s_Loc_Ori;
	typedef struct _Xld_Point
	{
		int nCount;
		int * nRowsAry;
		int * nColsAry;
		_Xld_Point()
		{
			nCount = 0;
			nRowsAry = NULL;
			nColsAry = NULL;	
		}
	}s_Xld_Point;
	//图像定位信息
	typedef struct _AlgImageLocInfo
	{
		s_Loc_Ori		sLocOri;		//原点信息
		s_Xld_Point		sXldPoint;		//轮廓信息
	}s_AlgImageLocInfo;

	// 检测输入参数 
	typedef struct _AlgCheckInputParam
	{
		s_ImagePara sInputParam;
		s_AlgImageLocInfo sImgLocInfo; //2013.9.22 nanjc 用于正常图像定位应力图像
		int nParam;//保留 用于图像旋转的角度
		float fParam;//保留
	}s_AlgCInP;

	//2013.9.12 nanjc 初始化
	// 检测输出_错误结构体
	typedef struct _ErrorPara
	{
		RECT rRctError;  //错误区域
		int  nErrorType; //错误类型
		int  nArea;      //错误面积
		int  nLevel;     //错误级别
		int	 nxoffset;	 //x位移
		int	 nyoffset;	 //x位移

		_ErrorPara()
		{
			rRctError.bottom = 0;
			rRctError.left = 0;
			rRctError.right = 0;
			rRctError.top = 0;
			nErrorType = 0;
			nArea = 0;
			nLevel = 0;
			nxoffset = 0;
			nyoffset = 0;
		}
	}s_ErrorPara;

	// 检测输出_尺寸测量结果结构体
	typedef struct _SizeResult
	{
		float fBodyDia;			// 身外径
		float fNeckDia;			// 脖外径
		float fMouthDia;		// 口外径
		float fInnerDia;		// 瓶口内径 // 封口径（fSealDia）
		float fTotalHeight;		// 瓶全高
		float fMouthHeight;		// 口边厚	

		_SizeResult()
		{		
			fBodyDia= -1;
			fNeckDia= -1;
			fMouthDia=-1;
			fInnerDia=-1;
			fTotalHeight=-1;
			fMouthHeight=-1;	
		}
	}s_SizeResult;


	//2013.9.12 nanjc 跨边界传递stl异常，改用数组指针传递
	//检测输出结果结构体
	typedef struct _AlgCheckReslut
	{
		int  nSizeError;//错误总个数//算法与系统以地址方式传递检测结果，可能存在BUG，如将
		//错误总个数与结构体向量调换顺序，将导致总个数不被赋值，系统不报错！

		s_ErrorPara *vErrorParaAry;//返回错误结构体向量	

		s_SizeResult sSizeResult;

		s_AlgImageLocInfo sImgLocInfo; //2013.9.22 nanjc 用于正常图像定位应力图像

		int nArea; //正常应力综合
		int nMouldID;
	}s_AlgCheckResult;



	//模板输入设置结构体
	typedef struct _AlgModelPara
	{	
		s_ImagePara  sImgPara;   //图像数据
		
		s_AlgImageLocInfo sImgLocInfo; //2013.9.22 nanjc 用于正常图像定位应力图像
	}s_AlgModelPara;

	//用于尺寸综合时复制操作的尺寸结构体
	typedef struct _SizePara4Copy
	{
		int sizeFlag;
		float fBodyDiaS, fBodyDiaL, fBodyDiaH, fBodyDiaM;
		float fNeckDiaS, fNeckDiaL, fNeckDiaH, fNeckDiaM;
		float fMouthDiaS, fMouthDiaL, fMouthDiaH, fMouthDiaM;
		float fTotalHeightS, fTotalHeightL, fTotalHeightH, fTotalHeightM;
		float fMouthHeightS, fMouthHeightL, fMouthHeightH, fMouthHeightM;
		float fSealDiaS, fSealDiaL, fSealDiaH, fSealDiaM;

		BOOL bBodyDia,bNeckDia,bMouthDia,bTotalHeight,bMouthHeight,bSealDia;
		_SizePara4Copy()
		{
			sizeFlag=0;
			fBodyDiaS=fNeckDiaS=fMouthDiaS=fTotalHeightS=fMouthHeightS=fSealDiaS=20.0;
			fBodyDiaL=fNeckDiaL=fMouthDiaL=fTotalHeightL=fMouthHeightL=fSealDiaL=0.0;
			fBodyDiaH=fNeckDiaH=fMouthDiaH=fTotalHeightH=fMouthHeightH=fSealDiaH=1000.0;
			fBodyDiaM=fNeckDiaM=fMouthDiaM=fTotalHeightM=fMouthHeightM=fSealDiaM=0.0;
			bBodyDia=bNeckDia=bMouthDia=bTotalHeight=bMouthHeight=bSealDia=FALSE;	
		}
	}s_SizePara4Copy;
}
#endif