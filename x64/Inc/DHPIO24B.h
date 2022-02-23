/************************************************************************/
/* Copyright (c) 2009, 北京大恒图像视觉技术有限公司视觉系统部    
/* All rights reserved.													
/*																		
/* 文件名称： DHPIO24B.h
/* 摘要： 
/*
/* 当前版本： 3.1
/* 修改者： 葛争
/* 完成日期： 2009年9月24日
/* 说明： 接口卡寄存器说明文档
/*加入读写系统寄存器
/************************************************************************/

#if !defined(PIO24B_h_)
#define PIO24B_h_


//////////////////////////////////////////////////////////////////////////
//

// 电平状态
enum e_PIOLevelVal
{
	LevelLow = 0,
	LevelHigh
};

// 接口卡复位模式
enum e_PIOResetMode
{
	FPGAProgramReset = 0x5a5a, // 复位PIO24B接口卡的FPGA程序，但不复位所有的parameter寄存器
		PramRegisterReset = 0x12ab // 复位所有的parameter寄存器，但不复位PIO24B接口卡的FPGA程序
};

// 中断等待返回值
enum e_InterruptReturnVal
{
	WaitSuccess = 0,
		WaitTimeout,
		WaitCanceled,
		WaitFailed
};

//  系统寄存器[9/24/2009 gZ]
enum e_PIOSystemRegister
{
	SystemRegister0	 = 0,	//	{"版本复位寄存器", 0},
	SystemRegister1	,		//	{"项目号寄存器0", 1},
	SystemRegister2	 ,		//	{"项目号寄存器1", 2},
	SystemRegister3			//	{"监测系统死机状态", 3},

};
// 触发刻度表格首址
enum e_PIOTriggerTable
{
	TriggerTable0 = 0,
	TriggerTable1,
	TriggerTable2,
	TriggerTable3
};

// 计数器
enum e_PIOCounter
{
	Counter0 = 0, //	{"输入信号计数器0", 0},     
	Counter1,     //	{"输入信号计数器1", 1},     
	Counter2,     //	{"输入信号计数器2", 2},     
	Counter3,     //	{"输入信号计数器3", 3},     
	Counter4,     //	{"输出信号计数器0", 4},     
	Counter5,     //	{"输出信号计数器1", 5},     
	Counter6,     //	{"输出信号计数器2", 6},     
	Counter7,     //	{"输出信号计数器3", 7},     
	Counter8,     //	{"输出信号计数器4", 8},     
	Counter9,     //	{"输出信号计数器5", 9},     
	Counter10,    //	{"输出信号计数器6", 10},     
	Counter11,    //	{"输出信号计数器7", 11},     
	Counter12,    //	{"软帧计数器", 12},     
	Counter13,    //	{"行信号A相计数器", 13},     
	Counter14,    //	{"行信号B相计数器", 14},     
    Counter15,    //	{"帧信号计数器", 15},     
	Counter16,    //	{"A相行频率寄存器", 16},     
	Counter17,    //	{"B相行频率寄存器", 17},     
	Counter18,    //	{"A相每周最大行数寄存器", 18},     
	Counter19,    //	{"B相每周最大行数寄存器", 19},     
	Counter20,    //	{"发出触出发信号时候的行数", 20},     
    Counter21,    //	{"离压帧计数", 21},     
	Counter22,    //	{"合压帧计数", 22},     
	Counter23,    //	{"面阵相机触发回馈时的行计数", 23},     
	Counter24,    //	{"保留 ", 24},     
	Counter25,    //	{"保留", 25},     
	Counter26,    //	{"打标队列x的触发信号源 ", 26},     
	Counter27,    //	{"保留", 27},     
	Counter28,		//	{"图像号计数器0", 28},
	Counter29,		//	{"图像号计数器1", 29},
	Counter30,	//	{"图像号计数器2", 30},
	Counter31		//	{"图像号计数器3", 31},

};

