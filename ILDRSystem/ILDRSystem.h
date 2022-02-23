#ifndef ILDR_SYS_MAIN_H
#define ILDR_SYS_MAIN_H

#include <time.h>

#include <QDialog>

#include "stateTool.h"
#include "CombineResult.h"
#include "ConfigInfo.h"
#include "myQueue.h"
#include "DetectThread.h"
#include "CIOCard.h"
#include "cmyerrorlist.h"
#include "widget_count.h"

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

class QStackedWidget;
class WidgetWarning;
class QLabel;
class QTimer;
class QTcpSocket;
class QTcpServer;
class WidgetTitle;
class WidgetCarveSetting;
class WidgetCarveSetting;
class WidgetManagement;
class WidgetTest;
class Widget_PLC1;
class WidgetWarning;
class DataBase;
class CameraStatusLabel;
class RLed;

class SysMainUI : public QDialog
{
	Q_OBJECT

public:
	SysMainUI(QWidget *parent = 0, Qt::WFlags flags = 0);
	~SysMainUI();
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
	QString getVersion()const;
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
	/*static DWORD WINAPI SendDetect1(void*);
	static DWORD WINAPI SendDetect2(void*);
	static DWORD WINAPI SendIOCard(void*);
	static DWORD WINAPI DataCountThread(void*);*/
    void CountDefectIOCard(int iCamera, int ImageNumber, int tmpResult);
    bool isLock()const;
    bool checkLock();
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
    //设置状态
    void signals_setNaviLockSt(bool);
    //更新结果和统计数据
    void signals_updateResult(int _nImgNO, QString _id, QDateTime _dt, QString _cavityNum, QDateTime _gmtTime);
    void signals_updateCount(int total, int reject, int read, int intime, int engraved);
    //用于重置当前界面统计数据
    void signals_resetCurrent();
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
	/*void onSocketDataReady();
	void onServerDataReady();
	void ServerNewConnection();
	void onServerConnected(QString IPAddress,bool nState);
	void SendStartDecect(int pStartDetect);
    void slot_ConnectedSock();
    void slots_ShowPLCStatus(int iStatus);*/
	void slots_UpdateRecordSet();
	void slots_UpdateShiftSet();
	void slots_SaveCountBytime();
	void slots_SaveCountByShift();
	void UpdateCountForShow(bool isFirst);

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
	//void initSocket();
	int ReadImageSignal(int camNO);
	void SetLanguage(int pLang);
	//bool SendDataToSever(int,StateEnum);
    int valueByKickMode(int cardSN, int result)const;
    void SetCardKickMode(int cardSN, int mode);
	void SaveCountInfo();
	void SaveToDatebase();
	void ClearCount(bool isChangeShift = true);

protected:
    void keyPressEvent(QKeyEvent *e);
public:
    //主窗体控件
    QStackedWidget *statked_widget;
    WidgetWarning *widget_Warning;          //报警窗体
    QString skin_name;						//主窗口背景图片的名称
    QPixmap skin;
    //状态栏控件
    QList<CameraStatusLabel *> cameraStatus_list;   //相机状态标签
    QLabel *labelCoder;
    QWidget *stateBar;

    //子窗体界面
	WidgetTitle *title_widget;				//标题栏 

