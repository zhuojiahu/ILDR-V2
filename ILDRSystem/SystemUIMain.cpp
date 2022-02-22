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

#include "commondef.h"
#include "uitypes.h"
#include "NavigationWidget.h"
#include "cpassworddlg.h"
#include "uiOperation.h"


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
    initInterface();
    Init();
}

SystemUIMain::~SystemUIMain()
{
    
}

void SystemUIMain::Init()
{
    //设置初始状态
    bLock = true;
    emit signals_setNaviLockSt(bLock);
    slots_NaviOperation(ENaviPageOperation);
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
