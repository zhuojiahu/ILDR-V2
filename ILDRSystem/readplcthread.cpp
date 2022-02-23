#include "readplcthread.h"

ReadPLCThread::ReadPLCThread(QObject *parent)
	: QThread(parent)
{
	//m_strPLCInfoPath = "Config\\PLC_Info.ini";
	//char charPLCInfoPath[maxpath];
	//memset(charPLCInfoPath,0,maxpath);
	//strcpy_s(charPLCInfoPath,m_strPLCInfoPath); 
	//PLCContorl = new CHMIDLL();
	//PLCContorl->InitPath(charPLCInfoPath);
	//PLCContorl->InitSerialComm();

}

ReadPLCThread::~ReadPLCThread()
{
   //	PLCContorl->CloseComm();
  	//delete PLCContorl;

}

 void ReadPLCThread::run()
 {
 // 	PLCContorl->GetPLCStatus(sPLCStatus);
 	sleep(500);
 }
// 
// s_PLCStatus ReadPLCThread::GetPLCStatus()
// {
// // 	return sPLCStatus;
// }
