#include "SystemUIMain.h"

#include <QPaintEvent>
#include <QKeyEvent>
#include <QWidget>
#include <QStackedWidget>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

#include "qt_windows.h"
#pragma comment(lib,"version.lib")

#include "common.h"
#include "uitypes.h"
#include "NavigationWidget.h"
#include "cpassworddlg.h"
#include "uiOperation.h"


#include <QTextCodec>
#include <QDateTime>
#include <QPropertyAnimation>
#include <QProcess>
#include <QLabel>
#include <QDesktopWidget>
#include <QTranslator>
#include <QIcon>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QSplitter>
#include <QMessageBox>

#include "common.h"
#include "DetectThread.h"
#include "clogFile.h"
#include "CIOCard.h"
#include "ThickCombine.h"
#include "CHRThick.h"
#include "SixianThick.h"
#include "StilThick.h"
#include "SimulationThick.h"
#include "CTReadThickness.h"
#include "CUdpsocket.h"

#include "uitypes.h"
#include "widget_title.h"
#include "widget_image.h"
#include "widget_info.h"
#include "UserManegeWidget.h"
#include "widget_FinishBottom.h"
#include "widget_Ovality.h"
#include "widget_Management.h"
#include "widget_test.h"
#include "widget_count.h"
#include "widgetwarning.h"
#include "Widget_LinearCamera.h"
#include "thickness.h"

#include "Counter/datacenter.h"
#ifndef NO_MOULD_COUNT
#ifdef _DEBUG
#pragma comment(lib, "DataCenter_d.lib")
#else
#pragma comment(lib, "DataCenter.lib")
#endif
#endif
SystemUIMain * pMainFrm;
QString appPath;  //更新路径


typedef struct tagWNDINFO
{
    DWORD processid;
    HWND hWnd;
} WNDINFO, *LPWNDINFO;

bool FindProcessFromName(QString ProcessName)
{
    QString str1;

    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0) ;
    PROCESSENTRY32 pInfo;//进程信息
    pInfo.dwSize = sizeof(pInfo);

    Process32First(hSnapShot, &pInfo);
    do
    {
        str1 = (QString::fromUtf16(reinterpret_cast<const unsigned short *>(pInfo.szExeFile)));
        if (str1 == ProcessName)
        {
            return true;
        }
    } while(Process32Next(hSnapShot, &pInfo) );
    return false;
}

bool FindProcessIDFromName(QString ProcessName , DWORD &pID)
{
    QString str1;

    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0) ;
    PROCESSENTRY32 pInfo;//进程信息
    pInfo.dwSize = sizeof(pInfo);

    Process32First(hSnapShot, &pInfo);
    do
    {
        str1 = (QString::fromUtf16(reinterpret_cast<const unsigned short *>(pInfo.szExeFile)));
        if (str1 == ProcessName)
        {
            DWORD dwProcessID = pInfo.th32ProcessID;
            pID = dwProcessID;

            return true;
        }
    } while(Process32Next(hSnapShot, &pInfo) );
    return false;
}
BOOL IsMainWindow(HWND handle)
{
    return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
    WNDINFO& data = *(WNDINFO*)lParam;
    unsigned long process_id = 0;
    GetWindowThreadProcessId(handle, &process_id);
    if (data.processid != process_id || !IsMainWindow(handle)) {
        return TRUE;
    }
    data.hWnd = handle;
    return FALSE;
}

HWND FindMainWindow(unsigned long process_id)
{
    WNDINFO data;
    data.processid = process_id;
    data.hWnd = 0;
    EnumWindows(EnumWindowsCallback, (LPARAM)&data);
    return data.hWnd;
}

QString GetLocalPogramVersion()
{
    TCHAR pFilePath[MAX_PATH] = {0};     
    DWORD dwRet = GetModuleFileName(NULL, pFilePath, MAX_PATH); 
    if(dwRet == 0)
    {
        return "";
    }
    //获取版本信息大小
    DWORD dwSize = GetFileVersionInfoSize(pFilePath,NULL); 
    if (dwSize == 0) 
    {
        return "";
    }

    TCHAR *pBuf = (TCHAR *)malloc(dwSize + 1); 
    memset(pBuf, 0, dwSize + 1);

    //获取版本信息
    DWORD dwRtn = GetFileVersionInfo(pFilePath,NULL, dwSize, pBuf); 
    if(dwRtn == 0) 
    {
        return "";
    }

    LPVOID lpBuffer = NULL;
    UINT uLen = 0;

    dwRtn = VerQueryValue(pBuf, TEXT("\\StringFileInfo\\080404b0\\FileVersion"), &lpBuffer, &uLen); 
    if(dwRtn == 0) 
    {
        free(pBuf);
        return "";	 
    }

    int iLen = 2*wcslen((TCHAR*)lpBuffer);//CString,TCHAR汉字算一个字符，因此不用普通计算长度   
    char* chRtn = (char *)malloc(iLen+1); 
    memset(chRtn,0,iLen+1);
    wcstombs(chRtn,(TCHAR*)lpBuffer,iLen+1);//转换成功返回为非负值

    QString strVersion(chRtn);
    free(chRtn);
    free(pBuf);
    return strVersion;
}    

SystemUIMain::SystemUIMain(QWidget *parent)
    : QWidget(parent)
{
    buttonVisible = QVector<bool>(12, true).toList();
    pThis=NULL;
    nTestDevID=-1;
    pMainFrm = this;
    for (int i=0;i<CAMERA_MAX_COUNT;i++)
    {
        m_queue[i].listDetect.clear();
        m_iDetectStep[i] = 0;
        m_iDetectSignalNo[i] = 0;
        m_iGrabCounter[i] = 0;
        m_iImgGrabCounter[i] = 0;
        pdetthread[i] = NULL;
        blostImage[i] = false;
        m_SavePicture[i].pThat=NULL;
    }

    m_vcolorTable.clear();
    for (int i = 0; i < 256; i++)  
    {  
        m_vcolorTable.append(qRgb(i, i, i)); 
    }
    imgTime = 0;
    uniqueFlag = false;
    CherkerAry.pCheckerlist=NULL;
    sPermission.iFinBot = 1;
    sPermission.iOva = 1;
    sPermission.iThickness = 1;
    sPermission.iLCamReadMold = 1;

    record.id = "VEXI_DATIA";
    record.dt_start = QDateTime::currentDateTime();
    dbInit = false;
    dbCenter = nullptr;
    m_udpsocket = nullptr;
    initInterface();
    Init();
}

SystemUIMain::~SystemUIMain()
{
    CLogFile::write(tr("Start Exit Program..."),OperationLog);
    //停止检测
    timerUpdateCoder->stop();
    ReleaseAll();
    
    CLogFile::write(tr("Release Over"),OperationLog);
}
void SystemUIMain::closeWidget()
{
    close();
}
void SystemUIMain::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) 
    {
        return;
    }
}
void SystemUIMain::Init()
{
    m_sRunningInfo.m_iPermission = 0;
    CLogFile::write(tr("InitParameter!"),OperationLog);
    InitParameter();
    CLogFile::write(tr("ReadIniInformation!"),OperationLog);
    ReadIniInformation();
//    SetCarvedCamInfo();
    CLogFile::write(tr("Initialize!"),OperationLog);
    Initialize();
    CLogFile::write(tr("initInterface!"),OperationLog);
    initInterface();
    CLogFile::write(tr("startdetect!"),OperationLog);

    StartDetectThread();
    StartCamGrab();

    CLogFile::write(tr("Enter system!"),OperationLog);
    InitLastData();
    //设置初始状态
    bLock = true;
    emit signals_setNaviLockSt(bLock);
    slots_NaviOperation(ENaviPageOperation);
}
//初始化
void SystemUIMain::Initialize()
{
    CLogFile::write(tr("LoadParameterAndCam!"),OperationLog);
    LoadParameterAndCam();

    CLogFile::write(tr("InitImage!"),OperationLog);
    InitImage();    //初始化图像
    SetCombineInfo(); //初始化结果综合
    CLogFile::write(tr("InitIOCard!"),OperationLog);
    InitIOCard();
    CLogFile::write(tr("InitThickness!"),OperationLog);
    InitThickness();
    CLogFile::write(tr("InitCheckSet!"),OperationLog);
    InitCheckSet();
    initDetectThread();
    initDataCenter();
}
//采集回调函数
void WINAPI GlobalGrabOverCallback (const s_GBSIGNALINFO* SigInfo)
{
    if (SigInfo && SigInfo->Context)
    {
        SystemUIMain* pSystemUIMain = (SystemUIMain*) SigInfo->Context;
        pSystemUIMain->GrabCallBack(SigInfo);
    }
}
//采集回调函数

void SystemUIMain::GrabCallBack(const s_GBSIGNALINFO *SigInfo)
{
    int iRealCameraSN = SigInfo->nGrabberSN;// 读取相机序号 [7/13/2010 GZ]
    if (iRealCameraSN == -1 || !m_sRealCamInfo[iRealCameraSN].m_bGrabIsStart || !m_sRunningInfo.m_bCheck)
    {
        return;
    }
    
    if(SigInfo->nErrorCode != GBOK)
    {
        CLogFile::write(QString("GrabCallBack(131)：Camera:%1 have Residual frame:%2").arg(iRealCameraSN+1).arg(SigInfo->strDescription),CheckLog);
        return;
    }
    m_iGrabCounter[iRealCameraSN]++;
    //**********************获得机器号与误触发判断*******************//
    CMachineSignal m_cMachioneSignalCurrent;//获得机器信号
    if (!m_sSystemInfo.m_bIsIOCardOK)//生成机器号
    {
        if (-1 == m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast)
        {
            m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast++;
        }
        if (m_sRealCamInfo[iRealCameraSN].m_iImageGrabbedNo >= m_sRealCamInfo[iRealCameraSN].m_iImageTargetNo)
        {
            m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast++;
            m_sRealCamInfo[iRealCameraSN].m_iImageGrabbedNo = 0;
        }
        m_sRealCamInfo[iRealCameraSN].m_iImageGrabbedNo++;
        if (m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast >= 256)
        {
            m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast = 0;
        }
        static QList<int> imgsn_cnt;
        static QMutex mx;
        //模拟计数
        if( nTestDevID == -1)
        {
            QMutexLocker lk(&mx);
            if(!imgsn_cnt.contains(m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast))
            {
                imgsn_cnt.push_back(m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast);
                m_sRunningInfo.m_checkedNum++;
                if(imgsn_cnt.size() > 10)
                {
                    imgsn_cnt.pop_front();
                }
            }
        }
    }
    else if (m_sSystemInfo.m_bIsIOCardOK && m_sRealCamInfo[iRealCameraSN].m_bGrabIsTrigger)// 读机器信号
    {
        m_cMachioneSignalCurrent.m_iImageCount = ReadDeviceImageSignal(iRealCameraSN);
        
        if (m_cMachioneSignalCurrent.m_iImageCount != m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast)
        {
            m_sRealCamInfo[iRealCameraSN].m_iImageGrabbedNo = 1;
            m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast = m_cMachioneSignalCurrent.m_iImageCount;
            /* if(iRealCameraSN == 4)
            CLogFile::write(QString("GrabCallBack：Camera:%1 Image:%2").arg(iRealCameraSN+1).arg(m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast),DebugLog);*/
        }
        else
        {
            if(m_sRealCamInfo[iRealCameraSN].m_iImageGrabbedNo == m_sRealCamInfo[iRealCameraSN].m_iImageTargetNo)
            {
                m_sRealCamInfo[iRealCameraSN].m_iImageGrabbedNo = 0;
            }
            m_sRealCamInfo[iRealCameraSN].m_iImageGrabbedNo++;
        }
    }

    try
    {
        //**************************************************//
        //******************采集****************************//
        // 得到图像缓冲区地址
        uchar* pImageBuffer = NULL;
        uchar* pImageRo = NULL;
        int nAddr = 0;
        int nWidth, nHeight, nBitCount;
        m_mutexmGrab.lock();
        m_sRealCamInfo[iRealCameraSN].m_pGrabber->GetParamInt(GBImageBufferAddr, nAddr);
#ifndef _WIN64
        pImageBuffer = (uchar*)nAddr;
#else
		int nAddrH = 0;
        m_sRealCamInfo[iRealCameraSN].m_pGrabber->GetParamInt(GBImageBufferAddr2, nAddrH);
		INT64 addr = nAddrH;
		addr = (addr << 32) + nAddr;
		pImageBuffer = (uchar*)addr;
#endif
        m_sRealCamInfo[iRealCameraSN].m_pGrabber->GetParamInt(GBImageWidth, nWidth);
        m_sRealCamInfo[iRealCameraSN].m_pGrabber->GetParamInt(GBImageHeight, nHeight);
        m_sRealCamInfo[iRealCameraSN].m_pGrabber->GetParamInt(GBImagePixelSize, nBitCount);
        m_mutexmGrab.unlock();
        nBitCount = 8;
        long lImageSize = nWidth * nHeight * (nBitCount/8);
        //保存相机原始图像
        try
        {
            if (90 == m_sRealCamInfo[iRealCameraSN].m_iImageRoAngle || 180 == m_sRealCamInfo[iRealCameraSN].m_iImageRoAngle|| 270 == m_sRealCamInfo[iRealCameraSN].m_iImageRoAngle)
            {
                RoAngle(pImageBuffer,m_sRealCamInfo[iRealCameraSN].m_pRealImage->bits(),\
                    nWidth,    nHeight,m_sRealCamInfo[iRealCameraSN].m_iImageRoAngle);
            } 
            else
            {
                memcpy(m_sRealCamInfo[iRealCameraSN].m_pRealImage->bits(),pImageBuffer,lImageSize);//pixmapShow = QPixmap::fromImage(tempImg);
            }
        }
        catch (...)
        {
            CLogFile::write(QString("GrabCallBack(131)：Camera:%1 copy image data exception").arg(iRealCameraSN+1),CheckLog);
        }
        CGrabElement *pGrabElement = NULL;
        
        if(m_queue[iRealCameraSN].listGrab.size()<=0)
        {
            return;
        }
        
        m_queue[iRealCameraSN].mLocker.lock();
        pGrabElement = (CGrabElement *) m_queue[iRealCameraSN].listGrab.first();
        m_queue[iRealCameraSN].listGrab.removeFirst();
        m_queue[iRealCameraSN].mLocker.unlock();
        if (pGrabElement != NULL)
        {    
            pGrabElement->nCamSN = iRealCameraSN;
            pGrabElement->nCheckRet = FALSE;
            pGrabElement->cMachineInfoOfAllElement = m_cMachioneSignalCurrent;
            pGrabElement->cErrorParaList.clear();
            lImageSize = m_sCarvedCamInfo[iRealCameraSN].m_iImageWidth * m_sCarvedCamInfo[iRealCameraSN].m_iImageHeight * (nBitCount/8);
            if (lImageSize != pGrabElement->myImage->byteCount())
            {
                delete []pGrabElement->sImgLocInfo.sXldPoint.nColsAry;
                delete []pGrabElement->sImgLocInfo.sXldPoint.nRowsAry;
                delete pGrabElement->myImage;
                //m_mutexmCarve.unlock();
                CLogFile::write(tr("get %1 failed").arg(iRealCameraSN),AbnormityLog);
                return;
            }
            //m_iImgGrabCounter[iRealCameraSN]++;
                
            m_mutexmCarve.lock();
            if(deviceType(iRealCameraSN) != int(EDTCameraLineMatrix))
            {
                //剪切图像
                CarveImage(m_sRealCamInfo[iRealCameraSN].m_pRealImage->bits(),m_sCarvedCamInfo[iRealCameraSN].m_pGrabTemp,\
                    m_sRealCamInfo[iRealCameraSN].m_iImageWidth,m_sRealCamInfo[iRealCameraSN].m_iImageHeight, m_sCarvedCamInfo[iRealCameraSN].i_ImageX,m_sCarvedCamInfo[iRealCameraSN].i_ImageY,\
                    m_sCarvedCamInfo[iRealCameraSN].m_iImageWidth,m_sCarvedCamInfo[iRealCameraSN].m_iImageHeight);
                memcpy(pGrabElement->myImage->bits(), m_sCarvedCamInfo[iRealCameraSN].m_pGrabTemp, \
                    m_sCarvedCamInfo[iRealCameraSN].m_iImageWidth*m_sCarvedCamInfo[iRealCameraSN].m_iImageHeight);
            }
            else
            {
                //线阵相机图片不裁剪
                if(m_sSystemInfo.m_bMirror)
                {
                    QImage img = m_sRealCamInfo[iRealCameraSN].m_pRealImage->mirrored(true,false);
                    memcpy(m_sRealCamInfo[iRealCameraSN].m_pRealImage->bits(), img.bits(), \
                        m_sRealCamInfo[iRealCameraSN].m_iImageWidth*m_sRealCamInfo[iRealCameraSN].m_iImageHeight);
                }
                memcpy(m_sCarvedCamInfo[iRealCameraSN].m_pGrabTemp, m_sRealCamInfo[iRealCameraSN].m_pRealImage->bits(), \
                    m_sCarvedCamInfo[iRealCameraSN].m_iImageWidth*m_sCarvedCamInfo[iRealCameraSN].m_iImageHeight);
                memcpy(pGrabElement->myImage->bits(), m_sRealCamInfo[iRealCameraSN].m_pRealImage->bits(), \
                    m_sRealCamInfo[iRealCameraSN].m_iImageWidth*m_sRealCamInfo[iRealCameraSN].m_iImageHeight);

            }
            m_mutexmCarve.unlock();

            //将图像数据填人到元素队列中
            pGrabElement->bHaveImage=TRUE;
            pGrabElement->nImgSN = m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast;
            pGrabElement->nGrabImageCount = m_sRealCamInfo[iRealCameraSN].m_iImageGrabbedNo;
            pGrabElement->nSignalNo = m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast;

            if (m_queue[iRealCameraSN].InitID == pGrabElement->initID)
            {
                m_detectElement[iRealCameraSN].iSignalNoNormal = pGrabElement->nSignalNo; 
                m_detectElement[iRealCameraSN].ImageNormal = pGrabElement;
                m_detectElement[iRealCameraSN].iCameraNormal = iRealCameraSN;
                m_queue[iRealCameraSN].mDetectLocker.lock();
                m_queue[iRealCameraSN].listDetect.append(m_detectElement[iRealCameraSN]);
                m_queue[iRealCameraSN].mDetectLocker.unlock();
            }
            else
            {
                delete pGrabElement;
            }
        }
    }
    catch (...)
    {
    }
}

