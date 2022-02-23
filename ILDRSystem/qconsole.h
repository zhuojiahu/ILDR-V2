#ifndef QCONSOLE_H
#define QCONSOLE_H

#include <QMainWindow>
#include "ui_qconsole.h"
#include <QString>
#include "CIOCard.h"
#include <QMessageBox>
#include <QTimer>
#include "widget_plc.h"
//#include "widgetwarning.h"
#include <QFile>
#include <QCloseEvent>
enum StateEnum
{
	SENDDATA,
	CONNECT,
	PLCWARN,
	STARTDETECT
};
enum UnitEnum
{
	LEADING, //前端
	CLAMPING,//夹持
	BACKING, //后端
};
struct MyStruct
{
	StateEnum nState;	//发送状态标志位
	UnitEnum nUnit;		//发送的设备标志位
	int nCount;			//过检总数
	int nFail;			//踢废总数
	int nPLCStatus;		//PLC报警信息
	int nStartDecect;	//开始检测  0 - 停止检测 ， 1 - 开始检测
	//QString nTemp;//前端的多余变量
};

struct IpStruct
{
	QString ipAddress; //ip地址
	int startTime; //时间，用于判断是否网络连接中断
	bool nstate; //目前连接状态
};

struct NingInfo
{
	int m_checkedNum;
	int m_passNum;
	int m_failureNum;
	int m_checkedNum2;
	NingInfo()
	{
		m_checkedNum = 0;
		m_passNum = 0;
		m_failureNum = 0;
		m_checkedNum2 = 0;
	}
};
class QConsole : public QMainWindow
{
	Q_OBJECT

public:
	QConsole(int,QWidget *parent = 0);
	~QConsole();
	void closeEvent(QCloseEvent *event);
	bool WritePrivateProfileQString(QString strSectionName, QString strKeyName, QString strValue, QString strFileName);
	void SendDataToSever();
public:
	Ui::QConsoleClass ui;
signals:
	void signal_HideWidget();
public slots:
	void slot_SaveCard();
	void slot_OpenCard();
	void slot_OpenPLC();
	void slot_ResetIoCard();
	void slot_readIoCard();
public:
	CIOCard* m_vIOCard;
	Widget_PLC * m_plc;
	//WidgetWarning* m_warning;
	QTimer* nReadIOcard;
	int nType;
public:
	NingInfo nInfo;
};

#endif // QCONSOLE_H