// 参数控制寄存器
enum e_PIOParamRegister
{
	Parameter0 = 0,    //	{"编码器输入滤波器采样间隔", 0},
	Parameter1,        //	{"编码器输入滤波器深度", 1},
	Parameter2,        //	{"低速输入0滤波器采样间隔", 2},
	Parameter3,        //	{"编码器行信号分频", 3},
	Parameter4,        //	{"低速输入1滤波器采样间隔", 4},
	Parameter5,        //	{"保留", 5},
	Parameter6,        //	{"低速输入2滤波器采样间隔", 6},
	Parameter7,        //	{"保留", 7},
	Parameter8,        //	{"低速输入3滤波器采样间隔", 8},
	Parameter9,        //	{"保留", 9},
	Parameter10,       //	{"低速输入0最小间隔", 10},
	Parameter11,       //	{"低速输入1最小间隔", 11},
	Parameter12,       //	{"低速输入2最小间隔", 12},
	Parameter13,       //	{"低速输入3最小间隔", 13},
	Parameter14,       //	{"清零低速输入计数器", 14},
	Parameter15,       //	{"PIO24B输入端口控制", 15},
	Parameter16,       //	{"低速输入端口连接选择", 16},
	Parameter17,       //	{"清零低速输出计数器", 17},
	Parameter18,       //	{"低速输出端口连接选择-高位", 18},
	Parameter19,       //	{"低速输出端口连接选择-低位", 19},
	Parameter20,       //	{"置位低速输出端口", 20},
	Parameter21,       //	{"RS422输出端口连接选择", 21},
	Parameter22,       //	{"LVDS输出端口连接选择", 22},
	Parameter23,       //	{"置位LVDS/RS422输出端口", 23},
	Parameter24,       //	{"编码器帧信号下限参数", 24},
	Parameter25,       //	{"编码器帧信号上限参数", 25},
	Parameter26,       //	{"编码器帧信号补帧参数", 26},
	Parameter27,       //	{"编码器行信号最小周期", 27},
	Parameter28,       //	{"清零帧信号输入计数器", 28},
	Parameter29,       //	{"有效起使行数参数", 29},
	Parameter30,       //	{"输出有效位参数", 30},
	Parameter31,       //	{"“延迟队列x”模式寄存器", 31},
	Parameter32,       //	{"“延迟队列x”延迟参数-低位", 32},
	Parameter33,       //	{"“延迟队列x”延迟参数-高位", 33},
	Parameter34,       //	{"“打标队列x”模式寄存器", 34},
	Parameter35,       //	{"“打标队列x”控制寄存器", 35},
	Parameter36,       //	{"“打标队列x”检图结果", 36},
	Parameter37,       //	{"“打标队列x”图像号", 37},
	Parameter38,       //	{"“打标队列x”打标偏移位置", 38},
	Parameter39,       //	{"刻度输出0的脉冲宽度", 39},
	Parameter40,       //	{"频闪的输出延时", 40},
	Parameter41,       //	{"保留", 41},
	Parameter42,       //	{"保留", 42},
	Parameter43,       //	{"延迟队列输出的触发间隔", 43},
	Parameter44,       //	{"延迟队列输出的触发次数", 44},
	Parameter45,       //	{"延迟队列x的输出的脉冲宽度", 45},
	Parameter46,       //	{"打标队列x的输出的脉冲宽度", 46},
	Parameter47,       //	{"编码器最大行数", 47},
	Parameter48,       //	{"保留", 48},
	Parameter49,       //	{"打标队列x的延迟参数低位", 49},
	Parameter50,       //	{"打标队列x的延迟参数高位", 50},
	Parameter51,       //	{"检测到黑点后的延迟触发时间", 51},
	Parameter52,       //	{"检测到黑点后的触发脉宽", 52},
	Parameter53,       //	{"预估两个黑点的间距", 53},
	Parameter54,		   //	{"Qpick0控制寄存器", 54},
	Parameter55,			//	{"Qpick1控制寄存器", 55},
	Parameter56,		//	{"Qpick2控制寄存器", 56},
	Parameter57,		//	{"“延迟队列0”延迟参数-低位", 57},
	Parameter58,		//	{"“延迟队列0”延迟参数-高位", 58},
	Parameter59,		//	{"“延迟队列1”延迟参数-低位", 59},
	Parameter60,		//	{"“延迟队列1”延迟参数-高位", 60},
	Parameter61,		//	{"“延迟队列2”延迟参数-低位", 61},
	Parameter62,		//	{"“延迟队列2”延迟参数-高位", 62},
	Parameter63,		//	{"图像号延迟更新参数A", 63},
	Parameter64,			//	{"图像号延迟更新参数B", 64},
	Parameter65,
	Parameter66,
	Parameter67,
	Parameter68,
	Parameter69,
	Parameter70,
	Parameter71,
	Parameter72,
	Parameter73,
	Parameter74,
	Parameter75

};