//配置初始化信息
void SystemUIMain::InitParameter()
{
    // 注册s_MSGBoxInfo至元对象系统,否则s_MSGBoxInfo,s_ImgWidgetShowInfo，s_StatisticsInfo无法作为参数进行传递
    qRegisterMetaType<s_MSGBoxInfo>("s_MSGBoxInfo"); 
    qRegisterMetaType<e_SaveLogType>("e_SaveLogType");  
    qRegisterMetaType<QList<QRect>>("QList<QRect>");  

    //初始化路径
    QString path = QApplication::applicationFilePath();  
    appPath = path.left(path.findRev("/")+1);
    m_sConfigInfo.m_strAppPath = appPath;
    //配置文件在run目录中位置
    m_sConfigInfo.m_strConfigPath = QString("%1%2").arg(ConfigDir).arg(ConfigFileName);
    m_sConfigInfo.m_strDataPath = QString("%1%2").arg(ConfigDir).arg( DATAFILE);
    m_sConfigInfo.m_sAlgFilePath = ModelConfigDIR;// 算法路径 [10/26/2010 GZ]    

    //配置文件绝对路径
    m_sConfigInfo.m_strConfigPath = appPath + m_sConfigInfo.m_strConfigPath;
    m_sConfigInfo.m_strDataPath = appPath + m_sConfigInfo.m_strDataPath;
    m_sConfigInfo.m_strErrorTypePath = appPath + m_sConfigInfo.m_strErrorTypePath;
    m_sConfigInfo.m_sAlgFilePath = appPath + m_sConfigInfo.m_sAlgFilePath;

    SaveDataPath = appPath + LastDataFile;
    //初始化相机参数
    for (int i = 0;i<CAMERA_MAX_COUNT;i++)
    {
        m_bSaveImage[i] = FALSE;
        m_bShowImage[i] = TRUE;
        m_sRealCamInfo[i].m_bGrabIsStart = FALSE;
    }
    connect(this,SIGNAL(signals_MessageBoxMainThread(s_MSGBoxInfo)),this,SLOT(slots_MessageBoxMainThread(s_MSGBoxInfo)));
}
//读取配置信息
void SystemUIMain::ReadIniInformation()
{
    QSettings iniset(m_sConfigInfo.m_strConfigPath,QSettings::IniFormat);
    iniset.setIniCodec(QTextCodec::codecForName("GBK"));

    m_sSystemInfo.nLanguage = iniset.value("/system/Language",0).toInt();    //语言设置
    if(m_sSystemInfo.nLanguage == 0)
    {//中文
        m_sConfigInfo.m_strErrorTypePath = QString("%1%2%3").arg(ConfigDir).arg(ErrorDefFileName).arg(ConfigFileSuffix);
        m_sConfigInfo.m_strWordsPath = QString("%1%2_zh%3").arg(ConfigDir).arg(TranslationWords).arg(ConfigFileSuffix);
    }
    else
    {
        m_sConfigInfo.m_strErrorTypePath = QString("%1%2-en").arg(ConfigDir).arg(ErrorDefFileName).arg(ConfigFileSuffix);
        m_sConfigInfo.m_strWordsPath = QString("%1%2_en%3").arg(ConfigDir).arg(TranslationWords).arg(ConfigFileSuffix);
    }

    QSettings erroriniset(m_sConfigInfo.m_strErrorTypePath,QSettings::IniFormat);
    erroriniset.setIniCodec(QTextCodec::codecForName("GBK"));
    QSettings iniDataSet(m_sConfigInfo.m_strDataPath,QSettings::IniFormat);
    iniDataSet.setIniCodec(QTextCodec::codecForName("GBK"));

    QString strSession;
    strSession = QString("/ErrorType/total");
    m_sErrorInfo.m_iErrorTypeCount = erroriniset.value(strSession,0).toInt();

    for (int i=0;i<=m_sErrorInfo.m_iErrorTypeCount;i++)
    {
        if (0 == i)
        {
            m_sErrorInfo.m_bErrorType[i] = false;
            m_sErrorInfo.m_vstrErrorType.append(tr("Good"));//.toLatin1().data()));
        }
        else
        {
            m_sErrorInfo.m_bErrorType[i] = true;
            strSession = QString("/ErrorType/%1").arg(i);
            QString err = erroriniset.value(strSession,"NULL").toString();
            m_sErrorInfo.m_vstrErrorType.append(err);//.toLatin1().data()));
            m_sErrorInfo.m_cErrorReject.iErrorCountByType[i] = 0;
        }
        strSession = QString("/TrackErrorType/IsTrack%1").arg(i);
        m_sSystemInfo.m_iIsTrackErrorType[i]= iniset.value(strSession,0).toInt();    //是否报警统计的错误类型
        strSession = QString("/TrackErrorType/MaxRate%1").arg(i);
        m_sSystemInfo.m_iTrackAlertRateMax[i]= iniset.value(strSession,10).toInt();    //是否报警统计的错误类型
        strSession = QString("/TrackErrorType/MinRate%1").arg(i);
        m_sSystemInfo.m_iTrackAlertRateMin[i]= iniset.value(strSession,0).toInt();    //是否报警统计的错误类型
    }
    m_sErrorInfo.m_vstrErrorType.append(tr("Other"));//.toLatin1().data()));

    //读取系统参数
    m_sRunningInfo.m_failureNumFromIOcard = iniDataSet.value("/system/failureNum",0).toInt();    
    m_sRunningInfo.m_checkedNum = iniDataSet.value("/system/checkedNum",0).toInt(); 
    m_sRunningInfo.nModelCheckedCount = iniDataSet.value("/system/MouldCount",0).toInt(); 

    m_sSystemInfo.nLoginHoldTime = iniset.value("/system/nLoginHoldTime",10).toInt();    //是否报警统计
    m_sSystemInfo.m_strWindowTitle = QObject::tr("Glass Bottle Detect System");//读取系统标题
    m_sSystemInfo.m_iSimulate = iniset.value("/system/m_iSimulate",0).toInt();
    m_sSystemInfo.m_bDebugMode = iniset.value("/system/DebugMode",0).toInt();    //读取是否debug
    m_sSystemInfo.m_iSystemType = e_SystemType(iniset.value("/system/systemType",0).toInt());    //读取系统类型
    m_sSystemInfo.m_iSystemType = VexiFinishButtom;//当前软件只作为联合软件使用
    m_sSystemInfo.m_bIsIOCardOK=iniset.value("/system/isUseIOCard",1).toInt();    //是否使用IO卡
    m_sSystemInfo.m_bIsStopNeedPermission=iniset.value("/system/IsStopPermission",0).toBool();    //是否使用IO卡
    m_sSystemInfo.iIOCardCount=iniset.value("/system/iIOCardCount",1).toInt();    //读取IO卡个数
    m_sSystemInfo.iRealCamCount = iniset.value("/GarbCardParameter/DeviceNum",2).toInt();    //真实相机个数
    m_sSystemInfo.m_bIsCarve = iniset.value("/system/IsCarve",0).toInt();    //获取是否切割设置
    m_sSystemInfo.m_bIsTest = iniset.value("/system/IsTest",0).toInt();//是否是测试模式
    m_sSystemInfo.iIsButtomStress = iniset.value("/system/IsButtomStree",0).toInt();//是否有瓶底应力
    m_sSystemInfo.iIsSaveCountInfoByTime = iniset.value("/system/IsSaveCountInfoByTime",0).toInt();//是否保存指定时间段内的统计信息
    m_sSystemInfo.iIsSample = iniset.value("/system/IsSample",0).toInt();//是否取样
    m_sSystemInfo.iIsCameraCount = iniset.value("/system/IsCameraCount",0).toInt();//是否统计各相机踢废率
    m_sSystemInfo.LastModelName = iniset.value("/system/LastModelName","default").toString();    //读取上次使用模板
    m_sSystemInfo.m_iIsTrackStatistics = iniset.value("/system/isTrackStatistics",0).toInt();    //是否报警统计
    m_sSystemInfo.m_iTrackNumber = iniset.value("/system/TrackNumber",1000).toInt();    //报警统计个数
    m_sSystemInfo.m_NoKickIfNoFind = iniset.value("/system/NoKickIfNoFind",0).toInt();    //报警类型
    m_sSystemInfo.m_NoKickIfROIFail = iniset.value("/system/NoKickIfROIFail",0).toInt();    //报警类型    
    m_sSystemInfo.m_iTwoImagePage = iniset.value("/system/twoImagePage",1).toInt();    //是否两页显示图像
    m_sSystemInfo.m_iImageStyle = iniset.value("/system/ImageStyle",0).toInt();    //图像横向排布还是上下排布
    m_sSystemInfo.m_iImageStretch = iniset.value("/system/ImageStretch",1).toInt();    //图像横向排布还是上下排布
    m_sSystemInfo.m_iSaveNormalErrorImageByTime = iniset.value("/system/SaveNormalErrorImageByTime",0).toInt();    
    m_sSystemInfo.m_iSaveStressErrorImageByTime = iniset.value("/system/SaveStressErrorImageByTime",0).toInt();    
    m_sSystemInfo.m_iStopOnConveyorStoped = iniset.value("/system/stopCheckWhenConveyorStoped",0).toBool();    //输送带停止是否停止检测
    m_sSystemInfo.fPressScale = iniset.value("/system/fPressScale",1).toDouble();    //瓶身应力增强系数
    m_sSystemInfo.fBasePressScale = iniset.value("/system/fBasePressScale",1).toDouble();    //瓶底应力增强系数
    m_sSystemInfo.m_strModelName = m_sSystemInfo.LastModelName;
    m_sSystemInfo.bSaveRecord = iniset.value("/system/bSaveRecord",1).toBool();
    m_sSystemInfo.iSaveRecordInterval = iniset.value("/system/iSaveRecordInterval",60).toInt();
    m_sSystemInfo.bAutoSetZero = iniset.value("/system/bAutoSetZero",1).toBool();
    m_sSystemInfo.iIsSampleAndAlamConflict = iniset.value("/system/IsSampleAndAlamConflict",0).toInt();
    m_sSystemInfo.m_iIs3Sensor = iniset.value("/system/Is3Sensor",0).toInt();
    m_sSystemInfo.m_bMirror = iniset.value("/system/bMirror",false).toBool();
    m_sSystemInfo.m_nLCamShowRatio = iniset.value("/system/nShowRatio",-1).toInt();
    m_sSystemInfo.isThickNessEnable = iniset.value("/system/ThickNessEnable",false).toBool();
    sPermission.iLCamReadMold = true;
    buttonVisible[PGThicknessPage] = m_sSystemInfo.isThickNessEnable;

    m_sSystemInfo.bCameraOffLineSurveillance = iniset.value("/system/bCameraOffLineSurveillance",1).toBool();    
    m_sSystemInfo.bVEXIDoubleReject = iniset.value("/system/bVEXIDoubleReject",1).toBool();    
    m_sSystemInfo.bVEXICleanLightSource = iniset.value("/system/bVEXICleanLightSource",1).toBool();    
    m_sSystemInfo.nVEXICleanLightSourceInterval = iniset.value("/system/nVEXICleanLightSourceInterval",1).toInt();    
    m_sSystemInfo.nVEXICleanLightSourceWidth = iniset.value("/system/nVEXICleanLightSourceWidth",1).toInt();    
    m_savecameranumber=iniset.value("/system/savecameranumber",3).toInt()-1;
    m_overPress = iniset.value("/system/overPress",1).toInt();
    m_ContinueButtle =iniset.value("/system/ContinueButtle",3).toInt();
    for (int i=0;i<CAMERA_MAX_COUNT;i++)
    {
        strSession = QString("/NoRejectIfNoOrigin/Device_%1").arg(i+1);
        m_sSystemInfo.m_iNoRejectIfNoOrigin[i] = iniset.value(strSession,0).toInt();
        strSession = QString("/NoKickIfROIFail/Device_%1").arg(i+1);
        m_sSystemInfo.m_iNoRejectIfROIfail[i] = iniset.value(strSession,0).toInt();
        strSession = QString("/NoStaticIfNoOrigin/Device_%1").arg(i+1);
        m_sSystemInfo.m_iNoStaticIfNoOrigin[i] = iniset.value(strSession,0).toInt();

        strSession = QString("/ImageTargetNo/Device_%1").arg(i+1);
        m_sRealCamInfo[i].m_iImageTargetNo = iniset.value(strSession,0).toInt();
    }
    int iShift[3];
    iShift[0] = iniset.value("/system/shift1",000000).toInt();
    iShift[1] = iniset.value("/system/shift2",80000).toInt();
    iShift[2] = iniset.value("/system/shift3",160000).toInt();
    for (int i = 0; i<2; i++)
    {
        if (iShift[i] > iShift[i+1] )
        {
            int temp =iShift[i];
            iShift[i] = iShift[i+1];
            iShift[i+1] = temp;
        }
    }
    m_sSystemInfo.shift1.setHMS(iShift[0]/10000,(iShift[0]%10000)/100,iShift[0]%100);
    m_sSystemInfo.shift2.setHMS(iShift[1]/10000,(iShift[1]%10000)/100,iShift[1]%100);
    m_sSystemInfo.shift3.setHMS(iShift[2]/10000,(iShift[2]%10000)/100,iShift[2]%100);

    //设置剪切参数路径
    m_sConfigInfo.m_strGrabInfoPath = m_sConfigInfo.m_strAppPath + "ModelInfo/" + m_sSystemInfo.m_strModelName + "/GrabInfo.ini";
    m_sSystemInfo.iRealCamCount = m_sSystemInfo.iRealCamCount;
    int devID = 0;
    for (int i=0;i<m_sSystemInfo.iRealCamCount;i++)
    {
        struGrabCardPara[i].iGrabberTypeSN = EGT_Dalsa_X64;
        strSession = QString("/GarbCardParameter/Device%1ID").arg(i+1);
        struGrabCardPara[i].nGrabberSN = iniset.value(strSession,-1).toInt();
        strSession = QString("/GarbCardParameter/Device%1Type").arg(i+1);
        m_sRealCamInfo[i].m_iDevType = iniset.value(strSession, (int)EDTCameraFinishBottom).toInt();
        m_sDeviceType.insert(i, m_sRealCamInfo[i].m_iDevType);
        strSession = QString("/GarbCardParameter/Device%1Name").arg(i+1);
        strcpy_s(struGrabCardPara[i].strDeviceName,iniset.value(strSession,"").toString().toLocal8Bit().data());
        strSession = QString("/GarbCardParameter/Device%1Mark").arg(i+1);
        strcpy_s(struGrabCardPara[i].strDeviceMark,iniset.value(strSession,"").toString().toLocal8Bit().data());
        QString strGrabInitFile;//存储相机初始化位置
        strSession = QString("/GarbCardParameter/Device%1InitFile").arg(i+1);
        strGrabInitFile = iniset.value(strSession,"").toString();

        strSession = QString("/GarbCardParameter/Device%1Station").arg(i+1);
        m_sRealCamInfo[i].m_iGrabPosition = iniset.value(strSession,0).toInt();
        strSession = QString("/RoAngle/Device_%1").arg(i+1);
        m_sRealCamInfo[i].m_iImageRoAngle = iniset.value(strSession,0).toInt();
        strSession = QString("/ImageType/Device_%1").arg(i+1);
        m_sRealCamInfo[i].m_iImageType = iniset.value(strSession,0).toInt();
        strSession = QString("/IOCardSN/Device_%1").arg(i+1);
        m_sRealCamInfo[i].m_iIOCardSN = iniset.value(strSession,0).toInt();

        //采集卡文件路径与config所在文件夹相同
        strGrabInitFile = m_sConfigInfo.m_strConfigPath.left(m_sConfigInfo.m_strConfigPath.findRev("/")+1) + strGrabInitFile;
        memcpy(struGrabCardPara[i].strGrabberFile,strGrabInitFile.toLocal8Bit().data(),GBMaxTextLen);
        devID++;
    }
    if(m_sSystemInfo.isThickNessEnable)
    {//初始化壁厚ID与类型绑定
        //读取壁厚配置
        ReadThicknessConfig();
        for(int i = 0; i < thickConfig.SensorTotal;i++)
        {
            m_sDeviceType.insert(devID, (int)EDTThickness);
            thk_devid_index.insert(devID, i);
            devID++;
        }
    }
    m_sSystemInfo.iDevTotalCount = devID;
    QSettings iniCameraSet(m_sConfigInfo.m_strGrabInfoPath,QSettings::IniFormat);
    QString strShuter,strTrigger;
    for(int i = 0; i < m_sSystemInfo.iRealCamCount; i++)
    {
        strShuter = QString("/Shuter/Grab_%1").arg(i);
        strTrigger = QString("/Trigger/Grab_%1").arg(i);
        m_sRealCamInfo[i].m_iShuter=iniCameraSet.value(strShuter,20).toInt();
        m_sRealCamInfo[i].m_iTrigger=iniCameraSet.value(strTrigger,1).toInt();//默认外触发
    }

    //根据语言找到对应的文件名
    moldwors.clear();
    sensorwords.clear();
    if(QFile::exists(m_sConfigInfo.m_strWordsPath))
    {//解析词条文件 
        QSettings sets(m_sConfigInfo.m_strWordsPath, QSettings::IniFormat);
        sets.setIniCodec(QTextCodec::codecForName("UTF-8"));

        sets.beginGroup("MoldWords");
        QStringList keys = sets.childKeys();
        foreach(QString key, keys)
        {
            moldwors.insert(key, sets.value(key).toString()  ) ;
        }
        sets.endGroup();
        sets.beginGroup("SensorWords");
        keys = sets.childKeys();
        foreach(QString key, keys)
        {
            sensorwords.insert(key, sets.value(key).toString() ) ;
        }
        sets.endGroup();
    }
    bKickFailed = false;
    InitLastData();
}

