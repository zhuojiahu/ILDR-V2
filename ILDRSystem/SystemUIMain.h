#ifndef ILDRSYSTEM_H
#define ILDRSYSTEM_H

#include <QtGui/QMainWindow>
#include <qlabel.h>

#include <QWidget>
#include <QDesktopWidget>
#include <QTranslator>
#include <QIcon>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QSplitter>
#include <QMessageBox>
#include <QFileSystemWatcher>
#include <QTcpSocket>
#include <QTcpServer>
#include "widget_title.h"
#include "widget_info.h"
#include "UserManegeWidget.h"
#include "widget_image.h"
#include "Widget_CarveSetting.h"
#include "widget_Management.h"
#include "widget_plc1.h"
#include "widget_test.h"
#include <time.h>
#include "widget_count.h"
#include "CombineResult.h"
#include "ConfigInfo.h"
#include "myQueue.h"
#include "clogFile.h"
#include "DetectThread.h"
#include "CIOCard.h"
#include "cmyerrorlist.h"
#include "stateTool.h"
#include "rled.h"
#include "database.h"
#include <Mmsystem.h>
#pragma comment( lib,"winmm.lib" )

#define DETA_LEN 1

struct ImageSave
{
	QImage* pThat;
	QImage m_Picture;
};
struct MyErrorType
{
	UINT id;
	UINT nType;
	UINT nErrorArea;
	UINT nMoldNo;
};

class WidgetWarning;

class SystemUIMain : public QWidget
{
    Q_OBJECT
public:
    SystemUIMain(QWidget *parent = 0);
    ~SystemUIMain();
protected:
	virtual void paintEvent(QPaintEvent *event);
public:
    void Init();
	void InitImage();//初始化图像
	void CloseCam();//关闭相机
	void InitIOCard();
	void ReleaseIOCard();
	void ReleaseImage();
	void ReleaseAll();//释放资源
	void GrabCallBack(const s_GBSIGNALINFO *SigInfo);//采集回调函数
	QString getVersion(QString strFullName);
	void StartDetectThread();	//开启检测线程
	void initDetectThread();	//开启检测线程
	void ShowCheckSet(int nCamIdx = 0,int signalNumber = 0);
	void writeLogText(QString string,e_SaveLogType eSaveLogType);
	bool changeLanguage(int nLangIdx);
	void InitCamImage(int iCameraNo);
	void StartCamGrab();
	void CarveImage(uchar* pRes,uchar* pTar,int iResWidth,int iResHeight,int iTarX,int iTarY,int iTarWidth,int iTarHeight);
	bool RoAngle(uchar* pRes,uchar* pTar,int iResWidth,int iResHeight,int iAngle);
	bool CheckLicense();
	void MonitorLicense();
	void showAllert();
	static DWORD WINAPI SendDetect1(void*);
	static DWORD WINAPI SendDetect2(void*);
	static DWORD WINAPI SendIOCard(void*);
	static DWORD WINAPI DataCountThread(void*);
	void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void CountDefectIOCard(int iCamera, int ImageNumber, int tmpResult);
signals:
	void signals_intoManagementWidget();
	void signals_intoTestWidget();
	void signals_writeLogText(QString string,e_SaveLogType eSaveLogType);
    void signals_clearTable();
	void signals_clear();
	void signals_HideWarning(int);
    void signals_ConnectSever();
    void signals_ShowWarning(int , QString );
	void signals_updateCount(bool);
	void signals_plcstatus(int);
	void signals_startdetect(bool);
    //操作
    void signals_turnPage(int);
    //设置状态
    void signals_setNaviLockSt(bool);
    void signals_setCurrentPage(int page);
public slots:
	void slots_turnPage(int current_page, int iPara = 0);
	void slots_MessageBoxMainThread(s_MSGBoxInfo msgbox);				//子线程弹出对话框
	void slots_OnBtnStar(bool isRecv = false);
	void slots_OnExit(bool ifyanz=false);
    int slots_deviceType(int i);
    QList<int> slots_getDeviceList(int t);
	void slots_UpdateCoderNumber();
	void slots_updateCameraState(int nCam,int nMode = 0);
	void slots_SetCameraStatus(int nCam,int mode);
	void directoryChanged(QString path);
	void onSocketDataReady();
	void onServerDataReady();
	void ServerNewConnection();
	void onServerConnected(QString IPAddress,bool nState);
	void SendStartDecect(int pStartDetect);
    void slot_ConnectedSock();
    void slots_ShowPLCStatus(int iStatus);
	void slots_UpdateRecordSet();
	void slots_UpdateShiftSet();
	void slots_SaveCountBytime();
	void slots_SaveCountByShift();
	void UpdateCountForShow(bool isFirst);
    void slots_NaviOperation(int op);
    void slots_switchPage(int current_page);
    bool isLock()const;
    bool checkLock();
public:
	//初始化
	void Initialize();
	void InitParameter();//初始化参数
	void ReadIniInformation();
	void LoadParameterAndCam();//加载参数初始化相机
	void InitGrabCard(s_GBINITSTRUCT struGrabCardPara,int index);//初始化采集卡
	void InitCam();//初始化相机
	void SetCarvedCamInfo();
	void SetCombineInfo();
	int InitCheckSet();
	void ReadCorveConfig();
	void initInterface();	//初始化界面
	void initSocket();
	int ReadImageSignal(int nImageNum,int camera=0);
	void SetLanguage(int pLang);
	bool SendDataToSever(int,StateEnum);
    int valueByKickMode(int cardSN, int result)const;
    void SetCardKickMode(int cardSN, int mode);
	void SaveCountInfo();
	void SaveToDatebase();
	void ClearCount(bool isChangeShift = true);

protected:
    void keyPressEvent(QKeyEvent *e);
    
private:
    void initInterface();    //初始化界面
private:
    //ui控件
    QStackedWidget* statked_widget;


    QLabel* lStatus;    //用于显示状态信息
    QLabel* lVersion;   //用于显示程序版本


    //控制参数
    int iLastPage;
    bool bLock;
    
};

#endif // ILDRSYSTEM_H

