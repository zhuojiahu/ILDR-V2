#ifndef ROTEMPTYBOTTLE_H
#define ROTEMPTYBOTTLE_H


#define DLL_EXT _declspec(dllexport) 

#include "alg_struct.h"
using namespace Alg;

#include <vector>
using namespace std;

#include <iostream>

const int RETURN_OK           = 0; //返回正常
const int RETURN_CHECK_INIT   = 1; //初始化错误
const int RETURN_MODEL_MANAGE = 2; //模板管理错误
const int RETURN_PARA_MANAGE  = 3; //检测参数设置错误
const int RETURN_CHECK_ERROR  = 4; //检测算法出错
const int RETURN_CHECK_FREE   = 5; //释放错误
const int RETURN_NO_CLOSE	  = 6; //禁止关闭窗口


class DLL_EXT RotEmptyBottle
{
public:
	RotEmptyBottle();
	~RotEmptyBottle();
private:

};
class CBottleCheck;

typedef struct 
{
	int nID;   //区域ID
	CBottleCheck  *pChecker;   //检测类对象
}s_CheckerList;

//2013.9.11 nanjc 跨边界传递泛型异常，改用数组传递
typedef struct 
{
	int iValidNum; //数组个数（相机个数）
	s_CheckerList *pCheckerlist; //检测对象指针
}s_InputCheckerAry;

typedef struct 
{
	bool bIsChecking; //是否正在检测

	bool bReserve;  //备用
	int nReserve;	//备用
	char* cReserve; //备用
}s_SystemInfoforAlg;

///*****以下为算法动态库相关接口定义*****//

// 检测抽象类
class DLL_EXT CAlgorithm  
{
public:
	CAlgorithm()
	{
	}
	virtual ~CAlgorithm()
	{
	}
	virtual Alg::s_Status init(Alg::s_AlgInitParam sAInitParam)=0;
	virtual Alg::s_Status Free()=0;
	virtual Alg::s_Status Check(Alg::s_AlgCInP sAlgCInP,Alg::s_AlgCheckResult **sAlgCheckResult)=0;
	virtual Alg::s_Status CopySizePara(Alg::s_SizePara4Copy &sSizePara)=0;
	virtual bool setsSystemInfo(s_SystemInfoforAlg &sSystemInfo)=0;
	virtual bool FindTwoPointsSubpix(Alg::s_ImagePara sImagePara,double inRow,POINT &pLeft,POINT &pRight,int nEdge)=0;
};

// 模板抽象类
class DLL_EXT CModel 
{
public:
	CModel()
	{
	}
	virtual ~CModel()
	{
    }

	virtual Alg::s_Status SetModelDlg(Alg::s_AlgModelPara sAlgModelPara,CBottleCheck *pChecker,
		s_InputCheckerAry checkerAry,void *parent = NULL)=0;
	virtual Alg::s_Status CloseModelDlg() = 0;

};


///*****以下为算法导出函数定义*****//

/************************************************************************
	功能：初始化检测动态库，必须在整个动态库开始前或是改变任务后调用区域前
    int nWidth, int nHeight该任务图像宽高
	这是对于所有类需要统一初始化的参数 
/************************************************************************/
s_Status DLL_EXT init_bottle_module(int nWidth, int nHeight, int nChannel);

s_Status DLL_EXT free_bottle_module();

//检测接口类
class DLL_EXT CBottleCheck : public CAlgorithm
{
public:
	CBottleCheck();
	virtual ~CBottleCheck();
	
   /************************************************************************
	    功能：初始化检测类参数
 	/************************************************************************/
	s_Status init(Alg::s_AlgInitParam sAlgInitParam);
	/************************************************************************
	    功能：释放检测类分配的资源
 	/************************************************************************/
	s_Status Free();	
	/************************************************************************
	    功能：质量检测
	    s_AlgCInP sAlgCInP  检测输入参数
        s_AlgCheckResult **sAlgCheckResult   检测输出参数
 	/************************************************************************/
	s_Status Check(s_AlgCInP sAlgCInP,s_AlgCheckResult **sAlgCheckResult);	
	/************************************************************************
	    功能：用于尺寸检测数据综合时复制参数到当前相机模板文件
 	/************************************************************************/	
	Alg::s_Status CopySizePara(Alg::s_SizePara4Copy &sSizePara);
	bool setsSystemInfo(s_SystemInfoforAlg &sSystemInfo);
	bool FindTwoPointsSubpix(Alg::s_ImagePara sImagePara,double inRow,POINT &pLeft,POINT &pRight,int nEdge);

public:
	CAlgorithm *m_pAlg;
};

//模板接口类 
class DLL_EXT CBottleModel : public CModel
{
public:
	// 检测类指针，用于更新对话框上设置的数据
	CBottleCheck *m_pChecker;

	CBottleModel();
	virtual ~CBottleModel();
	
	/************************************************************************
	    功能：设置检测模板参数
        s_AlgModelPara sAlgModelPara 模板输入数据，暂时只包含图像信息
	    CBottleCheck *pChecker  当前检测指针
        s_InputCheckerAry checkerAry 所有检测数组，用于-
		        更新所有区域使用
	/************************************************************************/	
	Alg::s_Status SetModelDlg(Alg::s_AlgModelPara sAlgModelPara,CBottleCheck *pChecker,
		s_InputCheckerAry checkerAry,void *parent = NULL);
	Alg::s_Status CloseModelDlg();

protected:
	CModel *m_pModel;

};
#endif // ROTEMPTYBOTTLE_H