void SystemUIMain::ReadThicknessConfig()
{
    QString path = QString("%1%2").arg(ConfigDir).arg(ThicknessCfgFile);
    QSettings iniset(path, QSettings::IniFormat);
    iniset.setIniCodec(QTextCodec::codecForName("GBK"));
    iniset.beginGroup("System");
    thickConfig.SensorTotal = iniset.value("SensorTotal",0).toInt();
    thickConfig.SensorName = iniset.value("SensorName","SIM").toString();
    thickConfig.s_TriggerMode = iniset.value("TriggerMode",0).toInt();
    thickConfig.s_FilterWidth = iniset.value("nFilterWidth",0).toInt();
    thickConfig.m_Units = iniset.value("bUnits",0).toInt();						//单位 
    thickConfig.isAutoRestart = iniset.value("isAutoRestart",false).toBool();		//图像不刷新自动重启  
    thickConfig.isDivKickCount = iniset.value("isDivKickCount",false).toBool();	//开启或关闭分开踢废功能
    thickConfig.DataAllZeroisKickout = iniset.value("DataAllZeroisKickout",false).toBool();
    iniset.endGroup();
    //Sensor set
    for (int i=0;i<thickConfig.SensorTotal;i++)
    {
        ThicknessParameter m_SensorParam;
        iniset.beginGroup(QString("Thickness%1").arg(i+1));
        m_SensorParam.m_CheckNess = iniset.value("CheckNess",0).toDouble();
        m_SensorParam.m_CheckStation = iniset.value("CheckStation",0).toInt();
        m_SensorParam.m_bThickness = iniset.value("bThickness",1).toBool();
        m_SensorParam.m_MinNess = iniset.value("MinNess",0).toDouble();
        m_SensorParam.m_MinNessNo = iniset.value("MinNessNo",0).toInt();
        m_SensorParam.m_ThinkNessRate = iniset.value("ThinkNessRate",1.0).toFloat();
        m_SensorParam.m_Allresult = iniset.value("Allresult",0).toInt();
        m_SensorParam.m_PresentRate = iniset.value("TriggerRate",0).toInt();
        m_SensorParam.m_nThicknessOffset = iniset.value("nThicknessOffset",0).toDouble();
        m_SensorParam.m_ConnStr = iniset.value("IPAddress","127.0.0.1").toString();
        m_SensorParam.m_bOvaEnable = iniset.value("bOvaEnable",1).toBool();
        m_SensorParam.m_bOvalization = iniset.value("bOvalization",1).toBool();
        m_SensorParam.m_maxDistance = iniset.value("MaxDistance",0).toDouble();
        m_SensorParam.m_minDistance = iniset.value("MinDistance",0).toDouble();
        m_SensorParam.m_ovalizationLimit = iniset.value("OvaThreshold",0).toDouble();
        m_SensorParam.m_OvaFactor = iniset.value("OvaFactor",0).toDouble();
        m_SensorParam.m_iSaveOvaData = iniset.value("OvaSaveData",0).toInt();
        QString str = iniset.value(QString("graphColor"), "rgb(0,0,0)").toString();
        m_SensorParam.curveColor = StringToColor(str);
        iniset.endGroup();
        thickParams<<m_SensorParam;
    }
}

void SystemUIMain::SaveThicknessConfig()
{
    QString path = QString("%1%2").arg(ConfigDir).arg(ThicknessCfgFile);
    QSettings iniset(path, QSettings::IniFormat);
    iniset.setIniCodec(QTextCodec::codecForName("GBK"));
    iniset.beginGroup("System");
    iniset.setValue("SensorTotal", thickConfig.SensorTotal);
    iniset.setValue("SensorName", thickConfig.SensorName);
    iniset.setValue("TriggerMode", thickConfig.s_TriggerMode);
    iniset.setValue("nFilterWidth", thickConfig.s_FilterWidth);
    iniset.setValue("bUnits", thickConfig.m_Units);
    iniset.setValue("isDivKickCount", thickConfig.isDivKickCount);
    iniset.setValue("DataAllZeroisKickout", thickConfig.DataAllZeroisKickout);
    iniset.endGroup();

    //Sensor set
    for (int i=0;i<thickConfig.SensorTotal;i++)
    {
        auto& thkParam = thickParams[i];
        iniset.beginGroup(QString("Thickness%1").arg(i+1));
        iniset.setValue("CheckNess", thkParam.m_CheckNess);
        iniset.setValue("CheckStation", thkParam.m_CheckStation);
        iniset.setValue("bThickness", thkParam.m_bThickness);
        iniset.setValue("MinNess", thkParam.m_MinNess);
        iniset.setValue("MinNessNo", thkParam.m_MinNessNo);
        iniset.setValue("ThinkNessRate", QString::number(thkParam.m_ThinkNessRate, 'f', 2));
        iniset.setValue("Allresult", thkParam.m_Allresult);
        iniset.setValue("TriggerRate", thkParam.m_PresentRate);
        iniset.setValue("nThicknessOffset", thkParam.m_nThicknessOffset);
        iniset.setValue("IPAddress", thkParam.m_ConnStr);
        iniset.setValue("bOvaEnable", thkParam.m_bOvaEnable);
        iniset.setValue("bOvalization", thkParam.m_bOvalization);
        iniset.setValue("MaxDistance", thkParam.m_maxDistance);
        iniset.setValue("MinDistance", thkParam.m_minDistance);
        iniset.setValue("OvaThreshold", thkParam.m_ovalizationLimit);
        iniset.setValue("OvaFactor", thkParam.m_OvaFactor);
        auto colorStr = QString("rgb(%1,%2,%3)").arg(thkParam.curveColor.red()).arg(thkParam.curveColor.green()).arg(thkParam.curveColor.blue());
        iniset.setValue("graphColor", colorStr);
        iniset.endGroup();
    }
}

//读取切割信息
void SystemUIMain::ReadCorveConfig()
{
    for(int i=0; i<m_sSystemInfo.iRealCamCount; i++)
    {
        m_sRealCamInfo[i].m_sCorves.i_ImageCount = 0;
    }
    QSettings iniCarveSet(m_sConfigInfo.m_strGrabInfoPath,QSettings::IniFormat);
    QString strSession;
    int iRealCameraSN;
    if(m_sSystemInfo.m_bIsCarve)//如果剪切图像
    {
        for(int i=0; i<m_sSystemInfo.iRealCamCount; i++)
        {
            strSession = QString("/convert/Grab_%1").arg(i);
            iRealCameraSN=i;
            int index = m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageCount;
            if(deviceType(i) == EDTCameraLineMatrix || m_sRealCamInfo[iRealCameraSN].m_iGrabType == EGT_LCam_DALSA)
            {//线阵相机不支持裁剪
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_GrabSN[index] = i;
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageX[index] = 0;
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageY[index] = 0;
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageWidth[index] = m_sRealCamInfo[iRealCameraSN].m_iImageWidth;
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageHeight[index] = m_sRealCamInfo[iRealCameraSN].m_iImageHeight;
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageCount++;

                //加载剪切后参数
                m_sCarvedCamInfo[i].m_iImageAngle = 0;
                m_sCarvedCamInfo[i].m_iStress = 0;
                m_sCarvedCamInfo[i].m_iToNormalCamera = i;
            }
            else
            {
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_GrabSN[index] = i;
                strSession = QString("/pointx/Grab_%1").arg(i);
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageX[index] = iniCarveSet.value(strSession,0).toInt();
                strSession = QString("/pointy/Grab_%1").arg(i);
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageY[index] = iniCarveSet.value(strSession,0).toInt();
                strSession = QString("/width/Grab_%1").arg(i);
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageWidth[index] = iniCarveSet.value(strSession,m_sRealCamInfo[iRealCameraSN].m_iImageWidth).toInt();
                strSession = QString("/height/Grab_%1").arg(i);
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageHeight[index] = iniCarveSet.value(strSession,m_sRealCamInfo[iRealCameraSN].m_iImageHeight).toInt();
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageCount++;

                //加载剪切后参数
                strSession = QString("/angle/Grab_%1").arg(i);
                m_sCarvedCamInfo[i].m_iImageAngle = iniCarveSet.value(strSession,0).toInt();
                strSession = QString("/Stress/Device_%1").arg(i+1);
                m_sCarvedCamInfo[i].m_iStress = iniCarveSet.value(strSession,0).toInt();
                strSession = QString("/tonormal/Grab_%1").arg(i);
                m_sCarvedCamInfo[i].m_iToNormalCamera = iniCarveSet.value(strSession,0).toInt();
            }

            m_sRealCamInfo[i].m_sCorves.i_ImageCount = 1;
            
            //检测剪切参数是否合适
            if (m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageX[index]<0)
            {
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageX[index] = 0;
                CLogFile::write(tr("x is smaller than 0----Camera%1").arg(i+1),AbnormityLog);

            }
            if (m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageY[index]<0)
            {
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageY[index] = 0;
                CLogFile::write(tr("y is smaller than 0----Camera%1").arg(i+1),AbnormityLog);

            }
            if (m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageWidth[index]<0)
            {
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageWidth[index] =  m_sRealCamInfo[iRealCameraSN].m_iImageWidth;;
                CLogFile::write(tr("Width pix is smaller than 0----Camera%1").arg(i+1),AbnormityLog);

            }
            if (m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageHeight[index]<0)
            {
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageHeight[index] = m_sRealCamInfo[iRealCameraSN].m_iImageHeight;
                CLogFile::write(tr("Height pix is smaller than 0----Camera%1").arg(i+1),AbnormityLog);

            }
            if ((m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageX[index] + m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageWidth[index]) > m_sRealCamInfo[iRealCameraSN].m_iImageWidth)
            {
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageX[index] = 0;
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageWidth[index] = m_sRealCamInfo[iRealCameraSN].m_iImageWidth;
                CLogFile::write(tr("Width pix is too big----Camera%1").arg(i+1),AbnormityLog);
            }
            if ((m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageY[index] + m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageHeight[index]) > m_sRealCamInfo[iRealCameraSN].m_iImageHeight)
            {
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageY[index] = 0;
                m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageHeight[index] = m_sRealCamInfo[iRealCameraSN].m_iImageHeight;
                CLogFile::write(tr("Height pix is too big----Camera%1").arg(i+1),AbnormityLog);
            }
            m_sCarvedCamInfo[i].i_ImageX = m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageX[index];
            m_sCarvedCamInfo[i].i_ImageY = m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageY[index];
            m_sCarvedCamInfo[i].m_iImageWidth = m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageWidth[index];
            m_sCarvedCamInfo[i].m_iImageHeight =m_sRealCamInfo[iRealCameraSN].m_sCorves.i_ImageHeight[index];
        }
    }
    else//不剪切
    {
        for(int i=0; i<m_sSystemInfo.iRealCamCount; i++)
        {
            m_sCarvedCamInfo[i].m_iToRealCamera = i;
            int index = m_sRealCamInfo[i].m_sCorves.i_ImageCount;
            m_sRealCamInfo[i].m_sCorves.i_GrabSN[index] = i;
            m_sRealCamInfo[i].m_sCorves.i_ImageX[index] = 0;
            m_sRealCamInfo[i].m_sCorves.i_ImageY[index] = 0;
            m_sRealCamInfo[i].m_sCorves.i_ImageWidth[index] = m_sCarvedCamInfo[i].m_iResImageWidth;
            m_sRealCamInfo[i].m_sCorves.i_ImageHeight[index] = m_sCarvedCamInfo[i].m_iResImageHeight;
            m_sRealCamInfo[i].m_sCorves.i_ImageCount = 1;

            m_sCarvedCamInfo[i].m_iToRealCamera = i;
            m_sCarvedCamInfo[i].i_ImageX = 0;
            m_sCarvedCamInfo[i].i_ImageY = 0;
            m_sCarvedCamInfo[i].m_iImageWidth = m_sRealCamInfo[i].m_iImageWidth;
            m_sCarvedCamInfo[i].m_iImageHeight = m_sRealCamInfo[i].m_iImageHeight;

            m_sCarvedCamInfo[i].m_iStress = 1;
        }
    }
}
//加载参数和相机
void SystemUIMain::LoadParameterAndCam()
{
    for (int i=0;i<m_sSystemInfo.iRealCamCount;i++)
    {
        //回调
        struGrabCardPara[i].CallBackFunc = GlobalGrabOverCallback;
        struGrabCardPara[i].Context = this;
        //初始化采集卡
        InitGrabCard(struGrabCardPara[i],i);
    }
}

void SystemUIMain::InitThickness()
{
    auto ids = getDeviceList(EDTThickness);
    //由于壁厚处理需要IO卡,所以此函数应在IO卡初始化之后
    foreach(int id, ids)
    {
        CurrencyThick* pThick = NULL; 
        if(thickConfig.SensorName == "SIXIAN")
        {
            pThick = new SixianThick(id);
        }
        else if(thickConfig.SensorName == "STIL")
        {
            pThick = new StilThick(id);
        }
        else if(thickConfig.SensorName == "CHR")
        {
            pThick = new CHRThick(id);
        }
        else //if(thickConfig.SensorName == "SIM")
        {
            pThick = new SimulationThick(id);
        }
        m_ThickTypes.insert(id, pThick);
        CTReadThickness* pReadThk = new CTReadThickness(id, this);
        m_TReadThickNessList.insert(id, pReadThk);

        //初始化时 壁厚设备连接和启动采集
        char temps[255]={0};
        strcpy(temps,thickParams[thk_devid_index[id]].m_ConnStr.toStdString().c_str());
        if(! pThick->ConnectSensor(temps) )
        {//
            CLogFile::write(tr("Thickness%1 connect failed!").arg(id), OperationLog, 1);
        }
    }
}