// 系统状态寄存器Satus0-IO输入口状态  address = 0x800
enum e_PIOInputState
{
	In0 = 0,           //	{"PIO24B输入端口状态寄存器", 0},
	In1,               //	{"PIO24B输出端口状态寄存器", 1},
	In2,               //	{"编码器状态", 2},
	In3,               //	{"“延迟队列x”状态", 3},
	In4,               //	{"“打标队列x”状态", 4},
	In5,               //	{"“打标队列x”延迟队的状态", 5},
	In6,               //	{"“打标队列0”的状态", 6},
	In7,               //	{"“打标队列1”的状态", 7},
	In8,               //	{"“打标队列2”的状态", 8},
	In9,               //	{"“延迟队列0”状态", 9},
	In10,              //	{"“延迟队列1”状态", 10},
	In11,              //	{"“延迟队列2”状态", 11}
	In12,              // 保留
	In13,              // 保留
	In14,              // 保留
	In15               // 保留

};


// 系统状态寄存器Satus1-IO输出口状态 address = 0x802
enum e_PIOOutputState
{
	Out0 = 0,          // 第0路“PIO24B低速输出端口”
	Out1,              // 第1路“PIO24B低速输出端口”
	Out2,              // 第2路“PIO24B低速输出端口”
	Out3,              // 第3路“PIO24B低速输出端口”
	Out4,              // 第4路“PIO24B低速输出端口”
	Out5,              // 第5路“PIO24B低速输出端口”
	Out6,              // 第6路“PIO24B低速输出端口”
	Out7,              // 第7路“PIO24B低速输出端口”
	Out8,              // 保留
	Out9,              // 保留
	Out10,             // 保留
	Out11,             // 保留
	Out12,             // 保留
	Out13,             // 保留
	Out14,             // 保留
	Out15              // 保留
};

enum e_PIOSystemStateBit
{
	SysState0 = 0,          // 第0位
	SysState1,              // 第1位
	SysState2,              // 第2位
	SysState3,              // 第3位
	SysState4,              // 第4位
	SysState5,              // 第5位
	SysState6,              // 第6位
	SysState7,              // 第7位
	SysState8,              // 第8位
	SysState9,              // 第9位
	SysState10,             // 第10位
	SysState11,             // 第11位
	SysState12,             // 第12位
	SysState13,             // 第13位
	SysState14,             // 第14位
	SysState15              // 第15位
};

// 系统状态寄存器
enum e_PIOSystemStatus
{
	SystemStatus2 = 0,       //   编码器工作状态  address = 0x804
    SystemStatus3,           // “延迟队列x”状态 address = 0x806
	SystemStatus4,            // “打标队列x”状态 address = 0x808
	SystemStatus5            // “打标队列x”延迟队的状态 address = 0x80a
};


