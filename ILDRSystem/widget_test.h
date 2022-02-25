#ifndef TESTWIDGET_H
#define TESTWIDGET_H

#include <QWidget>
#include <QTimer>
#include "widget_errortype.h"
#include "widget_camera.h"
#include "ui_widget_test.h"
#include "camerasatuslabel.h"

class OptButtonGroup;

class WidgetTest : public QWidget
{
	Q_OBJECT

public:

	WidgetTest(QWidget *parent = 0);
	~WidgetTest();
	Ui::WidgetTest ui;

	Widget_ErrorType *widget_ErrorType;
	Widget_Camera *widget_Camera;
public:
	void init();
	void initEquipAlarmTablewidget();
	void initWidgetName();
	bool leaveWidget();
	void updateIOCardParam();
	void getIOCardParam(int);
//	void InitIOCardInfo();
	void initInformation();
signals:
	void signals_changeStatisMode(int iStatisMode,int iMinute,int iStatisNumber);
	void signals_ReadDistance1();
	void signals_sendAlarm(int warningType, QString warningInfo);
	void signals_ShowWarning(int , QString );
	void signal_UsualSend(int);
public slots:
	void slots_intoWidget();
	void slots_SaveLoginHoldTime();
	void slots_ChoseCamera();
	void slots_ChoseErrorType();
	void slots_OKSave();
	void slots_OKCameraSurveillance();
	void slots_updateAlert(int);
	void slots_Cancel();
	void slots_KickModeChanged(int iMode);

	void slots_SaveModeChanged(int index);
	void slots_readDelay();
	void slots_setToCard();
	void slots_setToFile();
	void slots_advance1();

	void slots_choseAllCamera();
	void slots_choseNoneCamera();
	void slots_choseAllErrorType();
	void slots_choseNoneErrorType();

	void slots_updateIOcardCounter();
	void slots_ifCheckShowImage();
	void slots_CameraOffAlarm();
	void SetCameraMonitorStatus();
	void slots_EquipAlarmCheckBox(bool);
	void slots_EquipAlarmInfoShowbtn(bool);
	void slots_EquipAlarmSave();
	void slots_EquipAlarmClear();
	void slots_SetEquipAlarmSatus(int,bool);
    void slots_setKickModeEnable(int cardSN, bool en);
	
    void on_cbtnKickMark_clicked(bool);
    void on_btnSaveKickParam_clicked();
    void on_cbtnManageList_clicked(bool);
    void on_btnIDAdd_clicked();
    void on_btnCaviAdd_clicked();
    void slots_ChangedSelection();
    void slots_reloadKickParams();
    void slots_clearKickList();
private:
	int iSaveMode;
	int iLastSaveMode;
	int iSaveImgCount;
	int iLastSaveImgCount;
	
	int iChoseCamera;
	int iChoseErrorType;
	//��ʱ
	int m_nDelay1[IOCard_MAX_COUNT];
	int m_nDelay2[IOCard_MAX_COUNT];
	int m_nDelay3[IOCard_MAX_COUNT];
	int m_nDelay4[IOCard_MAX_COUNT];
	int m_nDelay5[IOCard_MAX_COUNT];
	int m_nDelay6[IOCard_MAX_COUNT];

	int m_nKickDelay[IOCard_MAX_COUNT];     //�޷���ʱ
	int m_nKickWidth[IOCard_MAX_COUNT];     //�޷�����
	int m_nSampleDelay[IOCard_MAX_COUNT];   //�����ʱ
	int m_nSampleWidth[IOCard_MAX_COUNT];   //��������

	QTimer* timerUpdateIOCardCounter;
	QTimer* CameraOffAlarm;

	QList<CameraStatusLabel*> m_EquipAlarmStatusList;
    //���ڿ��ƿ��޷�ģʽ
    QButtonGroup* buttonGroup;
    QButtonGroup* buttonGroup2;

    OptButtonGroup* markedIDGroup;      //���ID�ĸ�ѡ���б�
    OptButtonGroup* markedCaviGroup;    //����޷�����ŵĸ�ѡ���б�

public:
	int iIOCardOffSet[IOCard_MAX_COUNT];  //��Ʒ���
	int ifshowImage;
    //����������߼���
	bool CameraOFF[CAMERA_MAX_COUNT];        //��ǰ����Ƿ����
    int LastImageNO[CAMERA_MAX_COUNT];      //�ϴε����ͼ���
    int lastGrabCount[CAMERA_MAX_COUNT];    //�ϴε�����ص�����
};

#endif // TESTWIDGET_H