//初始化采集卡（：初始化相机）
void SystemUIMain::InitGrabCard(s_GBINITSTRUCT struGrabCardPara,int index)
{
    QString strDeviceName = QString(struGrabCardPara.strDeviceName);
    if (strDeviceName == "MER")
    {
        m_sRealCamInfo[index].m_pGrabber = new CDHGrabberMER;
        m_sRealCamInfo[index].m_bSmuGrabber = false;
        m_sRealCamInfo[index].m_iGrabType = EGT_DH_MER;
    }
#ifndef NO_DALSA_CAM
    else if(strDeviceName == "DALSA")
    {
        m_sRealCamInfo[index].m_pGrabber = new CDHGrabberDALSA;
        m_sRealCamInfo[index].m_bSmuGrabber = false;
        m_sRealCamInfo[index].m_iGrabType = EGT_LCam_DALSA;
    }
#endif
    else //if (strDeviceName=="SimulaGrab")
    {
        m_sRealCamInfo[index].m_pGrabber = new CDHGrabberSG;
        m_sRealCamInfo[index].m_bSmuGrabber = true;
        m_sRealCamInfo[index].m_iGrabType = EGT_Simu;
    }    
    BOOL bRet = FALSE;
    int iErrorPosition = 0;
    try
    {
        CLogFile::write(tr("InitCamera%1,begin").arg(struGrabCardPara.nGrabberSN),OperationLog);

        bRet = m_sRealCamInfo[index].m_pGrabber->Init(&struGrabCardPara);            
        CLogFile::write(tr("InitCamera%1,over").arg(index),OperationLog);

        if(bRet)
        {
            iErrorPosition = 1;
            m_sRealCamInfo[index].m_bCameraInitSuccess=TRUE;
            bRet = FALSE;
            bRet = m_sRealCamInfo[index].m_pGrabber->GetParamInt(GBImageWidth, m_sRealCamInfo[index].m_iImageWidth);
            if(bRet)
            {
                iErrorPosition = 2;
                bRet = FALSE;
                bRet = m_sRealCamInfo[index].m_pGrabber->GetParamInt(GBImageHeight, m_sRealCamInfo[index].m_iImageHeight);
                if(bRet)
                {
                    iErrorPosition = 3;
                    bRet = FALSE;
                    bRet = m_sRealCamInfo[index].m_pGrabber->GetParamInt(GBImageBufferSize, m_sRealCamInfo[index].m_iImageSize);    
                    if(bRet)
                    {
                        iErrorPosition = 4;
                        int nImagePixelSize = 0;
                        bRet = FALSE;
                        bRet = m_sRealCamInfo[index].m_pGrabber->GetParamInt(GBImagePixelSize, nImagePixelSize);
                        if(bRet)
                        {
                            iErrorPosition = 5;
                            m_sRealCamInfo[index].m_iImageBitCount =8* nImagePixelSize;
                            if(strDeviceName == "MER")
                            {
                                iErrorPosition = 6;
                                ((CDHGrabberMER*)m_sRealCamInfo[index].m_pGrabber)->MERSetParamInt(MERSnapMode,1);
                            }
                        }
                    }
                }
            }        
        }
    }
    catch (...)
    {
        QString strError;
        strError = QString("catch camera%1initial error").arg(index);
        CLogFile::write(strError,OperationLog);
        m_sRealCamInfo[index].m_bCameraInitSuccess = FALSE;

    }

    if (bRet)
    {
        InitCam();
        m_sRealCamInfo[index].m_bCameraInitSuccess = TRUE;

    }
    else
    {
        m_sRealCamInfo[index].m_bCameraInitSuccess = FALSE;

        s_GBERRORINFO ErrorInfo;
        QString str;            
        m_sRealCamInfo[index].m_pGrabber->GetLastErrorInfo(&ErrorInfo);
        str = tr("DeviceName:%1").arg(strDeviceName)+"\n"+\
            tr("ErrorCode:%2").arg(ErrorInfo.nErrorCode)+"\n"+\
            tr("ErrorDescription:%3").arg(QString::fromLocal8Bit(ErrorInfo.strErrorDescription))+"\n"+\
            tr("ErrorRemark:%4\n").arg(QString::fromLocal8Bit(ErrorInfo.strErrorRemark))+"\n"+\
            tr("ErrorPosition:%5\n").arg(iErrorPosition);
        QMessageBox::information(this,tr("Error"),str);
        QString strError;
        strError = QString("camera%1initial error,ErrorPosition%2").arg(index).arg(iErrorPosition);
        CLogFile::write(strError,OperationLog);
        CLogFile::write(str, AbnormityLog);

        m_sRealCamInfo[index].m_strErrorInfo = str;
    }

    int iRealCameraSN = index;
    if (90 == m_sRealCamInfo[iRealCameraSN].m_iImageRoAngle || 270 == m_sRealCamInfo[iRealCameraSN].m_iImageRoAngle )
    {
        int iTemp = m_sRealCamInfo[iRealCameraSN].m_iImageHeight;
        m_sRealCamInfo[iRealCameraSN].m_iImageHeight = m_sRealCamInfo[iRealCameraSN].m_iImageWidth;
        m_sRealCamInfo[iRealCameraSN].m_iImageWidth = iTemp;
    }

}
//初始化相机（设置曝光时间和触发方式）
void SystemUIMain::InitCam()
{
    for(int i = 0; i < m_sSystemInfo.iRealCamCount; i++)
    {
        //m_mutexmGrab.lock();
        if(m_sRealCamInfo[i].m_iGrabType == (int)EGT_DH_MER)
        {
            if(m_sRealCamInfo[i].m_iTrigger == 1)
            {
                ((CDHGrabberMER*)m_sRealCamInfo[i].m_pGrabber)->MERSetParamInt(MERSnapMode,1);
                m_sRealCamInfo[i].m_bGrabIsTrigger = true;
            }
            else if(m_sRealCamInfo[i].m_iTrigger == 0)
            {
                ((CDHGrabberMER*)m_sRealCamInfo[i].m_pGrabber)->MERSetParamInt(MERSnapMode,0);
                m_sRealCamInfo[i].m_bGrabIsTrigger = false;
            }
            ((CDHGrabberMER*)m_sRealCamInfo[i].m_pGrabber)->MERSetParamInt(MERExposure,m_sRealCamInfo[i].m_iShuter);
        }
#ifndef NO_DALSA_CAM
        else if(m_sRealCamInfo[i].m_iGrabType == (int)EGT_LCam_DALSA)
        {
            ((CDHGrabberDALSA*)m_sRealCamInfo[i].m_pGrabber)->SetDALSAParamInt(DALSA_ExposureTime,m_sRealCamInfo[i].m_iShuter);
            m_sRealCamInfo[i].m_bGrabIsTrigger = true;
        }
#endif
        //m_mutexmGrab.unlock();
    }
}
//初始化图像（：读取切割信息:初始化图像队列和剪切后相机参数）
void SystemUIMain::InitImage()
{
    //获取最大图像信息
    for (int i=0;i<m_sSystemInfo.iRealCamCount;i++)
    {
        if (i==0)
        {
            m_sSystemInfo.m_iMaxCameraImageWidth     = m_sRealCamInfo[i].m_iImageWidth;
            m_sSystemInfo.m_iMaxCameraImageHeight    = m_sRealCamInfo[i].m_iImageHeight;
            m_sSystemInfo.m_iMaxCameraImageSize      = m_sRealCamInfo[i].m_iImageSize;
            m_sSystemInfo.m_iMaxCameraImagePixelSize = (m_sRealCamInfo[i].m_iImageBitCount+7)/8;
        }
        else
        {
            if (m_sRealCamInfo[i].m_iImageWidth > m_sSystemInfo.m_iMaxCameraImageWidth)
            {
                m_sSystemInfo.m_iMaxCameraImageWidth = m_sRealCamInfo[i].m_iImageWidth;
            }                
            if (m_sRealCamInfo[i].m_iImageHeight > m_sSystemInfo.m_iMaxCameraImageHeight)
            {
                m_sSystemInfo.m_iMaxCameraImageHeight = m_sRealCamInfo[i].m_iImageHeight;
            }                
            if (((m_sRealCamInfo[i].m_iImageBitCount+7)/8) > m_sSystemInfo.m_iMaxCameraImagePixelSize)
            {
                m_sSystemInfo.m_iMaxCameraImagePixelSize = ((m_sRealCamInfo[i].m_iImageBitCount+7)/8);
            }            
        }
        m_sSystemInfo.m_iMaxCameraImageSize = m_sSystemInfo.m_iMaxCameraImageWidth*m_sSystemInfo.m_iMaxCameraImageHeight;
    }
    
    //读取切割信息,因为要用到初始化以后的相机信息，所以不能提前读取。
    {
        for (int i = 0;i<m_sSystemInfo.iRealCamCount;i++)
        {
            m_sCarvedCamInfo[i].m_iResImageWidth = m_sRealCamInfo[i].m_iImageWidth;
            m_sCarvedCamInfo[i].m_iResImageHeight = m_sRealCamInfo[i].m_iImageHeight;
            m_sCarvedCamInfo[i].m_iImageWidth = m_sRealCamInfo[i].m_iImageWidth;
            m_sCarvedCamInfo[i].m_iImageHeight = m_sRealCamInfo[i].m_iImageHeight;
            m_sCarvedCamInfo[i].m_iImageType = m_sRealCamInfo[i].m_iImageType;
            m_sCarvedCamInfo[i].m_iIOCardSN =  m_sRealCamInfo[i].m_iIOCardSN;
            m_sCarvedCamInfo[i].m_iToRealCamera = i;
            m_sCarvedCamInfo[i].m_iImageBitCount = m_sRealCamInfo[i].m_iImageBitCount;
            m_sCarvedCamInfo[i].m_iImageRoAngle = m_sRealCamInfo[i].m_iImageRoAngle;    
        }
        
        int iCarvedCamNum = 0;
        QString strSession;
        
        for(int i = 0 ; i < m_sSystemInfo.iRealCamCount; i++)
        {
            //分配原始图像空间：每真实相机1个，剪切图像使用
            if (m_sRealCamInfo[i].m_pRealImage!=NULL)
            {
                delete m_sRealCamInfo[i].m_pRealImage;
                m_sRealCamInfo[i].m_pRealImage = NULL;
            }

            m_sRealCamInfo[i].m_pRealImage=new QImage(m_sRealCamInfo[i].m_iImageWidth,m_sRealCamInfo[i].m_iImageHeight,m_sRealCamInfo[i].m_iImageBitCount);// 用于实时显示

            if (8 == m_sRealCamInfo[i].m_iImageBitCount)
            {
                m_sRealCamInfo[i].m_pRealImage->setColorTable(m_vcolorTable);
            }

            memset(m_sRealCamInfo[i].m_pRealImage->bits(),0, m_sRealCamInfo[i].m_pRealImage->byteCount());

        }
        ReadCorveConfig();
        for(int i = 0 ; i < m_sSystemInfo.iRealCamCount; i++)
        {
            iCarvedCamNum = i;


            //实时显示用, 预分配QImage空间，每切出相机一个
            if (m_sCarvedCamInfo[iCarvedCamNum].m_pActiveImage!=NULL)
            {
                delete m_sCarvedCamInfo[iCarvedCamNum].m_pActiveImage;
                m_sCarvedCamInfo[iCarvedCamNum].m_pActiveImage = NULL;
            }
            if (90 == m_sCarvedCamInfo[iCarvedCamNum].m_iImageRoAngle || 270 ==m_sCarvedCamInfo[iCarvedCamNum].m_iImageRoAngle)
            {
                m_sCarvedCamInfo[iCarvedCamNum].m_pActiveImage=new QImage(m_sCarvedCamInfo[iCarvedCamNum].m_iImageHeight,m_sCarvedCamInfo[iCarvedCamNum].m_iImageWidth,m_sCarvedCamInfo[iCarvedCamNum].m_iImageBitCount);// 用于实时显示
            }
            else
            {
                m_sCarvedCamInfo[iCarvedCamNum].m_pActiveImage=new QImage(m_sCarvedCamInfo[iCarvedCamNum].m_iImageWidth,m_sCarvedCamInfo[iCarvedCamNum].m_iImageHeight,m_sCarvedCamInfo[iCarvedCamNum].m_iImageBitCount);// 用于实时显示
            }
            m_sCarvedCamInfo[iCarvedCamNum].m_pActiveImage->setColorTable(m_vcolorTable);
            //开始采集前补一张黑图
            BYTE* pByte = m_sCarvedCamInfo[iCarvedCamNum].m_pActiveImage->bits();
            int iLength = m_sCarvedCamInfo[iCarvedCamNum].m_pActiveImage->byteCount();
            memset((pByte),0,(iLength));
            //分配图像剪切内存区域,大小等于真实相机大小
            if (m_sCarvedCamInfo[iCarvedCamNum].m_pGrabTemp!=NULL)
            {
                delete m_sCarvedCamInfo[iCarvedCamNum].m_pGrabTemp; 
                m_sCarvedCamInfo[iCarvedCamNum].m_pGrabTemp = NULL;
            }
            m_sCarvedCamInfo[iCarvedCamNum].m_pGrabTemp = new BYTE[m_sCarvedCamInfo[iCarvedCamNum].m_iImageWidth*m_sCarvedCamInfo[iCarvedCamNum].m_iImageHeight*m_sCarvedCamInfo[iCarvedCamNum].m_iImageBitCount];
            //分配元素链表中图像的内存，每剪切出来的相机10个。
            m_queue[iCarvedCamNum].mLocker.lock();
            m_queue[iCarvedCamNum].InitQueue(m_sCarvedCamInfo[iCarvedCamNum].m_iImageWidth, m_sCarvedCamInfo[iCarvedCamNum].m_iImageHeight,m_sCarvedCamInfo[iCarvedCamNum].m_iImageBitCount, m_sRealCamInfo[iCarvedCamNum].m_iImageTargetNo+5, true);
            m_queue[iCarvedCamNum].mLocker.unlock();

            for (int k = 0; k < 256;k++)
            {
                delete []m_sCarvedCamInfo[iCarvedCamNum].sImageLocInfo[k].m_AlgImageLocInfos.sXldPoint.nColsAry;
                delete []m_sCarvedCamInfo[iCarvedCamNum].sImageLocInfo[k].m_AlgImageLocInfos.sXldPoint.nRowsAry;

                m_sCarvedCamInfo[iCarvedCamNum].sImageLocInfo[k].m_iHaveInfo = 0;
                m_sCarvedCamInfo[iCarvedCamNum].sImageLocInfo[k].m_AlgImageLocInfos.sXldPoint.nRowsAry = new int[4*BOTTLEXLD_POINTNUM];
                m_sCarvedCamInfo[iCarvedCamNum].sImageLocInfo[k].m_AlgImageLocInfos.sXldPoint.nColsAry = new int[4*BOTTLEXLD_POINTNUM];
                memset(m_sCarvedCamInfo[iCarvedCamNum].sImageLocInfo[k].m_AlgImageLocInfos.sXldPoint.nRowsAry,0, 4*BOTTLEXLD_POINTNUM);
                memset(m_sCarvedCamInfo[iCarvedCamNum].sImageLocInfo[k].m_AlgImageLocInfos.sXldPoint.nColsAry,0, 4*BOTTLEXLD_POINTNUM);
            }
        }

        for(int i = 0 ; i < m_sSystemInfo.iDevTotalCount; i++)
        {
            // 错误统计用类
            m_sRunningInfo.m_cErrorTypeInfo[i].m_iErrorTypeCount = m_sErrorInfo.m_iErrorTypeCount;
        }
    }
    //初始化缺陷图像列表
    m_ErrorList.initErrorList(m_sSystemInfo.m_iMaxCameraImageWidth,m_sSystemInfo.m_iMaxCameraImageHeight,m_sSystemInfo.m_iMaxCameraImagePixelSize*8,ERROR_IMAGE_COUNT,true);
}

//设置图像综合参数
void SystemUIMain::SetCombineInfo()
{
    //综合结果ID
    //初始化结果综合参数
    for (int i = 0;i<m_sSystemInfo.iDevTotalCount;i++)
    {
        auto devtype = deviceType(i);
        switch(devtype)
        {
        case EDTThickness:
            {//壁厚
                if(thickParams[thk_devid_index[i]].m_cardID == 0)
                {
                    m_cCombine.SetCombineCamera(i,true);
                    m_sSystemInfo.IOCardiCamCount[0]++;
                }
                else
                {
                    m_cCombine.SetCombineCamera(i,false);
                }
            }
            break;
        case EDTCameraFinishBottom:
        case EDTCameraOvality:
        case EDTCameraLineMatrix:
        default:
            {
                if (m_sCarvedCamInfo[i].m_iIOCardSN == 0)
                {
                    m_cCombine.SetCombineCamera(i,true);
                    m_sSystemInfo.IOCardiCamCount[0]++;
                }
                else
                {
                    m_cCombine.SetCombineCamera(i,false);
                }
            }
            break;
        }
    }
    m_cCombine.Inital(m_sSystemInfo.IOCardiCamCount[0]);
}
//初始化IO卡
void SystemUIMain::InitIOCard()
{
    if (m_sSystemInfo.m_bIsIOCardOK && 1 != m_sSystemInfo.iIOCardCount)
    {//当前只支持1张接口卡
        QString err = tr("IOCard Number is not one! IOCardNumber:%1 set support count:1.").arg(m_sSystemInfo.iIOCardCount);
        CLogFile::write(err, OperationLog);
        m_sSystemInfo.iIOCardCount = 1;
    }
    if (m_sSystemInfo.m_bIsIOCardOK)
    {
        for (int i=0;i<m_sSystemInfo.iIOCardCount;i++)
        {
            if(i ==0)
            {
                m_sSystemInfo.m_sConfigIOCardInfo[i].iCardID = i;
                m_sSystemInfo.m_sConfigIOCardInfo[i].strCardInitFile = QString("./PIO24B_reg_init.txt");
                m_sSystemInfo.m_sConfigIOCardInfo[i].strCardName = QString("PIO24B");
            }
            else
            {
                m_sSystemInfo.m_sConfigIOCardInfo[i].iCardID = i;
                m_sSystemInfo.m_sConfigIOCardInfo[i].strCardInitFile = QString("./PIO24B_reg_init%1.txt").arg(i);
                m_sSystemInfo.m_sConfigIOCardInfo[i].strCardName = QString("PIO24B").arg(i);
            }
            m_vIOCard[i] = new CIOCard(m_sSystemInfo.m_sConfigIOCardInfo[i],i);
            connect(m_vIOCard[i],SIGNAL(emitMessageBoxMainThread(s_MSGBoxInfo)),this,SLOT(slots_MessageBoxMainThread(s_MSGBoxInfo)));
            s_IOCardErrorInfo sIOCardErrorInfo = m_vIOCard[i]->InitIOCard();
            if (!sIOCardErrorInfo.bResult)
            {
                m_sSystemInfo.m_bIsIOCardOK = FALSE;
                CLogFile::write(tr("Error in init IOCard"),AbnormityLog);
            }
        }
        pThreadBaseClean = new QThreadBaseClean(this);
        pThreadBaseClean->setBaseCleanParam(m_sSystemInfo.bVEXICleanLightSource, m_sSystemInfo.nVEXICleanLightSourceInterval,m_sSystemInfo.nVEXICleanLightSourceWidth);
        if (m_sSystemInfo.m_bIsIOCardOK)
        {
            pThreadBaseClean->start();
            timerTest = new QTimer(this);
            connect(timerTest, SIGNAL(timeout()), this, SLOT(slots_timerTest()));  
            if(m_sSystemInfo.bUDPSoc)
            {
                m_udpsocket = new CUdpsocket(this);
                m_udpsocket->ConnectToServer(m_sSystemInfo.strVEXIIP, m_sSystemInfo.nVEXIPort);
                m_udpsocket->SetIOCardHandle(m_vIOCard[0]);
            }
        }
    }
}
//初始化算法
void SystemUIMain::InitCheckSet()
{
    //算法初始化，模板调入等 [8/4/2010 GZ]
    s_Status  sReturnStatus;
    s_AlgInitParam   sAlgInitParam;    
    //2021-6-16:G 算法模板不检查是否是0,如果是0,算法模块会崩溃.
    if(m_sSystemInfo.iRealCamCount == 0)
    {
        sReturnStatus.nErrorID = RETURN_OK;
        m_sSystemInfo.m_bLoadModel =  TRUE;  //成功载入上一次的模板
    }
    else if(m_sSystemInfo.m_iMaxCameraImageHeight != 0 && m_sSystemInfo.m_iMaxCameraImageWidth != 0)
    {
        if(m_sSystemInfo.m_iMaxCameraImageWidth>m_sSystemInfo.m_iMaxCameraImageHeight)
        {
            sReturnStatus = init_bottle_module(m_sSystemInfo.m_iMaxCameraImageWidth,m_sSystemInfo.m_iMaxCameraImageWidth,1);
        }
        else
        {
            sReturnStatus = init_bottle_module(m_sSystemInfo.m_iMaxCameraImageHeight,m_sSystemInfo.m_iMaxCameraImageHeight,1);
        }
    }
    else
    {
        sReturnStatus.nErrorID = -1;
    }
    // 算法初始化，模板调入等 [8/4/2010 GZ]
    //////////////////////////////////////////////////////////////////////////

    if (CherkerAry.pCheckerlist != NULL)
    {
        delete[] CherkerAry.pCheckerlist;
    }
    CherkerAry.iValidNum = m_sSystemInfo.iRealCamCount;
    CherkerAry.pCheckerlist = new s_CheckerList[CherkerAry.iValidNum];
    
    if (sReturnStatus.nErrorID != RETURN_OK)
    {
        CLogFile::write(tr("----load model error----"), AbnormityLog);
        return;
    }    
    for (int i=0;i<m_sSystemInfo.iRealCamCount;i++)
    {
        sAlgInitParam.nCamIndex=i;
        sAlgInitParam.nModelType = m_sRealCamInfo[i].m_iImageType;  //检测类型
        sAlgInitParam.nWidth = m_sRealCamInfo[i].m_iImageWidth; 
        sAlgInitParam.nHeight =  m_sRealCamInfo[i].m_iImageHeight;
        memset(sAlgInitParam.chCurrentPath,0,MAX_PATH);

        strcpy_s(sAlgInitParam.chCurrentPath,m_sConfigInfo.m_sAlgFilePath.toLocal8Bit()); 
        memset(sAlgInitParam.chModelName,0,MAX_PATH); //模板名称
        strcpy_s(sAlgInitParam.chModelName,m_sSystemInfo.m_strModelName.toLocal8Bit()); 
        sReturnStatus = m_cBottleCheck[i].init(sAlgInitParam);

        if (sReturnStatus.nErrorID != RETURN_OK && sReturnStatus.nErrorID != 1)
        {
            CLogFile::write(tr("----camera%1 load model error----").arg(i),AbnormityLog);
            return;
        }        
        if (sReturnStatus.nErrorID == 1) //模板为空
        {
            //模板为空
            m_sSystemInfo.m_bLoadModel =  FALSE;  //如果模板为空，则不能检测 
        }
        else
        {
            m_sSystemInfo.m_bLoadModel =  TRUE;  //成功载入上一次的模板
        }
        // 旋转类 [12/10/2010]
        sAlgInitParam.nModelType = 99;  //检测类型
        memset(sAlgInitParam.chModelName,0,MAX_PATH); //模板名称
        m_cBottleRotate[i].init(sAlgInitParam);
        sAlgInitParam.nModelType = 98;  //检测类型
        m_cBottleStress[i].init(sAlgInitParam);
    }
}
//开启相机采集
void SystemUIMain::StartCamGrab()
{ 
    for (int i = 0;i<m_sSystemInfo.iRealCamCount;i++)
    {
        m_sRealCamInfo[i].m_pGrabber->StartGrab();
        m_sRealCamInfo[i].m_bGrabIsStart=TRUE;
    }
    //屏蔽系统键 [4/20/2011 lly]
    //OnDisableTaskKeys(TRUE);
}
void SystemUIMain::slots_CameraOffLine(bool bIsOffLine)
{
    if (bIsOffLine)
    {
        QList<int> listOfflineCameraNumber;
        QString strTemp;
//        pThreadCameraOffLineMonitor->getOfflineCameraMarkNumber(listOfflineCameraNumber);
        int nOffLineCameraCount = listOfflineCameraNumber.size();
        if (nOffLineCameraCount>0)
        {
            for (int i=0; i<nOffLineCameraCount; i++ )
            {
                int nOfflineCameraNumber = listOfflineCameraNumber.at(i);
                if (nOfflineCameraNumber<m_sSystemInfo.iRealCamCount)
                {
                    cameraStatus_list.at(nOfflineCameraNumber)->SetCameraStatus(1);
                    if (0 == i)
                    {
                        strTemp = tr("Off line Camera Number: %1").arg(nOfflineCameraNumber);
                    }
                    else
                    {
                        strTemp += tr(", %1").arg(nOfflineCameraNumber);
                    } 
                }
            }
            QMessageBox::information(this,tr("ERROR"),strTemp + ".");
        }
        else
        {
            if (!m_sSystemInfo.m_iSimulate )
            {
                CLogFile::write(tr("Wrong camera off line Alarm!"),AbnormityLog);
            }
        }
    }
}
void SystemUIMain::slots_MessageBox(QString strMessageBox)
{
    QMessageBox::information(this,tr("Information"),strMessageBox);
}