// 触发控制寄存器
enum e_PIOTriggerPosition
{
	TriggerPosition0 = 0,
	TriggerPosition1,
	TriggerPosition2,
	TriggerPosition3,
	TriggerPosition4,
	TriggerPosition5,
	TriggerPosition6,
	TriggerPosition7,
	TriggerPosition8,
	TriggerPosition9,	
	TriggerPosition10,
	TriggerPosition11,
	TriggerPosition12,
	TriggerPosition13,
	TriggerPosition14,
	TriggerPosition15,
	TriggerPosition16,
	TriggerPosition17,
	TriggerPosition18,
	TriggerPosition19,
	TriggerPosition20,
	TriggerPosition21,
	TriggerPosition22,
	TriggerPosition23,
	TriggerPosition24,
	TriggerPosition25,	
	TriggerPosition26,
	TriggerPosition27,
	TriggerPosition28,
	TriggerPosition29,
	TriggerPosition30,
	TriggerPosition31,
	TriggerPosition32,
	TriggerPosition33,
	TriggerPosition34,
	TriggerPosition35,
	TriggerPosition36,
	TriggerPosition37,
	TriggerPosition38,
	TriggerPosition39,
	TriggerPosition40,
	TriggerPosition41,	
	TriggerPosition42,
	TriggerPosition43,
	TriggerPosition44,
	TriggerPosition45,
	TriggerPosition46,
	TriggerPosition47,
	TriggerPosition48,
	TriggerPosition49,
	TriggerPosition50,
	TriggerPosition51,
	TriggerPosition52,
	TriggerPosition53,
	TriggerPosition54,
	TriggerPosition55,
	TriggerPosition56,
	TriggerPosition57,	
	TriggerPosition58,
	TriggerPosition59,
	TriggerPosition60,
	TriggerPosition61,
	TriggerPosition62,
	TriggerPosition63    
};
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 异常

class AFX_EXT_CLASS CPIO24BException
{
//属性
public:	
	int m_iCode;				// 异常代码
	CString m_strDescription;	// 异常描述
	CString m_strContext;		// 异常附加信息
	
//操作
public:
	
	CPIO24BException(int iCode, const CString &strDescription, const CString &strContext);
	~CPIO24BException();

	//对话框显示
	void ShowMessageBox();
};
//
//////////////////////////////////////////////////////////////////////////

class CPIO24B;

class AFX_EXT_CLASS CDHPIO24B
{
public:

	
	CDHPIO24B();
	virtual ~CDHPIO24B();
	
	// 打开	
	void Open();
	
	// 关闭
	void Close();	
	
	// 清零
	void SetZero();
	
	// 复位接口卡
	void ResetPortCard(e_PIOResetMode ResetMode);
	
	// 获取接口卡版本号
	CString GetPortCardVersion();
	
	// 获取接口卡程序适用的项目编号
	CString GetProjectVersion();
	
	// 得到计数器值
	int GetCounterVal(e_PIOCounter eCounter);
	
	// 得到参数值
	int GetParameterVal(e_PIOParamRegister eParameter);
	
	// 设置参数值
	void SetParameterVal(e_PIOParamRegister eParameter, int iParameterVal);
	
	// 得到IO输出口状态
	e_PIOLevelVal GetOutputState(e_PIOOutputState eOutputState);
	
	// 设置IO输出口状态
	void SetOutputState(e_PIOOutputState eOutputState, e_PIOLevelVal eLevelVal);
	
	// 得到IO输入口状态
	e_PIOLevelVal GetInputState(e_PIOInputState eInputState);
	
	// 得到系统工作状态
	e_PIOLevelVal GetSystemStatus(e_PIOSystemStatus eSystemStatus,e_PIOSystemStateBit eStateBit);
	
	// 得到触发控制行数
	int GetTriggerLines(e_PIOTriggerTable eTriggerTable,e_PIOTriggerPosition eTriggerPosition);
	
	// 设置触发控制行数
	void SetTriggerLines(e_PIOTriggerTable eTriggerTable,e_PIOTriggerPosition eTriggerPosition, int iTriggerLines);
	
	// 使能中断
	void EnableInterrupt();
	
	// 终止中断
	void DisableInterrupt();
	
	// 设置中断事件对象
	void SetInterruptEvent();
	
	// 等待终端事件对象
	e_InterruptReturnVal WaitInterruptEvent(int iWaitOutTime);	
	
	// 调用参数对话框
	void CallParamDialog();

	BOOL SetSystemParam0Val(int iParam0);

	//读写系统寄存器
	BOOL SetSystemRegisterVal(e_PIOSystemRegister eSystemRegister,int iparamval);
	int GetSystemRegisterVal(e_PIOSystemRegister eSystempRegister);
	
private:
	CPIO24B* m_pIOCard;
};


#endif// !defined(DHPIO24B_h_)