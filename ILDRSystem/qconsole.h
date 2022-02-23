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
	LEADING, //ǰ��
	CLAMPING,//�г�
	BACKING, //���
};
struct MyStruct
{
	StateEnum nState;	//����״̬��־λ
	UnitEnum nUnit;		//���͵��豸��־λ
	int nCount;			//��������
	int nFail;			//�߷�����
	int nPLCStatus;		//PLC������Ϣ
	int nStartDecect;	//��ʼ���  0 - ֹͣ��� �� 1 - ��ʼ���
	//QString nTemp;//ǰ�˵Ķ������
};

struct IpStruct
{
	QString ipAddress; //ip��ַ
	int startTime; //ʱ�䣬�����ж��Ƿ����������ж�
	bool nstate; //Ŀǰ����״̬
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