int SystemUIMain::deviceType(int i)
{
    if(!m_sDeviceType.contains(i))
    {
        return EDTCameraFinishBottom;
    }
    return m_sDeviceType[i];
}

QList<int> SystemUIMain::getDeviceList(int t)
{
    QList<int> idLst;
    foreach(int i, m_sDeviceType.keys())
    {
        if(t == m_sDeviceType[i])
        {
            idLst.append(i);
        }
    }
    return idLst;
}

bool SystemUIMain::getCameraShowCarveImage(int nCamNo)
{
    if(!m_sDeviceType.contains(nCamNo) || m_sDeviceType[nCamNo] == (int)EDTCameraFinishBottom)
    {
        return widget_finishBottom->getCarveImageShow();
    }
    else if(m_sDeviceType[nCamNo] == (int)EDTCameraOvality)
    {
        return widget_ovality->getCarveImageShow();
    }
    return false;
}

void SystemUIMain::slots_clearAllData()
{
    widget_count->ClearAllData();
    //清除Qwt曲线
    info_widget->dXData.clear();
    info_widget->dTotalNumber.clear();
    info_widget->dFailureNumber.clear();
    for (int i = 0; i<=m_sErrorInfo.m_iErrorTypeCount;i++)
    {
        info_widget->dFailureNumberByErrorType[i].clear();
    }
    info_widget->dFailureRate.clear();
    info_widget->slots_SaveCurveInfo();

    if(buttonVisible[PGLCamReadMoldPage])
        widget_LinearCamera->slots_clear();

    if(buttonVisible[PGOvalityPage])
        widget_ovality->clearData(true);
    QMutexLocker lk(&recordLock);
    record.reset();
}

void SystemUIMain::bindMoldID(int imgSN, int moldID)
{
    QMutexLocker lk(&mxImgMould);
    imgSNModeIDs[imgSN] = moldID;
    emit signals_updateMold(moldID);
}

void SystemUIMain::SaveCameraExporeAndTrig(int camNO, int expore, int trig)
{
    QSettings iniCameraSet(m_sConfigInfo.m_strGrabInfoPath,QSettings::IniFormat);
    QString strShuter,strTrigger;
    strShuter = QString("/Shuter/Grab_%1").arg(camNO);
    strTrigger = QString("/Trigger/Grab_%1").arg(camNO);
    m_sCarvedCamInfo[camNO].m_iShuter = expore;
    m_sCarvedCamInfo[camNO].m_iTrigger = trig;

    iniCameraSet.setValue(strShuter,QString::number(expore));
    iniCameraSet.setValue(strTrigger,QString::number(trig));
}

//开启检测线程
void SystemUIMain::StartDetectThread()
{
    m_bIsThreadDead = FALSE;
    for (int i=0;i<m_sSystemInfo.iRealCamCount;i++)
    {
        pdetthread[i]->start();
    }
}
void SystemUIMain::initDetectThread()
{
    m_bIsThreadDead = FALSE;
    for (int i=0;i<m_sSystemInfo.iRealCamCount;i++)
    {
        m_sRealCamInfo[i].m_hReceiveLocInfo = CreateEvent(NULL, FALSE, TRUE, NULL);  
        m_sRealCamInfo[i].m_hSendLocInfo = CreateEvent(NULL, FALSE, FALSE, NULL);  
    }
    for (int i=0;i<m_sSystemInfo.iRealCamCount;i++)
    {
        pdetthread[i] = new DetectThread(this);
        pdetthread[i]->ThreadNumber = i;
    }
}

void SystemUIMain::initDataCenter()
{
#ifndef NO_MOULD_COUNT
    dbCenter = new DataCenter;
    QObject::connect(dbCenter, SIGNAL(GetDataCenterConf(DataCenterConf&)), this, SLOT(slots_getDataCenterConf(DataCenterConf&)));

    uiCfg.bShowBarChart = false;
    uiCfg.iDefaultShow = ERT_Shift;
    uiCfg.iLanguage = 0;
    uiCfg.iShowStyle = ESS_SensorID;
    uiCfg.sCodetsprefix = "DataViewMerge";

    dcConf.eTimeInterval = (m_sSystemInfo.iSaveRecordInterval == 30)?ETI_30_Min:ETI_60_Min;
    dcConf.syscfg.iDaysDataOutDate = 60;
    dcConf.syscfg.iProtocolVersion = 0;
    dcConf.syscfg.iTimeInterval_GeneRecord = 5;
    dcConf.syscfg.iTimeInterval_GetSrcData = 30;
    QStringList shifts;
    if(m_sSystemInfo.shift1.isValid() )
    {
        shifts.append(m_sSystemInfo.shift1.toString("hh:mm:ss"));
        if(m_sSystemInfo.shift2.isValid() )
        {
            shifts.append(m_sSystemInfo.shift2.toString("hh:mm:ss"));
            if(m_sSystemInfo.shift3.isValid() )
            {
                shifts.append(m_sSystemInfo.shift3.toString("hh:mm:ss"));
            }
        }
        if(!shifts.isEmpty())
        {
            dcConf.shifts.reset(shifts);
        }
    }
    QString err;
    if(!dbCenter->Init(&err))
    {
        CLogFile::write(err, AbnormityLog, 0);
        dbInit = false;
        return;
    }
    connect(this, SIGNAL(signals_saveRecord(const Record&, QString*)), dbCenter, SLOT(AddRecord(const Record&, QString*)));
    dbInit = true;
#endif
}

//初始化界面
void SystemUIMain::initInterface()
{
    //初始化标题栏设置
    if(getDeviceList(EDTCameraFinishBottom).isEmpty())
    {
        buttonVisible[PGImagePage] = false;
    }
    if(getDeviceList(EDTCameraOvality).isEmpty())
    {
        buttonVisible[PGOvalityPage] = false;
    }
    if(getDeviceList(EDTCameraLineMatrix).isEmpty())
    {
        buttonVisible[PGLCamReadMoldPage] = false;
    }
    if(!buttonVisible[PGImagePage] && !buttonVisible[PGOvalityPage] && !buttonVisible[PGLCamReadMoldPage])
    {
        buttonVisible[PGAlgPage] = false;
    }
    if(getDeviceList(EDTThickness).isEmpty())
    {
        buttonVisible[PGThicknessPage] = false;
        sPermission.iThickness = 1;
    }

    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog  | Qt::WindowSystemMenuHint);//去掉标题栏
    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();
    setMinimumSize(screenRect.width(),screenRect.height());

    QIcon icon;
    icon.addFile(QString::fromUtf8(":/sys/icon"), QSize(), QIcon::Normal, QIcon::Off);
    setWindowIcon(icon);

    statked_widget = new QStackedWidget();
    statked_widget->setObjectName("mainStacked");
    //裁剪图像
    widget_finishBottom = new Widget_FinishBottom(this);
    //品种管理
    widgetManagement = new WidgetManagement(this);
    //系统设置界面
    test_widget = new WidgetTest(this);
    test_widget->m_lastAllNumber=m_sRunningInfo.m_checkedNum;
    widgetInfoContainer = new QWidget(this);
    //主界面
    info_widget = new widget_info(widgetInfoContainer);
    //用户权限管理界面
    widget_UserManege = new UserManegeWidget(widgetInfoContainer);
    //椭圆度界面
    widget_ovality = new WidgetOvality(this);
    widgetThick = new ThickNess;

    QHBoxLayout *layoutWidgetInfoContainer = new QHBoxLayout(widgetInfoContainer);
    layoutWidgetInfoContainer->addWidget(widget_UserManege);
    layoutWidgetInfoContainer->addWidget(info_widget);
    
    widget_UserManege->setVisible(false);
    //统计界面
    widget_count = new Widget_Count(this);
    widget_Warning = new WidgetWarning(this);
    //算法设置界面
    widget_alg = new QWidget(this);
    widget_alg->setObjectName("widget_alg");
    widget_LinearCamera = new Widget_LinearCamera(this);

    //导航栏
    title_widget = new WidgetTitle(this);

    QPalette palette;
    palette.setBrush(QPalette::Window, QBrush(Qt::white));
    statked_widget->setPalette(palette);
    statked_widget->setAutoFillBackground(true);
    statked_widget->addWidget(widgetInfoContainer);
    statked_widget->addWidget(widget_count);
    statked_widget->addWidget(widget_finishBottom);
    statked_widget->addWidget(widget_ovality);
    statked_widget->addWidget(widget_LinearCamera);
    statked_widget->addWidget(widgetThick);
    statked_widget->addWidget(widget_alg);
    statked_widget->addWidget(test_widget);
    statked_widget->addWidget(widgetManagement);

    //状态栏
    stateBar = new QWidget(this);
    stateBar->setFixedHeight(40);
    QGridLayout* gridLayoutStatusLight = new QGridLayout;
    for (int i = 0;i<m_sSystemInfo.iDevTotalCount;i++)
    {
        CameraStatusLabel *cameraStatus = new CameraStatusLabel(stateBar);
        cameraStatus->setObjectName("toolButtonCamera");
        cameraStatus->setAlignment(Qt::AlignCenter);
        cameraStatus->setText(QString::number(i+1));
        connect(this, SIGNAL(signals_exitSystem()), cameraStatus, SLOT(slots_exit()));
        cameraStatus_list.append(cameraStatus);
        //hLayoutButton->addWidget(cameraStatus);
        if (0 == m_sSystemInfo.m_iImageStyle)
        {
            gridLayoutStatusLight->addWidget(cameraStatus,i%2,i/2);
        }
        else if (1 == m_sSystemInfo.m_iImageStyle)
        {
            if (i < (m_sSystemInfo.iRealCamCount+1)/2)
            {
                gridLayoutStatusLight->addWidget(cameraStatus,0,i);
            }
            else
            {
                gridLayoutStatusLight->addWidget(cameraStatus ,1,i - (m_sSystemInfo.iRealCamCount+1)/2);
            }
        }
    }
    //检查相机状态
    for (int i = 0; i < m_sSystemInfo.iRealCamCount; i++)
    {
        int iRealCameraSN = m_sCarvedCamInfo[i].m_iToRealCamera;
        CameraStatusLabel *cameraStatus = cameraStatus_list.at(i);
        if (!m_sRealCamInfo[iRealCameraSN].m_bCameraInitSuccess)
        {
            cameraStatus->SetCameraStatus(1);
        }
        else
        {
            cameraStatus->SetCameraStatus(0);
        }
    }

    labelCoder = new QLabel(stateBar);
    labelVersin=new QLabel(stateBar);
    //根据程序编译器,显示第三位是32还是64
    QString sVer = GetLocalPogramVersion();
    if(!sVer.isEmpty())
    {
        QStringList tstrl = sVer.split(".",QString::SkipEmptyParts);
        QString verRes = "  V:";
        for(int i = 0; i < tstrl.size();i++)
        {
            if(i == 1)
            {
#ifdef WIN_32_1_10
                verRes += "32";
#else
                verRes += "64";
#endif  
            }
            else
            {
                verRes += tstrl[i];
            }
            verRes += ".";
        }  
        verRes.left(verRes.size() - 1);
        labelVersin->setText(verRes);
    }
    labelCoder->setText(tr("Signal Status"));

    QFont fontCoder;
    fontCoder.setPixelSize(28);
    fontCoder.setPixelSize(18);
    labelCoder->setFont(fontCoder);
    labelVersin->setFont(fontCoder);
    timerUpdateCoder = new QTimer(this);
    timerUpdateCoder->setInterval(200);
    timerUpdateCoder->start();  

    QHBoxLayout* hLayoutStateBar = new QHBoxLayout(stateBar);
    hLayoutStateBar->addLayout(gridLayoutStatusLight);
    hLayoutStateBar->addStretch();
    hLayoutStateBar->addWidget(labelCoder);
    hLayoutStateBar->addWidget(labelVersin);
    hLayoutStateBar->setSpacing(3);
    hLayoutStateBar->setContentsMargins(10, 0, 10, 0);

    QHBoxLayout *center_layout = new QHBoxLayout();
    center_layout->addWidget(statked_widget);
    center_layout->setSpacing(0);
    center_layout->setContentsMargins(5,5,5,5);

    QVBoxLayout *main_layout = new QVBoxLayout();

    main_layout->addWidget(title_widget);
    main_layout->addLayout(center_layout);
    main_layout->addWidget(stateBar);
    main_layout->setSpacing(0);
    //     main_layout->setContentsMargins(SHADOW_WIDTH, SHADOW_WIDTH, SHADOW_WIDTH, SHADOW_WIDTH);
    main_layout->setContentsMargins(0, 0, 0, 0);

    setLayout(main_layout);

    connect(test_widget,SIGNAL(signals_sendAlarm(int, QString)),widget_Warning,SLOT(slots_ShowWarning(int, QString)));
    connect(test_widget,SIGNAL(signals_hideAlarm(int)),widget_Warning,SLOT(slots_HideWarning(int)));
    connect(title_widget, SIGNAL(showMin()), this, SLOT(showMinimized()));
    connect(title_widget, SIGNAL(closeWidget()), this, SLOT(slots_OnExit()));
    connect(title_widget, SIGNAL(turnPage(int)), this, SLOT(slots_turnPage(int)));

    connect(this,SIGNAL(signals_intoCountWidget()),widget_count,SLOT(slots_intoWidget()));    
    connect(this,SIGNAL(signals_intoInfoWidget()),info_widget,SLOT(slots_intoWidget()));    
    connect(this,SIGNAL(signals_intoCarveSettingWidget()),widget_finishBottom,SLOT(slots_intoWidget()));    
    connect(this,SIGNAL(signals_intoManagementWidget()),widgetManagement,SLOT(slots_intoWidget()));    
    connect(this,SIGNAL(signals_intoTestWidget()),test_widget,SLOT(slots_intoWidget()));    
    connect(this,SIGNAL(signals_intoOvalityWidget()),widget_ovality,SLOT(slots_intoWidget()));

    connect(this,SIGNAL(signals_clear()),this, SLOT(slots_clearAllData()));    
    connect(this,SIGNAL(signals_ShowWarning(int , QString )),widget_Warning,SLOT(slots_ShowWarning(int , QString )));    
    connect(this,SIGNAL(signals_HideWarning(int)),widget_Warning,SLOT(slots_HideWarning(int)));    
    connect(this,SIGNAL(signals_intoLineraCaWidget()),widget_LinearCamera,SLOT(slots_intoWidget()));

    connect(this, SIGNAL(signals_exitSystem()), widget_finishBottom, SIGNAL(signals_exitSystem()));
    connect(this, SIGNAL(signals_exitSystem()), widget_ovality, SIGNAL(signals_exitSystem()));
    connect(this, SIGNAL(signals_exitSystem()), test_widget, SLOT(slots_exit()));
    connect(this, SIGNAL(signals_exitSystem()), info_widget, SLOT(slots_exit()));
    connect(this, SIGNAL(signals_exitSystem()), widget_count, SLOT(slots_exit()));
    connect(this, SIGNAL(signals_exitSystem()), widget_Warning, SLOT(slots_exit()));

    connect(info_widget, SIGNAL(signals_updateInfo()), widget_count, SLOT(slots_updateInfo()));

    connect(timerUpdateCoder, SIGNAL(timeout()), this, SLOT(slots_UpdateCoderNumber()));   

     for (int i = 0;i<m_sSystemInfo.iRealCamCount;i++)
    {
        connect(pdetthread[i], SIGNAL(signals_upDateCamera(int,int)), this, SLOT(slots_updateCameraState(int,int)));
    }   
    connect(widget_finishBottom, SIGNAL(signals_SetCameraStatus(int,int)), this, SLOT(slots_SetCameraStatus(int,int)));
    connect(widget_ovality, SIGNAL(signals_SetCameraStatus(int,int)), this, SLOT(slots_SetCameraStatus(int,int)));
    connect(info_widget,SIGNAL(signals_ShowWarning(int , QString )),widget_Warning,SLOT(slots_ShowWarning(int , QString )));    
    connect(info_widget,SIGNAL(signals_HideWarning(int)),widget_Warning,SLOT(slots_HideWarning(int)));    
    connect(widget_Warning, SIGNAL(signals_PauseAlert()), info_widget, SLOT(slots_PauseAlert()));
    connect(widgetManagement, SIGNAL(signals_currentMouldChanged(QString)), widget_ovality, SLOT(slots_changeMould(QString)));
    connect(this, SIGNAL(signals_CombineMoldNumKickOut(int)), widget_LinearCamera, SLOT(slots_onMoldNumKickOut(int)));
    if(buttonVisible[PGLCamReadMoldPage])
    {   
        connect(this,SIGNAL(signals_updateMold(int)),widget_LinearCamera,SLOT(slots_updateLatestMoldNumber(int)));    
    }
    if(buttonVisible[PGThicknessPage])
    {
        connect(this, SIGNAL(signals_bottleReady(int)), widgetThick, SLOT(slots_bottleReady(int)));
        connect(widgetThick, SIGNAL(signals_thickParamsUpdated(int , ThicknessParameter)), this, SLOT(slots_thicknessParamUpdated(int , ThicknessParameter)));
        connect(widgetThick, SIGNAL(signals_thickConfigUpdated(ThicknessConfig)), this, SLOT(slots_thicknessConfigUpdated(ThicknessConfig)));
        connect(this, SIGNAL(signals_thickStateChanged(int)), widgetThick, SLOT(slots_deviceStateChanged(int)));
        foreach(auto thkThd, m_TReadThickNessList.values())
        {
            connect(thkThd, SIGNAL(signals_SendThickNessData(int , QVector<qreal>,int,int )),  widgetThick, SLOT(slots_PaintPicture(int,  QVector<qreal>,int,int)));
            connect(thkThd, SIGNAL(signals_SetResult(int , int , int , double , double , double )), widgetThick, SLOT(slots_UpdateResult(int , int , int , double , double, double )));
        }
    }

    iLastPage = e_CurrentMainPage(PGHomePage);
    if(buttonVisible[PGLCamReadMoldPage])
    {
        slots_turnPage(PGLCamReadMoldPage);
    }
    slots_turnPage(PGHomePage);
    skin.fill(QColor(90,90,90,120));

    if(sPermission.iFinBot == 0 && sPermission.iOva == 0)
    {
        sPermission.iAlgSet = 0;
    }

    if (m_sRunningInfo.m_iPermission == 0)
    {
        title_widget->slots_permiss(sPermission);
        widget_finishBottom->slots_setCarveButtonVisible(false);
        widget_ovality->slots_setCarveButtonVisible(false);
    }
}
void SystemUIMain::slots_UpdateCoderNumber()
{
    int nSensorCounter = 0, nCodeNum=0, nSignNum=0, nSignNum1=0;
    if (m_sSystemInfo.m_bIsIOCardOK)
    {
        m_vIOCard[0]->m_mutexmIOCard.lock();
        nCodeNum = m_vIOCard[0]->ReadCounter(13);   //读编码器位置

        nSignNum = m_vIOCard[0]->ReadCounter(4);//从接口卡读剔废数,荣泰用的out0来计数 basler发送踢废信号就计数
        m_vIOCard[0]->m_mutexmIOCard.unlock();
        m_sRunningInfo.m_mutexRunningInfo.lock();
        if(nTestDevID == -1)
        {
            if ((nSignNum - m_sRunningInfo.m_kickoutNumber > 0) && (nSignNum - m_sRunningInfo.m_kickoutNumber < 50))
            {
                m_sRunningInfo.m_failureNumFromIOcard = m_sRunningInfo.m_failureNumFromIOcard + nSignNum - m_sRunningInfo.m_kickoutNumber;
            }
            m_sRunningInfo.m_kickoutNumber = nSignNum;

            int nIOCard1Checked = m_vIOCard[0]->ReadCounter(0);
            if (nIOCard1Checked > m_sRunningInfo.m_iLastIOCard1IN0)
            {
                m_sRunningInfo.m_checkedNum += nIOCard1Checked - m_sRunningInfo.m_iLastIOCard1IN0;
            }
            m_sRunningInfo.m_iLastIOCard1IN0 = nIOCard1Checked;
        }
        m_sRunningInfo.m_mutexRunningInfo.unlock();
    }

    QString strValue,strCounter,strEncoder,strTime;
    strCounter = QString(tr("Sensor Counter")+":%1").arg(nSensorCounter);
    strEncoder = QString(tr("Coder Number")+":%1").arg(nCodeNum);
    strTime = QTime::currentTime().toString();
    //计算模点失败率并显示
    double num = 0.00;
    if (m_sRunningInfo.m_checkedNum != 0)
    {
        num = ((double)(m_sRunningInfo.m_checkedNum - m_sRunningInfo.nModelReadFailed) / m_sRunningInfo.m_checkedNum) * 100;
    }
    labelCoder->setText(tr("Speed:%1    Rate Of Mould: %2%   %3 %4").arg(m_sRunningInfo.strSpeed).arg(QString::number(num,10, 2)).arg(strEncoder).arg(strTime));
    if(buttonVisible[PGLCamReadMoldPage])
    {
        widget_LinearCamera->slots_updateCounts();
    }
#ifndef NO_MOULD_COUNT
    if(!dbInit)
        return;
    static QDateTime tSaveRe = QDateTime::currentDateTime();
    if(tSaveRe.secsTo(QDateTime::currentDateTime()) < 5)
    {
        return;
    }
    Record _tRe;
    {
        QMutexLocker lk(&recordLock);
        if(record.inspected == 0)
        {
            return;
        }

        _tRe = record;
        _tRe.dt_end = QDateTime::currentDateTime();
        record.reset();
    }
    tSaveRe = QDateTime::currentDateTime();
    emit signals_saveRecord(_tRe);
#endif
}
void SystemUIMain::slots_updateCameraState(int nCam,int mode)
{
    cameraStatus_list.at(nCam)->BlinkCameraStatus(mode);
}
void SystemUIMain::slots_SetCameraStatus(int nCam,int mode)
{
    cameraStatus_list.at(nCam)->SetCameraStatus(mode);
}
//裁剪
void SystemUIMain::CarveImage(uchar* pRes,uchar* pTar,int iResWidth,int iResHeight,int iTarX,int iTarY,int iTarWidth,int iTarHeight)
{
    try
    {
        uchar* pTemp = pTar;
        uchar* pTempRes = pRes+iResWidth*(iTarY)+iTarX;
        for(int i = 0; i < iTarHeight; i++)
        {
            memcpy(pTemp,pTempRes,iTarWidth);
            pTemp += iTarWidth;
            pTempRes += iResWidth;
        }
        int i=1;
    }
    catch(...)
    {
        CLogFile::write(tr("Error in image carve "),AbnormityLog);
    }
}
//开启IO卡线程
void SystemUIMain::StartIOCardThread()
{
//     m_bIsIOCardThreadDead = FALSE;
//     pIOCardThread = new IOCardThread(this);
//     pIOCardThread->start();
}

