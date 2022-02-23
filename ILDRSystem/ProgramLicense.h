// ProgramLicense.h: interface for the CProgramLicense class.
//
//////////////////////////////////////////////////////////////////////
// #include "alg_struct.h"
// using namespace Alg;

#if !defined(AFX_PROGRAMLICENSE_H__A36FE32F_4A7E_435E_B3E1_D226AF592D80__INCLUDED_)
#define AFX_PROGRAMLICENSE_H__A36FE32F_4A7E_435E_B3E1_D226AF592D80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef GUID ver_code;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define	_EXPORTING

#ifdef _EXPORTING
#define CLASS_DECLSPEC    __declspec(dllexport)  //程序编译时使用
#else
#define CLASS_DECLSPEC    __declspec(dllimport)	 //动态库应用时使用
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CLOSE_WINDOWN           WM_USER+1000   //关闭程序消息，程序运行过程中拔掉加密狗或者回拨时间
#define TIME_ERROR              WM_USER+1001   //程序运行过程中拔掉加密狗或者回拨时间, 一分钟后发送CLOSE_WINDOWN消息
#define DATE_WARN               WM_USER+1002   //即将到期提醒

#define KV_NOLIMITATION			0		//序列号无限制
#define KV_NOTREACHLIMITATION	-1		//未达到限制，且离截止日期还很远
#define KV_CLOSETODEADLINE		-2		//未达到限制，离截止日期很近

#define KV_NOTFINDROCKEYDLL		1		//没找到加密狗动态库
#define KV_NOTFINDDONGLE		2		//没找到加密狗
#define KV_OPENDONGLEFAIL		3		//打开加密狗失败
#define KV_FAILLOADKEY			4		//加载密钥失败
#define KV_UNVALIDUSERID		5		//用户名错误
#define KV_INVALIDKEY			6		//非法密钥或秘钥过期
#define KV_EXPIREDKEY			7		//密钥过期（不用了）
#define KV_LICENSEFILENOTFOUND  8		//没找到License文件
#define KV_INFOCHANGED			9		//注册信息被更改
#define KV_VCODEERROR			10		//UUID验证失败
#define KV_CALLBACKTIME			11		//检测到回拨
#define KV_LOADALGORITHMFAIL	12		//加载加密狗算法失败
#define KV_ALGORITHMVERIFYFAIL	13		//加密狗算法不对
#define KV_LOADDONGLEDATAFAIL	14		//读取加密狗日期失败
#define KV_INVALIDTEMPLICENSE	15		//非法的临时授权
#define KV_WRITEDOGFAILED		16		//写入加密狗失败
#define KV_CODEDECODEFAIL		17		//编码，解码失败
#define KV_INVALIDDATESTRING	18		//非法的日期字符串
#define KV_INVALIDDATEFORMAT	19		//非法的日期格式
#define KV_LOADDOGFUNCTIONFAIL	20		//加载加密狗函数失败
#define KV_PERIODCHECKDOGFAIL	21		//定期验证加密狗失败
#define KV_UNKNOWPROBLEM		99		//其他错误

typedef struct _KeyVerfResult
{
	int nError;				//对应上边的define的东西	
	int nExpireDate;		//-1: 无此项限制  0: 未到截止日期  1: 距离截止日期少于15天
	int nDays;				//-1: 无此项限制  0: 未达到使用天数限制  1: 距离使用天数限制还剩少于15天
	int nGlobalRuntime;		//-1: 无此项限制  0: 未达到使用时间限制  1: 距离使用天数限制少于30分钟
	int nExecutions;		//-1: 无此项限制  0: 未达到使用次数限制  1: 距离使用次数限制还剩少于10次
	char chErrorDetail[1024];
}s_KeyVerfResult;

class CLASS_DECLSPEC CProgramLicense  
{
public:
	
	CProgramLicense();
	virtual ~CProgramLicense();
	
	void GetVerCode(ver_code* vCode);//验证UU码
	void SetMainWnd(const HWND hWnd);//设置主窗口句柄，不调用此函数主窗口句柄默认为AfxSetMainWnd()->m_hWnd
	s_KeyVerfResult CheckLicenseValid(BOOL bFirst);//授权验证及定时验证	
	int	ReadHardwareID(char* chHardwareID);//getexpdate获得剩余天数,warn_days设置消息提醒例如warn_10离到期10天提醒DATE_WARN
	int ResetDog(char* chPassWord);//0:加密狗重置成功，1：加密狗重置失败，2：密码格式错误，3:错误代码验证失败
	DWORD ReadDog(void);//读加密狗，0xFFFFFFFF读取错误

	int CheckDogTime();	//调试用0: Good  1: 回拨日期  2: 非法日期或加/解密错误  >3: 加密狗类型错误
	void ShowDongleErrorCode();//调试用	
	void OpenDog(); //调试用！！
	
private:
	void* m_pCheck;
//	LONGLONG m_llFreq;	
};

#endif // !defined(AFX_PROGRAMLICENSE_H__A36FE32F_4A7E_435E_B3E1_D226AF592D80__INCLUDED_)