    WidgetCarveSetting *widget_operation;   //操作界面
    QWidget *widget_alg;                    //算法设置页面
	WidgetManagement *widget_article;       //模板设置
    WidgetTest *widget_settings;                //系统设置页面
    widget_count *widget_history;           //历史页面
    QWidget* widget_inout;                  //输入输出页面
    QWidget *widget_Alarm;                  //报警页面

public:
	ENavigation m_eCurrentMainPage;
	ENavigation m_eLastMainPage;
	int iLastPage;
	s_Permission sPermission;
    QVector<bool> buttonVisible;  //用于控制吧标题栏是否可见
	//配置信息结构体
	s_ConfigInfo m_sConfigInfo;
	s_SystemInfo m_sSystemInfo;
    s_ErrorInfo m_sErrorInfo;
    QVector<QString> m_vstrPLCInfoType;		//PLC错误类型
	s_GBINITSTRUCT struGrabCardPara[CAMERA_MAX_COUNT];
    QMap<int, int> m_sDeviceType;  //设备ID与设备类型映射关系ID从0开始
	//运行信息结构体
	s_RunningInfo m_sRunningInfo;
	//设备维护报警结构体
	s_RuntimeInfo m_sRuntimeInfo;
	//模号统计
	cMoldNoErrorInfo m_sMoldNoCount,LastMoldNoCount,nTmpMoldNoCount;
	//报表数据
	cErrorInfo nRunInfo,LastRunInfo,nTmpcountData;
	//上一个整点保存的数据
	QDateTime LastTime;
	//TemporaryData LastTimeData;
	//连接服务器
	QTcpSocket * m_tcpSocket;
	QTcpServer *m_tcpServer;
	QTcpSocket *m_tcpClient;
	//相机结构体
	s_RealCamInfo m_sRealCamInfo[CAMERA_MAX_COUNT];
	s_CarvedCamInfo m_sCarvedCamInfo[CAMERA_MAX_COUNT];
	CMyQueue nQueue[CAMERA_MAX_COUNT];	
	CDetectElement m_detectElement[CAMERA_MAX_COUNT];
	QMutex mutexDetectElement[CAMERA_MAX_COUNT];

	CIOCard *m_vIOCard[IOCard_MAX_COUNT];		//IO卡队列
	CMyErrorList m_ErrorList;

	//图像综合相关参数
    CCombineRlt* m_cCombine[IOCard_MAX_COUNT];
	QVector<QRgb> m_vcolorTable;				//生成灰度颜色表
	//算法使用的检测列表
	s_InputCheckerAry CherkerAry;
	//检测线程相关参数
	DetectThread *pdetthread[CAMERA_MAX_COUNT];
	
	BOOL m_bIsThreadDead;			//杀死线程，关闭窗口结束线程
	CBottleCheck m_cBottleCheck[CAMERA_MAX_COUNT];	// 算法类对象 [10/26/2010 GZ]
	CBottleModel m_cBottleModel;// 模板设置对话框 [10/26/2010 GZ]
	CBottleCheck m_cBottleRotate[CAMERA_MAX_COUNT];	
	CBottleCheck m_cBottleStress[CAMERA_MAX_COUNT];	
	CBottleCheck m_cBottleRotateCarve;
	QString sVersion;
	QString SaveDataPath;
	int sLanguage;

    int m_iGrabCounter[CAMERA_MAX_COUNT];//相机回调计数 用于相机掉线监视
    QTimer *timerUpdateCoder;   //用于更新界面统计数据
    QTimer *nConnectTimer;      //用于定时连接服务器
    QTimer *timerSaveCount;     //用于定时保存统计数据
public:
	//TemporaryData LastTimeData;
	ImageSave m_SavePicture[CAMERA_MAX_COUNT];
	int surplusDays;
	HANDLE pHandles[CAMERA_MAX_COUNT];
	MyErrorType nSendData1[256];
    MyErrorType nSendData2[256];

public:
	//涉及网络通信的变量
	char* m_ptr;
	int* nIOCard;
	QList<QByteArray> ncSocketWriteData1;
	QList<QByteArray> ncSocketWriteData2;
	int nCountNumber1;
	int nCountNumber2;
	QMutex nSocketMutex;
	RLed *sockLed1,*sockLed2;

	QList<QByteArray> nDataCount[3];
	QMutex nCountLock;
	int currentShift;
	bool isCnt;

	//QMutex countLocker;

    DataBase* m_Datebase;   //用于记录模号统计数据
private:
    int iLastStatus;
    bool bLock;
};
#endif // ILDR_SYS_MAIN_H