void SystemUIMain::slots_turnPage(int current_page, int iPara)
{
    if (iLastPage == current_page)
    {
        return;
    }
    if (current_page < PGThicknessPage && current_page >= PGHomePage)
    {
        if (!leaveWidget())
        {
            return;
        }
    }
    switch (current_page)
    {
    case PGHomePage:
    case PGCountPage:
    case PGImagePage:
    case PGOvalityPage:
    case PGThicknessPage:
    case PGAlgPage:
    case PGLCamReadMoldPage:
    case PGSettingsPage:
    case PGManagementSettingPage:
        slots_trigUpdate(current_page, iPara);
        iLastPage = current_page;
        break;
    case PGClearButton:
        {
            if (QMessageBox::No == QMessageBox::question(this,tr("Exit"),
                tr("Are you sure to clear?"),
                QMessageBox::Yes | QMessageBox::No))    
            {
                return ;
            }
            emit signals_clear();
        }
        break;
    case PGStartOrStopButton:
        slots_OnBtnStart();
        break;
    case PGExitButton:
        slots_OnExit();
        break;
    default:
        break;
    }
}
bool SystemUIMain::leaveWidget()
{
    switch((e_CurrentMainPage(iLastPage)))
    {
    case PGHomePage:
        if (!info_widget->leaveWidget())
        {
            return false;
        }
        break;
    case PGCountPage:
        if (!widget_count->leaveWidget())
        {
            return false;
        }
        break;
    case PGImagePage:
        if (!widget_finishBottom->leaveWidget())
        {
            return false;
        }

        break;
    case PGOvalityPage:
        widget_ovality->leaveWidget();
        break;
    case PGAlgPage:
        s_Status  sReturnStatus;
        sReturnStatus = m_cBottleModel.CloseModelDlg();

        if (0 != sReturnStatus.nErrorID)
        {
            return false;
        }
        break;
    case PGLCamReadMoldPage:
        if (!widget_LinearCamera->leaveWidget())
        {
            return false;
        }

        break;
    case PGSettingsPage:
        if (!test_widget->leaveWidget())
        {
            return false;
        }
        break;
    case PGManagementSettingPage:
        if (!widgetManagement->leaveWidget())
        {
            return false;
        }
        break;
    }
    title_widget->button_list.at(iLastPage)->setMousePress(false);
    return true;
}
void SystemUIMain::slots_OnBtnStart()
{//所有界面启动检测,都执行此处的功能,确保执行的逻辑一致
    if (m_sSystemInfo.m_bIsTest)
    {
        QMessageBox::information(this,tr("Information"),tr("Please Stop Test First!current DevID:%1").arg(nTestDevID+1));
        return;
    }
    ToolButton *TBtn = title_widget->button_list.at(PGStartOrStopButton);

    if (!m_sRunningInfo.m_bCheck )//开始检测
    {
        if (!m_sSystemInfo.m_bLoadModel)
        {
            QMessageBox::information(this,tr("Error"),tr("No Model,Please Load Model!"));
            return;
        }
        //图像综合清零
        m_cCombine.m_MutexCombin.lock();
        m_cCombine.RemovAllResult();
        m_cCombine.RemovAllError();
        m_cCombine.m_MutexCombin.unlock();
        //确认壁厚设备启动,如果没启动,则启动所有壁厚
        if(!CheckAndStartThick())
            return;
        // 使能接口卡
        if (m_sSystemInfo.m_bIsIOCardOK)
        {
            for (int i = 0; i< m_sSystemInfo.iIOCardCount;i++)
            {
                CLogFile::write(QString(tr("OpenIOCard%1")).arg(i),OperationLog,0);
                m_vIOCard[i]->enable(true);
            }
            if(m_udpsocket != nullptr)
            {
                m_udpsocket->start();
            }
        }
        for (int i = 0;i<=m_sSystemInfo.iRealCamCount;i++)
        {
            m_sRealCamInfo[i].m_iImageIdxLast = -1;
        }

        m_sRunningInfo.m_bCheck = true;

        CLogFile::write(tr("Start Check"),OperationLog);
        QPixmap pixmap(":/toolWidget/stop");
        TBtn->setText(tr("Stop detect"));
        TBtn->setIcon(pixmap);
        TBtn->bStatus = true;
        if (!sPermission.iStartStop)
        {
            TBtn->setDisabled(true);
        }
        else
        {
            TBtn->setDisabled(false);
        }
    }
    else if (m_sRunningInfo.m_bCheck)//停止检测
    {
        if (m_sSystemInfo.m_bIsIOCardOK)
        {
            if(m_udpsocket != nullptr)
            {
                m_udpsocket->stop();
            }
            for (int i = 0; i< m_sSystemInfo.iIOCardCount;i++)
            {
                CLogFile::write(QString(tr("CloseIOCard%1")).arg(i),OperationLog,0);
                m_vIOCard[i]->enable(false);
            }
        }
        // 停止算法检测 
        m_sRunningInfo.m_bCheck = false;
        CLogFile::write(tr("Stop Check"),OperationLog);
        QPixmap pixmap(":/toolWidget/start");
        TBtn->setText(tr("Start detect"));
        TBtn->setIcon(pixmap);
        TBtn->bStatus = false;
        
        for(int ins = 0; ins < m_sSystemInfo.iRealCamCount; ins++)
        {
            for (int i=0;i<256;i++)
            {
                pdetthread[ins]->isShowPicture[i]=0;
            }
        }
    }
}

void SystemUIMain::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::lightGray);
    painter.drawPixmap(QRect(0, 0, this->width(), this->height()), QPixmap(skin));
    //painter.drawPixmap(QRect(0, 0, this->width(), this->height()), QPixmap(skin_name));
}
//弹出提示信息对话框
void SystemUIMain::slots_MessageBoxMainThread(s_MSGBoxInfo msgbox)
{
    QMessageBox::information(this,msgbox.strMsgtitle,msgbox.strMsgInfo);    
}
//释放IO卡
void  SystemUIMain::ReleaseIOCard()
{
    if(!m_sSystemInfo.m_bIsIOCardOK)
    {
        return;
    }
    timerTest->stop();
    CLogFile::write(tr("CloseIOCard"),OperationLog);
    if(pThreadBaseClean != nullptr)
    {
        pThreadBaseClean->setBaseCleanParam(false,0, 9);
        pThreadBaseClean->wait(100);
        delete pThreadBaseClean;
        pThreadBaseClean = nullptr;
    }
    if(m_udpsocket != nullptr)
    {
        m_udpsocket->stop();
        delete m_udpsocket;
        m_udpsocket = nullptr;
    }
    for (int i=0;i<m_sSystemInfo.iIOCardCount;i++)
    {
        if (m_sSystemInfo.m_bIsIOCardOK && m_vIOCard[i] != nullptr)
        {
            m_vIOCard[i]->CloseIOCard();
            delete m_vIOCard[i];
            m_vIOCard[i] = nullptr;
        }
    }
}
// 关闭相机 [11/11/2010 zhaodt]
void SystemUIMain::CloseCam()
{
    CLogFile::write(tr("CloseCam"),OperationLog);

    for (int i=0;i<m_sSystemInfo.iRealCamCount;i++)
    {
        if (m_sRealCamInfo[i].m_bCameraInitSuccess && m_sRealCamInfo[i].m_bGrabIsStart) 
        {
            m_sRealCamInfo[i].m_pGrabber->StopGrab();
            m_sRealCamInfo[i].m_bGrabIsStart=FALSE;// 是否开始采集状态
        }
    }    
    Sleep(100);
    for (int i=0;i<m_sSystemInfo.iRealCamCount;i++)
    {
        if (m_sRealCamInfo[i].m_bCameraInitSuccess)
        {
            m_sRealCamInfo[i].m_pGrabber->Close();
        }
    }
}
//释放图像资源
void SystemUIMain::ReleaseImage()
{
    CLogFile::write(tr("ReleaseImage"),OperationLog);

    for(int i = 0 ; i < m_sSystemInfo.iRealCamCount; i++)
    {
        delete m_sRealCamInfo[i].m_pRealImage;
        m_sRealCamInfo[i].m_pRealImage = NULL;

        delete m_sCarvedCamInfo[i].m_pActiveImage;
        m_sCarvedCamInfo[i].m_pActiveImage = NULL;

        delete[] m_sCarvedCamInfo[i].m_pGrabTemp;
        m_sCarvedCamInfo[i].m_pGrabTemp = NULL;
        m_queue[i].releaseMemory();
    }
    m_ErrorList.releaseElement();
}
//释放所有资源
void SystemUIMain::ReleaseAll()
{
    CLogFile::write(tr("Stop&kill check thread!"),OperationLog);
    
    //杀死检测线程
    if (!m_bIsThreadDead)
    {
        m_bIsThreadDead = TRUE;
    }
    if (!m_bIsIOCardThreadDead)
    {
        m_bIsIOCardThreadDead = TRUE;
    }
    //停止所有检测线程
    for (int i=0;i<m_sSystemInfo.iRealCamCount;i++)
    {
        pdetthread[i]->WaitThreadStop();
    }

    if (CherkerAry.pCheckerlist != NULL)
    {
        delete[] CherkerAry.pCheckerlist;
    }
    CloseCam();
    //释放接口卡
    ReleaseIOCard();
    CLogFile::write(tr("Release algorithm resource."),OperationLog);
    //释放算法参数
    for(int i = 0; i < m_sSystemInfo.iRealCamCount; i++)
    {
        //释放相机对应线程
        if(pdetthread[i] != NULL)
        {
            delete pdetthread[i];
            pdetthread[i]  = NULL;
        }
        s_Status sReturnStatus = m_cBottleCheck[i].Free();
        sReturnStatus = m_cBottleRotate[i].Free();
        sReturnStatus = m_cBottleStress[i].Free();

        for (int j = 0; j < 256;j++)
        {
            delete []m_sCarvedCamInfo[i].sImageLocInfo[j].m_AlgImageLocInfos.sXldPoint.nRowsAry;
            delete []m_sCarvedCamInfo[i].sImageLocInfo[j].m_AlgImageLocInfos.sXldPoint.nColsAry;
        }
    }

    ReleaseImage();
    CLogFile::write(tr("Release all cameras."),OperationLog);
    //释放相机
    for(int i = 0; i < m_sSystemInfo.iRealCamCount; i++)
    {
        if(m_sRealCamInfo[i].m_pGrabber != NULL)
        {//2021-6-17G:水星库[1.0.0.1_20170524]32位主动析构会崩溃,模拟[3.0.0.1]不会,因此取消析构,但是会因此析构时内存泄露
            if(m_sRealCamInfo[i].m_iGrabType != EGT_DH_MER)
            {
                delete m_sRealCamInfo[i].m_pGrabber;
            }
            m_sRealCamInfo[i].m_pGrabber = NULL;
        }
    }
    //释放壁厚
    CLogFile::write(tr("Release all thickness."),OperationLog);
    foreach(auto devid, thk_devid_index.keys())
    {
        m_ThickTypes[thk_devid_index[devid]]->CloseSensor();
        m_TReadThickNessList[devid]->wait();
        delete m_TReadThickNessList[devid];
        m_TReadThickNessList[devid] = NULL;
        delete m_ThickTypes[thk_devid_index[devid]];
        m_ThickTypes[thk_devid_index[devid]] = NULL;
    }
#ifndef NO_MOULD_COUNT
    CLogFile::write(tr("Release dbcenter."),OperationLog);
    if(dbCenter != nullptr)
    {
        dbCenter->Stop();
        delete dbCenter;
    }
#endif
}

void SystemUIMain::directoryChanged(QString path)
{
    QMessageBox::information(NULL, tr("Directory change"), path);
}

void SystemUIMain::slots_trigUpdate(int index, int iPara)
{
    switch(index)
    {
    case PGHomePage:
        emit signals_intoInfoWidget();
        CLogFile::write(tr("Into information page"),OperationLog,0);
        break;
    case PGCountPage:
        emit signals_intoCountWidget();
        CLogFile::write(tr("Into count page"),OperationLog,0);
        break;
    case PGImagePage:
        emit signals_intoCarveSettingWidget();
        CLogFile::write(tr("Into Image page"),OperationLog,0);
        break;
    case PGOvalityPage:
        emit signals_intoOvalityWidget();
        CLogFile::write(tr("Into Ovality page"),OperationLog,0);
        break;
    case PGLCamReadMoldPage:
        emit signals_intoLineraCaWidget();
        CLogFile::write(tr("Into Mode page"),OperationLog,0);
        break;
    case PGThicknessPage:
        emit signals_intoLineraCaWidget();
        CLogFile::write(tr("Into Thickness page"),OperationLog,0);
        break;
    case PGAlgPage:
        ShowCheckSet(iPara);
        break;
    case PGSettingsPage:
        emit signals_intoTestWidget();
        CLogFile::write(tr("Into system set page"),OperationLog,0);
        break;
    case PGManagementSettingPage:
        emit signals_intoManagementWidget();
        CLogFile::write(tr("Into management page"),OperationLog,0);
        break;
    default:
        break;
    }
    statked_widget->setCurrentIndex(index);
    title_widget->slots_selectCurrent(index);
    m_eCurrentMainPage = (e_CurrentMainPage)index;
    iLastPage = index;
}

