#pragma once
#include "Omron.h"
const int maxpath=512;

typedef struct _PLCSTATUS
{
	bool bOnline;//PLC连线标志，连线为true，离线为false
	bool bRunning;//设备运行标志，运行为true，停止位false
	bool bManual;//手动状态标志，设备切换至手动状态时为true，false时可不理会
	bool bTestMode;//测试状态标志，测试参数阈值时为true，false时可不理会
	int nAlarm;
	int nCh0;
	int nCh1;
	int nCh100;
	int nCh101;
	int nEncoder;
	_PLCSTATUS()
	{
		bOnline=false;
		bRunning=false;
		bManual=false;
		bTestMode=false;
		nAlarm=0;
		nCh0=0;
		nCh1=0;
		nCh100=0;
		nCh101=0;
		nEncoder=0;
	}
}s_PLCStatus;


class __declspec(dllexport) CHMIDLL
{
public:
	CHMIDLL(void);
	~CHMIDLL(void);
public:
	//设置相对路径；调用InitSerialComm前使用
	void InitPath(char szPath[maxpath]);
	//调用参数设置对话框
	void CallParamDlg(void);
	//初始化串口
	bool InitSerialComm(void);
	//关闭串口
	void CloseComm(void);
	//读取PLC连线状态、报警信息/输入输出状态等
	bool GetPLCStatus(s_PLCStatus &s);
	//串口是否可用标志，false时GetPLCStatus可以使用
	bool m_bIsCommBusy;
	bool SetAlarm(int nAlarm);//相机掉线报警，20160913
protected:
	void GetAppPath(char szFullPath[maxpath]);
	void SaveCommInfo();
	void LoadCommInfo();
	//判断当前路径下Config文件夹是否存在，如果不存在则创建
	void IsFolderExist();
	int m_nPort;
	int m_nBaud;
	COmron *m_pOmron;
	char m_szPath[maxpath];
};
