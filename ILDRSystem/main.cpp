#include <QtGui/QApplication>
#include <QTextCodec>
#include <QSharedMemory>
#include <QSplashScreen>
#include <QTranslator>
#include <QSettings>
#include <QMessageBox>

//QTϵͳ����
#define DAHENGBLPKP_QT			//QTϵͳʱ��������ͣ�����ע�͵�
#include "ILDRSystem.h"
#include "clogfile.h"
//QT����ڴ�й©ͷ�ļ�
#include "setDebugNew.h"
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC 

#include "winuser.h"
#include "tlhelp32.h"
#include "tchar.h"
#include <DbgHelp.h>
#pragma comment(lib,"Dbghelp.lib")
static long  __stdcall CrashInfocallback(_EXCEPTION_POINTERS *pexcp);

//�ر�ͬ��������
void KillSameDeathProcess(QString exestr)
{
	HANDLE handle;
	HANDLE handle1;
	handle=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	PROCESSENTRY32 *info; 

	info=new PROCESSENTRY32;  
	info->dwSize=sizeof(PROCESSENTRY32); 

	Process32First(handle,info); 

	do
	{
		QString str = QString::fromWCharArray(info->szExeFile);
		if(exestr==str) 		
		{ 
			handle1=OpenProcess(PROCESS_TERMINATE,FALSE,info->th32ProcessID);
			TerminateProcess(handle1,0); 			
		}
	}while (Process32Next(handle,info)!=FALSE); 
	CloseHandle(handle); 
}

//������һ��
bool OnlyRunOnce(QString qstr,QString qstrext)
{
	HANDLE hMapping = ::CreateFileMapping((HANDLE)INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 16, qstr.utf16());
	if (hMapping == NULL)
	{
		return false;
	}
	else
	{
		if(GetLastError()==ERROR_ALREADY_EXISTS)
		{
			HWND hRecv = NULL;
			hRecv = ::FindWindow(NULL,qstr.utf16());
			//hRecv = ::FindWindow(NULL,QString("GlassDetectSystem").utf16());
			if (hRecv != NULL)
			{
				//Qt �޷���ȡ�ñ�ǣ�ԭ��δ֪����ֹ��С��
				//if (::IsIconic(hRecv))
				//	::ShowWindow(hRecv,SW_SHOWNORMAL);
				//::ShowWindow(hRecv,SW_MAXIMIZE);
				//::SetForegroundWindow(hRecv);
				::BringWindowToTop(hRecv);
			}
			else
			{
				//KillSameDeathProcess(qstrext);
            }
            return false;
		}
	}
	return true;
}

int main(int argc, char *argv[])
{
	::SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)CrashInfocallback);
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    int nResult = -1;
    {
	    QApplication a(argc, argv);
	    //���ù̶���񣬲�����ϵͳ�仯���仯
	    //QApplication::setStyle("plastique");
	    //��ֹͼ�겻��ʾ
	    QApplication::addLibraryPath("./QtPlugins");
	    //��ֹ��������
    // 	a.addLibraryPath("plugins/codecs/");
	    QTextCodec *codec = QTextCodec::codecForName("GBK"); 
	    QTextCodec::setCodecForLocale(codec); 
	    QTextCodec::setCodecForTr(codec);
	    QTextCodec::setCodecForCStrings(codec);	

        QTranslator translator;
        QSettings setTranslation(".\\Config\\Config.ini",QSettings::IniFormat);
        setTranslation.setIniCodec(QTextCodec::codecForName("GBK"));

        int iLanguage = setTranslation.value("/system/Language",0).toInt();//	m_sErrorInfo.m_iErrorTypeCount = erroriniset.value(strSession,0).toInt();
        if (0 == iLanguage)
        {
            translator.load(".\\ILDRSystem_zh.qm");
            a.installTranslator(&translator);
        }

	    //ִ���ļ�����+��չ��
	    QString exepath = argv[0];
	    QString exenameext = exepath.right(exepath.length()-exepath.findRev("\\")-1);
	    QString apppath = exepath.left(exepath.findRev("\\")+1);
	    QString exename = exenameext.left(exenameext.findRev("."));
        QString t = exepath.replace('\\', '/');
	    //ֻ����һ��ʵ��
	    if (!OnlyRunOnce(t,exenameext))
	    {
		    QMessageBox::information(NULL,QObject::tr("System Prompted"),QObject::tr("Can not run multiple instance!"));//ϵͳ��ʾ��ϵͳ�Ѿ����У��޷��ظ��򿪣�	
		    return 0;
	    }
	    //����QSS��ʽ��
	    QFile qss(".\\ILDRSystem.qss");
	    qss.open(QFile::ReadOnly);
 	    qApp->setStyleSheet(qss.readAll());
	    qss.close();

        QString strLoading = ":/loading/loading_tiama";
        QSplashScreen spLoading(QPixmap(strLoading.toLocal8Bit().constData()));
        spLoading.show();

	    SysMainUI w;
	    w.SetLanguage(iLanguage);
	    //���ܹ�
	    w.Init();
	    bool bReturn = w.CheckLicense();
	    if(!bReturn)
		    return false;
	    w.showMaximized();
	    w.raise();//�ŵ�����
	    w.activateWindow();
	    spLoading.finish(&w);
	    nResult = a.exec();
    }
    CLogFile* plog = CLogFile::getInstance();
    delete plog;
	return nResult; 
}

long  __stdcall CrashInfocallback( _EXCEPTION_POINTERS *pexcp)
{
	HANDLE hDumpFile = ::CreateFile(
		L"MEMORY.DMP",
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if( hDumpFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
		dumpInfo.ExceptionPointers = pexcp;
		dumpInfo.ThreadId = ::GetCurrentThreadId();
		dumpInfo.ClientPointers = TRUE;
		::MiniDumpWriteDump(
			::GetCurrentProcess(),
			::GetCurrentProcessId(),
			hDumpFile,
			MiniDumpNormal,
			&dumpInfo,
			NULL,
			NULL
			);
	}
	return 0;
}