bool SystemUIMain::slots_getImageLocaInfo(int nCamNO, QImage& img, s_AlgModelPara& para)
{
    bool bret = false;
    bool bInfo = false;
    s_AlgModelPara t = para;
    //如果窗体有图片(错误图或者正常图),则从窗体拿图片,否则从缓存队列拿图片和定位信息
    if(deviceType(nCamNO) == (int)EDTCameraFinishBottom)
    {
        bInfo = widget_finishBottom->slots_getImageLocaInfo(nCamNO, t);
    }
    else if(deviceType(nCamNO) == (int)EDTCameraOvality)
    {
        bInfo = widget_ovality->slots_getImageLocaInfo(nCamNO, t);
    }
    else if(deviceType(nCamNO) == (int)EDTCameraLineMatrix)
    {
        bInfo = widget_LinearCamera->slots_getImageLocaInfo(nCamNO, t);
    }
    if(bInfo)
    {
        if(m_SavePicture[nCamNO].pThat!=NULL )
        {
            img = m_SavePicture[nCamNO].m_Picture;
            bret = true;
        }
        else if(pThis!=NULL)
        {
            img = QImage(*pThis);
            bret = true;
        }
    }
    if(!bret)
    {
        m_queue[nCamNO].mLocker.lock();
        if(m_queue[nCamNO].listGrab.size()==0)
        {
            m_queue[nCamNO].mLocker.unlock();
            return false;
        }
        CGrabElement *pElement = m_queue[nCamNO].listGrab.last();
        img = QImage(*pElement->myImage);
        para.sImgLocInfo = pElement->sImgLocInfo;
        m_queue[nCamNO].mLocker.unlock();
    }
    else
    {
        para = t;
    }
    return true;
}

void SystemUIMain::slots_getUIConf(UIConfig& conf)
{
    conf = uiCfg;
}

void SystemUIMain::slots_getDataCenterConf(DataCenterConf& conf)
{
    conf = dcConf;
}

void SystemUIMain::slots_timerTest()
{
    if (m_sSystemInfo.m_bIsIOCardOK)
    {
        int iTesCameraNo = nTestDevID;
        int iCardid = 0;
        int iPort = 1;
        if(nTestDevID >= m_sSystemInfo.iRealCamCount)
        {//非相机设备
            if(nTestDevID > m_sSystemInfo.iDevTotalCount -1)
            {
                return;
            }
            if(deviceType(iTesCameraNo) == EDTThickness && thk_devid_index.contains(iTesCameraNo))
            {//检查壁厚
                iCardid = thickParams[thk_devid_index[iTesCameraNo]].m_cardID;
                iPort = thickParams[thk_devid_index[iTesCameraNo]].m_CheckStation;
            }
            else
            {
                //检查其他设备, 当前没其他设备
                return;
            }
        }
        else
        {
            iCardid = m_sRealCamInfo[iTesCameraNo].m_iIOCardSN;
            iPort = m_sRealCamInfo[iTesCameraNo].m_iGrabPosition;
        }

        if(iCardid < m_sSystemInfo.iIOCardCount && iPort > 0 && iPort <= 6)
        {
            m_vIOCard[iCardid]->TestOutPut(iPort);
        }
        else
        {
            CLogFile::write(tr("Cam:%1 invalid card params, card:%2 port:%3").arg(iTesCameraNo).arg(iCardid).arg(iPort),AbnormityLog);
        }
    }
}

void SystemUIMain::slots_thicknessConfigUpdated(ThicknessConfig cfg)
{
    thickConfig = cfg;
    SaveThicknessConfig();
}

void SystemUIMain::slots_thicknessParamUpdated(int devid, ThicknessParameter param)
{
    if(!thk_devid_index.contains(devid))
        return;
    int idx = thk_devid_index[devid];
    thickParams[idx] = param;
    SaveThicknessConfig();
}

void SystemUIMain::switchTest(int interval)
{
    if(interval <= 0)
    {//表示停止
        if(m_sSystemInfo.m_bIsIOCardOK)
            timerTest->stop();
        m_sSystemInfo.m_bIsTest = FALSE;
        nTestDevID=-1;
        m_sRunningInfo.m_bCheck = false;
        CLogFile::write(tr("Stop manually test."), OperationLog);
    }
    else{
        m_sSystemInfo.m_bIsTest = TRUE;
        m_sRunningInfo.m_bCheck=true;
        if(m_sSystemInfo.m_bIsIOCardOK){
            timerTest->setInterval(interval);
            timerTest->start();
        }
        CLogFile::write(tr("Start manually test."), OperationLog);
    }
}

bool SystemUIMain::ThickConnect(int devid)
{
    if(!m_ThickTypes.contains(devid))
        return false;
    int idx = thk_devid_index[devid];
    if(m_ThickTypes[devid]->GetSensorStatus() == ETST_Acq)
    {
        s_MSGBoxInfo info;
        info.strMsgtitle = tr("Wanning");
        info.strMsgInfo = tr("Please stop sensor%1 first, then could disconnect!").arg(devid);
        emit signals_MessageBoxMainThread(info);
        return false;
    }
    else if(m_ThickTypes[devid]->GetSensorStatus() == ETST_NotConn)
    {//连接设备
        char temps[255]={0};
        strcpy(temps, thickParams[idx].m_ConnStr.toStdString().c_str());
        if(!m_ThickTypes[devid]->ConnectSensor(temps))
        {
            s_MSGBoxInfo info;
            info.strMsgtitle = tr("Wanning");
            info.strMsgInfo = tr("Sensor%1 connect failed!").arg(devid);
            emit signals_MessageBoxMainThread(info);
            CLogFile::write(tr("Sensor%1 connect failed!").arg(devid), OperationLog);
            return false;
        }
        else
        {
            CLogFile::write(tr("Sensor%1 connect successfully!").arg(devid),OperationLog);
            emit signals_thickStateChanged(devid);
            return true;
        }
    }
    //else
    //关闭设备
    m_ThickTypes[devid]->CloseSensor();
    CLogFile::write(tr("Sensor%1 disconnect!").arg(devid),OperationLog);
    emit signals_thickStateChanged(devid);
    return true;
}

bool SystemUIMain::ThickDark(int devid)
{
    if(!m_ThickTypes.contains(devid))
        return false;
    int idx = thk_devid_index[devid];
    auto st = m_ThickTypes[devid]->GetSensorStatus();
    s_MSGBoxInfo info;
    info.strMsgtitle = tr("Wanning");
    if(st == ETST_Conn)
    {//正常状态
        return m_ThickTypes[devid]->SetDark();
    }
    else if(st == ETST_NotConn)
    {
        info.strMsgInfo = tr("Please connect sensor:%1 before dark!").arg(devid);
    }
    else
    {
        info.strMsgInfo = tr("Please stop check sensor:%1 before dark!").arg(devid);
    }
    emit signals_MessageBoxMainThread(info);
    return false;
}

bool SystemUIMain::ThickStartCheck(int devid)
{
    if(!m_ThickTypes.contains(devid))
        return false;
    int idx = thk_devid_index[devid];
    if(m_ThickTypes[devid]->GetSensorStatus() != ETST_Acq)
    {//未启动采集
        if(m_ThickTypes[devid]->GetSensorStatus() == ETST_NotConn)
        {//未连接设备
            s_MSGBoxInfo info;
            info.strMsgtitle = tr("Wanning");
            info.strMsgInfo = tr("Please connect Sensor%1 first.").arg(devid);
            emit signals_MessageBoxMainThread(info);
            CLogFile::write(tr("Sensor%1 start when hasn't connected!").arg(devid), OperationLog);
            return false;
        }
        m_ThickTypes[devid]->SetTriggerMode(thickConfig.s_TriggerMode);
        m_ThickTypes[devid]->SetScanRate(thickParams[idx].m_PresentRate);
        m_ThickTypes[devid]->ClearDataStack();
        if(m_ThickTypes[devid]->StartAcquisition())
        {
            m_cCombine.SetCombineCamera(devid, true);
            m_TReadThickNessList[devid]->start();
            CLogFile::write(tr("Sensor%1 start successfully!").arg(devid), OperationLog);
            emit signals_thickStateChanged(devid);
            return true;
        }
        CLogFile::write(tr("Sensor%1 start failed!").arg(devid), OperationLog);
        s_MSGBoxInfo info;
        info.strMsgtitle = tr("Wanning");
        info.strMsgInfo = tr("Sensor%1 start failed!").arg(devid);
        emit signals_MessageBoxMainThread(info);
        return false;
    }
    m_ThickTypes[devid]->StopAcquisition();
    m_cCombine.SetCombineCamera(devid, false);
    CLogFile::write(tr("Sensor%1 stop.").arg(devid), OperationLog);
    emit signals_thickStateChanged(devid);
    return true;
}

bool SystemUIMain::CheckAndStartThick()
{
    foreach(auto devid, thk_devid_index.keys())
    {
        auto idx = thk_devid_index[devid];
        if(!thickParams[idx].Enable)
        {
            continue;
        }
        if(m_ThickTypes[devid]->GetSensorStatus() == ETST_NotConn && !ThickStartCheck(devid))
        {
            return false;
        }
        if(m_ThickTypes[devid]->GetSensorStatus() == ETST_Conn && !ThickStartCheck(devid))
        {
            return false;
        }
    }
    return true;
}

void SystemUIMain::GetWordsTranslation(QMap<QString, QString>& mwords, QMap<QString, QString>& sensors)
{
    mwords = moldwors;
    sensors = sensorwords;
}

void SystemUIMain::slots_DataCenterConfUpdated()
{
    dcConf.eTimeInterval = (m_sSystemInfo.iSaveRecordInterval == 30)?ETI_30_Min:ETI_60_Min;
    QStringList shifts;
    if(m_sSystemInfo.shift1.isValid() )
    {
        shifts.append(m_sSystemInfo.shift1.toString("hh:mm:ss"));
        if(m_sSystemInfo.shift2.isValid() )
        {
            shifts.append(m_sSystemInfo.shift2.toString("hh:mm:ss"));
            if(m_sSystemInfo.shift3.isValid() )
            {
                shifts.append(m_sSystemInfo.shift3.toString("hh:mm:ss"));
            }
        }
        if(!shifts.isEmpty())
        {
            dcConf.shifts.reset(shifts);
        }
    }
#ifndef NO_MOULD_COUNT
    dbCenter->OnDataConfChange(dcConf);
#endif
}

//功能：动态切换系统语言
bool SystemUIMain::changeLanguage(int nLangIdx)
{
    QSettings sysSet("daheng","GlassDetectSystem");
    static QTranslator *translator = NULL, *qtDlgCN = NULL;
    bool bRtn = true;
    if (nLangIdx == 0)//中文
    {
        translator = new QTranslator;
        qtDlgCN = new QTranslator;
        if (translator->load("SystemUIMain_zh.qm"))
        {
            qApp->installTranslator(translator);
            //中文成功后，加载Qt对话框标准翻译文件，20141202
            if (qtDlgCN->load("SystemUIMain_zh.qm"))
            {
                qApp->installTranslator(qtDlgCN);
            }
            //保存设置
            sysSet.setValue("nLangIdx",nLangIdx);
//            nCurLang = nLangIdx;
        }
        else
        {
            QMessageBox::information(this,tr("Information"),tr("Load Language pack [SystemUIMain_zh.qm] fail!"));
            //保存设置
            sysSet.setValue("nLangIdx",1);
            bRtn = false;
        }
    }
     if (nLangIdx == 1)//英文
     {
 //        nCurLang = nLangIdx;
         if (translator != NULL)
         {
             qApp->removeTranslator(translator);
             delete translator;
             translator = NULL;
 //            translateUi();
             //保存设置
             sysSet.setValue("nLangIdx",nLangIdx);
         }            
     }
    return bRtn;
}
void SystemUIMain::ShowCheckSet(int nCamIdx,int signalNumber)
{
    if (nCamIdx >= m_sSystemInfo.iRealCamCount)
    {
        return;
    }
    try
    {
        s_AlgModelPara  sAlgModelPara;    
        QImage tempIamge;
        //获取一张此相机的图片和定位信息
        if(!slots_getImageLocaInfo(nCamIdx, tempIamge, sAlgModelPara))
        {
            CLogFile::write(QString("Change to Alg Page, get image failed."), AbnormityLog);
            return;
        }
        
        s_Status  sReturnStatus = m_cBottleModel.CloseModelDlg();
        if (sReturnStatus.nErrorID != RETURN_OK)
        {
            CLogFile::write(tr("Error in close Model"),AbnormityLog);
        }

        sAlgModelPara.sImgPara.nHeight = tempIamge.height();
        sAlgModelPara.sImgPara.nWidth = tempIamge.width();
        sAlgModelPara.sImgPara.nChannel = (tempIamge.byteCount()+7)/8;
        sAlgModelPara.sImgPara.pcData = (char*)tempIamge.bits();

        if (sAlgModelPara.sImgPara.nHeight != m_sCarvedCamInfo[nCamIdx].m_iImageHeight)
        {
            CLogFile::write(tr("Image height:%1 not fit camera height:%2").arg(sAlgModelPara.sImgPara.nHeight).arg(m_sCarvedCamInfo[nCamIdx].m_iImageHeight),AbnormityLog);
            return;
        }
        if (sAlgModelPara.sImgPara.nWidth != m_sCarvedCamInfo[nCamIdx].m_iImageWidth)
        {
            CLogFile::write(tr("Image Width:%1 not fit camera Width:%2").arg(sAlgModelPara.sImgPara.nWidth).arg(m_sCarvedCamInfo[nCamIdx].m_iImageWidth),AbnormityLog);
            return;
        }        
        for (int i=0;i<m_sSystemInfo.iRealCamCount;i++)
        {
            CherkerAry.pCheckerlist[i].nID = i;
            CherkerAry.pCheckerlist[i].pChecker = &m_cBottleCheck[i];
        }    
        int widthd = widget_alg->geometry().width();
        int heightd    = widget_alg->geometry().height();
        if (widthd < 150 || heightd < 150)
        {
            widget_alg->resize(statked_widget->size());
        }    
        sReturnStatus = m_cBottleModel.SetModelDlg(sAlgModelPara,&m_cBottleCheck[nCamIdx],CherkerAry,widget_alg);
        if (sReturnStatus.nErrorID != RETURN_OK)
        {
            CLogFile::write(QString("reset alg page failed."),AbnormityLog);
            return;
        }
    }
    catch (...)
    {
        CLogFile::write(tr("----Abnormal in set Mode2 ----"),AbnormityLog);
    }

    CLogFile::write(tr("In to Alg Page")+tr("CamraNo:%1").arg(nCamIdx),OperationLog,0);
    return;    
}
void SystemUIMain::slots_OnExit()
{
    if (QMessageBox::Yes == QMessageBox::question(this,tr("Exit"),
        tr("Are you sure to exit?"),
        QMessageBox::Yes | QMessageBox::No))    
    {
        if (m_sSystemInfo.m_bIsTest ||m_sRunningInfo.m_bCheck)
        {
            QMessageBox::information(this,tr("Information"),tr("Please Stop Test First!"));
            return;
        }

        CLogFile::write(tr("Close ModelDlg!"),OperationLog);
        s_Status  sReturnStatus = m_cBottleModel.CloseModelDlg();
        if (sReturnStatus.nErrorID != RETURN_OK)
        {
            CLogFile::write(tr("Error in Close ModelDlg--OnExit"),AbnormityLog);
            return;
        }
        ToolButton *TBtn = title_widget->button_list.at(PGStartOrStopButton);
        m_bIsThreadDead = TRUE;
        emit signals_exitSystem();
        CLogFile::write(tr("Exit system!"),OperationLog);

        if (m_sSystemInfo.isThickNessEnable)
        {
            //使能壁厚

        }
         
        QSettings iniset(m_sConfigInfo.m_strDataPath,QSettings::IniFormat);
        iniset.setIniCodec(QTextCodec::codecForName("GBK"));
        iniset.setValue("/system/failureNum",m_sRunningInfo.m_failureNumFromIOcard);
        iniset.setValue("/system/checkedNum",m_sRunningInfo.m_checkedNum);
        iniset.setValue("/system/MouldCount",m_sRunningInfo.nModelReadFailed);
        widget_count->slots_SaveCountInfo();
        closeWidget();
    }
}
void SystemUIMain::writeLogText(QString string,e_SaveLogType eSaveLogType)
{
    emit signals_writeLogText(string, eSaveLogType);
}
int SystemUIMain::ReadImageSignal(int TrigPort, int trigCardID)
{
    //CLogFile::write(tr("get Card:%1 Port:%2 image NO.").arg(trigCardID).arg(TrigPort), DebugLog, 0);
    switch(TrigPort-1) 
    {
    case 0:
        return m_vIOCard[trigCardID]->ReadImageSignal(28);
        break;
    case 1:
        return m_vIOCard[trigCardID]->ReadImageSignal(29);
        break;
    case 2:
        return m_vIOCard[trigCardID]->ReadImageSignal(30);
        break;
    case 3:
        return m_vIOCard[trigCardID]->ReadImageSignal(31);
        break;
    case 4:
        return m_vIOCard[trigCardID]->ReadImageSignal(33);
        break;
    case 5:
        return m_vIOCard[trigCardID]->ReadImageSignal(32);
        break;
    case 6:
        return m_vIOCard[trigCardID]->ReadImageSignal(34);
        break;
    default:
        CLogFile::write(tr("NO Position%1").arg(TrigPort),AbnormityLog);
        return 0;
    }
    return 1;
}

int SystemUIMain::ReadDeviceImageSignal(int devID)
{
    if(m_sDeviceType.count(devID) == 0)
    {
        CLogFile::write(tr("get DevID:%1 image NO, devID not exist.").arg(devID), DebugLog, 0);
        return 0;
    }
    int Position = -1;
    int cardID = 0;
    switch((EDeviceType)m_sDeviceType[devID])
    {
    case EDTCameraFinishBottom:
    case EDTCameraOvality:
    case EDTCameraLineMatrix:
        Position = m_sRealCamInfo[devID].m_iGrabPosition;
        cardID = m_sRealCamInfo[devID].m_iIOCardSN;
        break;
    }
    return ReadImageSignal(Position, cardID);
}

