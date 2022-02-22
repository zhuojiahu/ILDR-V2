#include <QtGui/QApplication>
#include <QTextCodec>
#include <QSharedMemory>
#include <QSplashScreen>
#include <QTranslator>
#include <QSettings>
#include <QMessageBox>
#include <QFile>

#include "qt_windows.h"
//QT检测内存泄漏头文件
#include "setDebugNew.h"
#include "reportingHook.h"

#define _CRTDBG_MAP_ALLOC 
#include <crtdbg.h>
#include "winuser.h"
#include "tlhelp32.h"
#include "tchar.h"
#include <DbgHelp.h>
#pragma comment(lib,"Dbghelp.lib")

#include "SystemUIMain.h"

static long  __stdcall CrashInfocallback(_EXCEPTION_POINTERS *pexcp);

int main(int argc, char *argv[])
{
    int nResult = -1;
    {
        ::SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)CrashInfocallback);
        _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
        QApplication a(argc, argv);
        //防止图标不显示
        a.addLibraryPath("./QtPlugins/");

        QSharedMemory shared_memory("VEXI/ILDR");            
        if(shared_memory.attach())   //尝试将进程附加到该共享内存段
        {
            QMessageBox::information(nullptr, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("程序已经运行,请不要多开实例!"),QString::fromLocal8Bit("确认"));
            //        QMessageBox::information(nullptr, QObject::tr("Error"), QObject::tr("this Counter has opened, Please don't run multi-instance!"), QObject::tr("OK"));
            return -1;   
        }
        shared_memory.create(1);

        //加载QSS样式表
        QFile qss(":/qss/ILDR.qss");
        qss.open(QFile::ReadOnly);
        qApp->setStyleSheet(qss.readAll());
        qss.close();
    
        QString strLoading = ":/loading/loading"; 
        QSplashScreen spLoading(QPixmap(strLoading.toLocal8Bit().constData()));
        spLoading.show();
        SystemUIMain w;
        w.showMaximized();
        w.activateWindow();
        spLoading.finish(&w);
        nResult = a.exec();
    }
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