bool SystemUIMain::CheckCombineResult(int ImageNumber)
{
    //综合剔废结果，统计缺陷
    int comResult = -1;
    if (m_cCombine.ConbineResult(ImageNumber, 0, comResult))//图像都拍完后结果综合
    {
        Record _tmp;
        //补剔当前图像号前面5个
        for (int i = ImageNumber-5; i<ImageNumber ;i++)
        {
            int imgNO = (i+256)%256;
            if (!m_cCombine.IsReject(imgNO))
            {
                s_ResultInfo sResultInfo;
                sResultInfo.tmpResult = m_cCombine.m_Rlts[imgNO].iResult;
                sResultInfo.nImgNo = imgNO;
                sResultInfo.nIOCardNum = 0;

                if (m_sSystemInfo.m_bIsIOCardOK)
                {
                    m_vIOCard[sResultInfo.nIOCardNum]->SendResult(sResultInfo);
                }
                m_cCombine.SetReject(imgNO);
                auto tinfo = m_cCombine.tempMoldinfo[imgNO];
                m_cCombine.tempMoldinfo[imgNO].clear();

                mxImgMould.lock();
                if(imgSNModeIDs.contains(imgNO))
                {
                    m_sRunningInfo.nModelChenkedNumber[imgSNModeIDs[ImageNumber]]++;
                    tinfo.id = QString::number(imgSNModeIDs[imgNO]);
                }
                else
                {
                    if(buttonVisible[PGLCamReadMoldPage])
                    {
                        m_sRunningInfo.nModelReadFailed++;
                    }
                    tinfo.id = "0";
                }
                imgSNModeIDs.remove(imgNO);
                mxImgMould.unlock();
#ifndef NO_MOULD_COUNT
                tinfo.inspected = 1;
                Record t;
                t.inspected = 1;
                if(sResultInfo.tmpResult)
                {
                    tinfo.rejects = 1;
                    tinfo.defects = 1;
                    tinfo.addASensor("200");//补踢
                    t.rejects = 1;
                    t.defects = 1;
                }
                t.moldinfo.push_back(tinfo);
                _tmp.MergeData(t);
#endif
                signals_bottleReady(imgNO);
            }
        }

        //设置认为后面5个图像号还没发
        for (int i = ImageNumber+1; i<=ImageNumber+5;i++)
        {
            int imgNO = (i+256)%256;
            m_cCombine.SetReject(imgNO, false);
        }
        //当前结果
        s_ResultInfo sResultInfo;
        sResultInfo.tmpResult = comResult;
        sResultInfo.nImgNo = ImageNumber;
        sResultInfo.nIOCardNum = 0;
        if (m_sSystemInfo.m_bIsIOCardOK)
        {
            m_vIOCard[sResultInfo.nIOCardNum]->SendResult(sResultInfo);

            quint32 result=0x00;
            for (int i=0;i< m_sSystemInfo.iDevTotalCount;i++)
            {
                if (m_cCombine.m_Rlts[ImageNumber].b_Rlts[i] && m_cCombine.b_CombinCamera[i] )
                {
                    result |= m_cCombine.m_Rlts[ImageNumber].b_Rlts[i]<<i;
                }
            }
            m_udpsocket->setUdpPacketRlt(ImageNumber, result);
        }
        m_cCombine.SetReject(ImageNumber);
        m_cCombine.RemoveOneResult(ImageNumber);
        if (m_sRunningInfo.m_bCheck)    
        {
            if(!m_sSystemInfo.m_bIsIOCardOK && !buttonVisible[PGAlgPage])
            {//模拟下 无相机情况下,只在综合里增加计数
                m_sRunningInfo.m_checkedNum++;
                if(comResult)
                {//模拟下的剔废计数
                    m_sRunningInfo.m_failureNumFromIOcard ++;
                }
            }

            int iErrorCamera = m_cCombine.ErrorCamera(ImageNumber);
            s_ErrorPara sComErrorpara = m_cCombine.ConbineError(ImageNumber);
            if (m_sRunningInfo.m_cErrorTypeInfo[iErrorCamera].ErrorTypeJudge(sComErrorpara.nErrorType))
            {
                m_sRunningInfo.m_cErrorTypeInfo[iErrorCamera].iErrorCountByType[sComErrorpara.nErrorType]+=1;
                m_sRunningInfo.m_iErrorCamCount[iErrorCamera] += 1;
                m_sRunningInfo.m_iErrorTypeCount[sComErrorpara.nErrorType] +=1;
            }
            else
            {
                m_sRunningInfo.m_cErrorTypeInfo[iErrorCamera].iErrorCountByType[0]+=1;
                m_sRunningInfo.m_iErrorTypeCount[0] +=1;
            }    
            auto tinfo = m_cCombine.tempMoldinfo[ImageNumber];
            m_cCombine.tempMoldinfo[ImageNumber].clear();
            mxImgMould.lock();
            if(imgSNModeIDs.contains(ImageNumber))
            {
                int moldnum = imgSNModeIDs[ImageNumber];
                tinfo.id = QString::number(moldnum);
                if(comResult)
                {
                    m_sRunningInfo.nModelChenkedNumber[imgSNModeIDs[ImageNumber]]++;
                    emit signals_CombineMoldNumKickOut(moldnum);
                }
            }
            else
            {
                if(buttonVisible[PGLCamReadMoldPage])
                {
                    m_sRunningInfo.nModelReadFailed++;
                }
                tinfo.id = "0";
            }
            imgSNModeIDs.remove(ImageNumber);
            mxImgMould.unlock();
#ifndef NO_MOULD_COUNT
            Record t;
            t.inspected = 1;
            tinfo.inspected = 1;
            if(comResult)
            {
                t.rejects = 1;
                t.defects = 1;
                tinfo.rejects = 1;
                tinfo.defects = 1;
            }
            t.moldinfo.push_back(tinfo);
            _tmp.MergeData(t);
            QMutexLocker lk(&recordLock);
            record.MergeData(_tmp);
#endif
            emit signals_bottleReady(ImageNumber);
        }
        return true;
    }
    return false;
}

void SystemUIMain::InitCamImage(int iCameraNo)
{
    //     CLogFile::write(tr("获取最大图像信息！"),OperationLog);
    //获取最大图像信息
    for (int i=0;i<m_sSystemInfo.iRealCamCount;i++)
    {
        if (i==0)
        {
            m_sSystemInfo.m_iMaxCameraImageWidth     = m_sRealCamInfo[i].m_iImageWidth;
            m_sSystemInfo.m_iMaxCameraImageHeight    = m_sRealCamInfo[i].m_iImageHeight;
            m_sSystemInfo.m_iMaxCameraImageSize      = m_sRealCamInfo[i].m_iImageSize;
            m_sSystemInfo.m_iMaxCameraImagePixelSize = (m_sRealCamInfo[i].m_iImageBitCount+7)/8;
        }
        else
        {
            if (m_sRealCamInfo[i].m_iImageWidth > m_sSystemInfo.m_iMaxCameraImageWidth)
            {
                m_sSystemInfo.m_iMaxCameraImageWidth = m_sRealCamInfo[i].m_iImageWidth;
            }                
            if (m_sRealCamInfo[i].m_iImageHeight > m_sSystemInfo.m_iMaxCameraImageHeight)
            {
                m_sSystemInfo.m_iMaxCameraImageHeight = m_sRealCamInfo[i].m_iImageHeight;
            }                
            if (((m_sRealCamInfo[i].m_iImageBitCount+7)/8) > m_sSystemInfo.m_iMaxCameraImagePixelSize)
            {
                m_sSystemInfo.m_iMaxCameraImagePixelSize = ((m_sRealCamInfo[i].m_iImageBitCount+7)/8);
            }            
        }
        m_sSystemInfo.m_iMaxCameraImageSize = m_sSystemInfo.m_iMaxCameraImageWidth*m_sSystemInfo.m_iMaxCameraImageHeight;
    }

    QSettings iniset(m_sConfigInfo.m_strConfigPath,QSettings::IniFormat);
    
    ReadCorveConfig();

    //int iCarvedCamNum = 0;
    QString strSession;
    for(int i = 0 ; i < m_sSystemInfo.iRealCamCount; i++)
    {
        m_sCarvedCamInfo[i].m_iImageBitCount = m_sRealCamInfo[i].m_iImageBitCount;   //图像位数从相机处继承[8/7/2013 nanjc]
        m_sCarvedCamInfo[i].m_iImageRoAngle = m_sRealCamInfo[i].m_iImageRoAngle;

        //实时显示用, 预分配QImage空间，每切出相机一个
        if (m_sCarvedCamInfo[i].m_pActiveImage!=NULL)
        {
            delete m_sCarvedCamInfo[i].m_pActiveImage;
            m_sCarvedCamInfo[i].m_pActiveImage = NULL;
        }
        if (90 == m_sCarvedCamInfo[i].m_iImageRoAngle || 270 ==m_sCarvedCamInfo[i].m_iImageRoAngle)
        {
            m_sCarvedCamInfo[i].m_pActiveImage=new QImage(m_sCarvedCamInfo[i].m_iImageHeight,m_sCarvedCamInfo[i].m_iImageWidth,m_sCarvedCamInfo[i].m_iImageBitCount);// 用于实时显示
        }
        else
        {
            m_sCarvedCamInfo[i].m_pActiveImage=new QImage(m_sCarvedCamInfo[i].m_iImageWidth,m_sCarvedCamInfo[i].m_iImageHeight,m_sCarvedCamInfo[i].m_iImageBitCount);// 用于实时显示
        }
        m_sCarvedCamInfo[i].m_pActiveImage->setColorTable(m_vcolorTable);
        //             //开始采集前补一张黑图
        BYTE* pByte = m_sCarvedCamInfo[i].m_pActiveImage->bits();
        int iLength = m_sCarvedCamInfo[i].m_pActiveImage->byteCount();
        memset((pByte),0,(iLength));
        //分配图像剪切内存区域,大小等于真实相机大小
        if (m_sCarvedCamInfo[i].m_pGrabTemp!=NULL)
        {
            delete m_sCarvedCamInfo[i].m_pGrabTemp; 
            m_sCarvedCamInfo[i].m_pGrabTemp = NULL;
        }
        m_sCarvedCamInfo[i].m_pGrabTemp = new BYTE[m_sCarvedCamInfo[i].m_iImageWidth*m_sCarvedCamInfo[i].m_iImageHeight];
        //分配元素链表中图像的内存，每剪切出来的相机10个。
        //             CLogFile::write(tr("分配元素链表中图像的内存！%1").arg(iCarvedCamNum),OperationLog);
        m_queue[i].mLocker.lock();
        m_queue[i].InitQueue(m_sCarvedCamInfo[i].m_iImageWidth, m_sCarvedCamInfo[i].m_iImageHeight,m_sCarvedCamInfo[i].m_iImageBitCount,m_sRealCamInfo[i].m_iImageTargetNo+5, true);
        m_queue[i].mLocker.unlock();
    }
    for (int i = 0; i < m_sSystemInfo.iDevTotalCount;i++)
    {
        // 错误统计用类
        m_sRunningInfo.m_cErrorTypeInfo[i].m_iErrorTypeCount = m_sErrorInfo.m_iErrorTypeCount;
    }
}
bool SystemUIMain::RoAngle (uchar* pRes,uchar* pTar,int iResWidth,int iResHeight,int iAngle)
{
    int iTarWidth;
    int iTarHeight;
    if(pRes == NULL || iResWidth == 0 || iResHeight == 0)
    {
        return FALSE;
    }
    //    BYTE* pImgBuff = new BYTE[iResWidth*iResHeight*8];
    if (iAngle == 90)
    {
        iTarWidth = iResHeight;
        iTarHeight = iResWidth;
        for (int i=0;i<iResHeight;i++)
        {
            for (int j=0;j<iResWidth;j++) 
            {
                *(pTar+j*iTarWidth+(iTarWidth-i-1)) = *(pRes+i*iResWidth+j);
            }
        }
    }
    else if (iAngle == 270)
    {    
        iTarWidth = iResHeight;
        iTarHeight = iResWidth;
        for (int i=0;i<iResHeight;i++)
        {
            for (int j=0;j<iResWidth;j++) 
            {
                *(pTar+(iTarHeight-j-1)*iTarWidth+i) = *(pRes+i*iResWidth+j);
            }
        }
    }
    else if (iAngle == 180)
    {
        iTarWidth = iResWidth;
        iTarHeight = iResHeight;
        for (int i=0;i<iResHeight;i++)
        {
            for (int j=0;j<iResWidth;j++) 
            {
                *(pTar+(iTarHeight-i-1)*iTarWidth+(iTarWidth-j-1))=*(pRes+i*iResWidth+j);
            }
        }
    }
    return TRUE;
}
void SystemUIMain::mouseMoveEvent(QMouseEvent *event)
{
    time(&info_widget->tLastOperaTime);
}
//鼠标点击事件
void SystemUIMain::mousePressEvent(QMouseEvent *event)
{  
    time(&info_widget->tLastOperaTime);
}

void SystemUIMain::InitLastData()
{
    QSettings LoadLastData(SaveDataPath,QSettings::IniFormat);
    LoadLastData.setIniCodec(QTextCodec::codecForName("GBK"));
    QString strSession;
    for (int j = 1;j<=m_sErrorInfo.m_iErrorTypeCount;j++)
    {
        for (int i = 0;i<m_sSystemInfo.iDevTotalCount;i++)
        {
            strSession = QString("DefaultTypeCount/EveryRow%1").arg(i);
            int xRowTemp=LoadLastData.value(strSession,0).toInt();

            strSession = QString("DefaultTypeCount/EveryLine%1").arg(j);
            int yLineTemp=LoadLastData.value(strSession,0).toInt();

            strSession = QString("DefaultTypeCount/EveryNumber%1_%2").arg(xRowTemp).arg(yLineTemp);
            m_sRunningInfo.m_cErrorTypeInfo[xRowTemp].iErrorCountByType[yLineTemp] = LoadLastData.value(strSession,0).toInt();
            m_sRunningInfo.m_iErrorTypeCount[j] += m_sRunningInfo.m_cErrorTypeInfo[xRowTemp].iErrorCountByType[yLineTemp];
            m_sRunningInfo.m_iErrorCamCount[i] += m_sRunningInfo.m_cErrorTypeInfo[xRowTemp].iErrorCountByType[yLineTemp];
        }
    }
    strSession=QString("HeadCount/AllCheckNumber");
    m_sRunningInfo.m_checkedNum=LoadLastData.value(strSession,0).toInt();

    strSession=QString("HeadCount/AllFailNumber");
    m_sRunningInfo.m_failureNumFromIOcard=LoadLastData.value(strSession,0).toInt();
    strSession=QString("HeadCount/MouldCount");
    m_sRunningInfo.nModelReadFailed=LoadLastData.value(strSession,0).toInt();

    strSession=QString("HeadCount/minRate");
    MinRate=LoadLastData.value(strSession,0).toDouble();

    strSession=QString("HeadCount/MaxRate");
    MaxRate=LoadLastData.value(strSession,0).toDouble();
}

//初始化界面
void SystemUIMain::initInterface()
{
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/sys/icon"), QSize(), QIcon::Normal, QIcon::Off);
    setWindowIcon(icon);

    QVBoxLayout* vLay = new QVBoxLayout();
    //导航栏
    auto naviWidget = new NavigationWidget;
    vLay->addWidget(naviWidget);
    //操作页
    statked_widget = new QStackedWidget();
    statked_widget->setObjectName("mainStacked");
    auto uiOperator = new UIOperation;
    auto uiSettings = new QWidget;
    auto uiHistory = new QWidget;
    auto uiInOut = new QWidget;
    auto uiLight = new QWidget;
    auto uiAlarm = new QWidget;

    uiLight->setStyleSheet("border-image:url(:/sys/Light.png);");

    statked_widget->addWidget(uiOperator);
    statked_widget->addWidget(uiSettings);
    statked_widget->addWidget(uiHistory);
    statked_widget->addWidget(uiInOut);
    statked_widget->addWidget(uiLight);
    statked_widget->addWidget(uiAlarm);

    vLay->addWidget(statked_widget);
    //状态栏
    auto stateBar = new QWidget(this);
    stateBar->setFixedHeight(40);
    //状态栏添加设备状态图标
    QGridLayout* gridLayoutStatusLight = new QGridLayout;

    lStatus = new QLabel(stateBar);
    lVersion = new QLabel(stateBar);
    QString strVer;
    //根据程序编译器,显示第三位是32还是64
    QString sVer = GetLocalPogramVersion();
    if(!sVer.isEmpty())
    {
        QStringList tstrl = sVer.split(".",QString::SkipEmptyParts);
        QString verRes = "  V:";
        for(int i = 0; i < tstrl.size();i++)
        {
            if(i == 1)
            {
#ifdef WIN_32_1_10
                verRes += "32";
#else
                verRes += "64";
#endif  
            }
            else
            {
                verRes += tstrl[i];
            }
            verRes += ".";
        }  
        strVer = verRes.left(verRes.size() - 1);
        lVersion->setText(strVer);
    }

    QFont fontCoder;
    fontCoder.setPixelSize(28);
    fontCoder.setPixelSize(18);
    lStatus->setFont(fontCoder);
    lVersion->setFont(fontCoder);

    QHBoxLayout* hLayoutStateBar = new QHBoxLayout(stateBar);
    hLayoutStateBar->addLayout(gridLayoutStatusLight);
    hLayoutStateBar->addStretch();
    hLayoutStateBar->addWidget(lStatus);
    hLayoutStateBar->addWidget(lVersion);
    hLayoutStateBar->setSpacing(3);
    hLayoutStateBar->setContentsMargins(10, 0, 10, 0);
    
    vLay->addWidget(stateBar);
    setLayout(vLay);
    setWindowTitle(tr("ILDR System %1").arg(strVer));
    //连接信号槽
    connect(this, SIGNAL(signals_setNaviLockSt(bool)), naviWidget, SLOT(slots_setLockState(bool)));
    connect(this, SIGNAL(signals_turnPage(int)), naviWidget, SLOT(slots_turnPage(int)));
    connect(this, SIGNAL(signals_setCurrentPage(int)), naviWidget, SLOT(slots_selectCurrent(int)));
    connect(naviWidget, SIGNAL(signals_navigateTrig(int)), this, SLOT(slots_NaviOperation(int)));

    resize(1024,768);
}

void SystemUIMain::slots_NaviOperation(int op)
{
    if (iLastPage == op)
    {
        return;
    }
    switch (op)
    {
    case ENaviPageOperation:
    case ENaviPageSettings:
    case ENaviPageHistory:
    case ENaviPageInOut:
    case ENaviPageLight:
    case ENaviPageAlarm:
        slots_switchPage(op);
        break;
    case ENaviToolResetCount:
        break;
    case ENaviToolLock:
        {
            if(!isLock())
            {
                bLock = true;
            }
            else
            {
                if(!checkLock())
                {
                    return;
                }
                bLock = false;
            }
            emit signals_setNaviLockSt(bLock);
        }
        break;
    default:
        break;
    }
}

void SystemUIMain::slots_switchPage(int current_page)
{
    //离开上个页面
    //进入新页面
    statked_widget->setCurrentIndex(current_page);
    emit signals_setCurrentPage(current_page);
    iLastPage = current_page;
}

bool SystemUIMain::isLock() const
{
    return bLock;
}

bool SystemUIMain::checkLock()
{
    if(!isLock())
        return true;
    CPasswordDlg PassDlg;
    PassDlg.exec();
    return PassDlg.isOK();
}
