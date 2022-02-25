#include "ILDRSystem.h"

#include <time.h>
#include "qt_windows.h"

#include <QLabel>
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
#include <QTextCodec>
#include <QDateTime>
#include <QPropertyAnimation>

#include "clogFile.h"
#include "DetectThread.h"
#include "CIOCard.h"

#include "widget_title.h"
#include "widget_image.h"
#include "Widget_CarveSetting.h"
#include "widget_Management.h"
#include "widget_test.h"
#include "widget_count.h"
#include "cpassworddlg.h"
#include "EquipRuntime.h"
#include "widgetwarning.h"
#include "cmyerrorlist.h"
#include "rled.h"

#include "database.h"
#include <Mmsystem.h>
#pragma comment( lib,"winmm.lib" )


//是否加密宏开关 JIAMI_INITIA
//#define  JIAMI_INITIA
#ifdef JIAMI_INITIA
#ifdef WIN_32_1_10
#include "ProgramLicense.h"
#include "pushLicense.h"
CProgramLicense m_ProgramLicense;
#pragma comment( lib,"DHProgramLicense.lib" )
#else
#include "ProgramLicense.h"
#include "pushLicense.h"
CProgramLicense m_ProgramLicense;
#pragma comment( lib,"DHProgramLicense64.lib" )
#endif
#endif


#pragma comment(lib, "version.lib")

SysMainUI * pMainFrm;
QString appPath;  //更新路径

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

//
//DWORD SysMainUI::SendIOCard(void* param)
//{
//	while(!pMainFrm->m_bIsThreadDead)
//	{
//		QByteArray nTest;
//		if(pMainFrm->ncSocketWriteData1.count()>0)
//		{
//			QByteArray nTmpdate = pMainFrm->ncSocketWriteData1.first();
//			pMainFrm->ncSocketWriteData1.removeFirst();
//			nTest.append(nTmpdate);
//		}
//		if(pMainFrm->ncSocketWriteData2.count()>0)
//		{
//			QByteArray nTmpdate = pMainFrm->ncSocketWriteData2.first();
//			pMainFrm->ncSocketWriteData2.removeFirst();
//			nTest.append(nTmpdate);
//		}
//		if (!nTest.isEmpty() && pMainFrm->m_tcpSocket != NULL )
//		{
//			int ret=pMainFrm->m_tcpSocket->write(nTest);
//			if(ret == -1)
//				CLogFile::write(QString("Send data Error"),CheckLog);
//		}
//		Sleep(1);
//	}
//	return TRUE;
//}
//
//DWORD SysMainUI::DataCountThread(void* param)
//{
//	MyErrorType nErrorFristData[DETA_LEN];
//	MyErrorType nErrorClampData[DETA_LEN];
//	MyErrorType nErrorBACKData[DETA_LEN];
//	QByteArray buffer[3];
//	char* ptr[3];
//	while(!pMainFrm->m_bIsThreadDead)
//	{
//		if(pMainFrm->nDataCount[0].count()>0 && pMainFrm->nDataCount[1].count()>0 && pMainFrm->nDataCount[2].count()>0)
//		{
//			for(int i=0;i<3;i++)
//			{
//				buffer[i] = pMainFrm->nDataCount[i].first();
//				pMainFrm->nDataCount[i].removeFirst();
//				ptr[i] = buffer[i].data();
//				ptr[i] += sizeof(MyStruct);
//			}
//			//处理数据
//			memset(nErrorFristData,0,sizeof(MyErrorType)*DETA_LEN);
//			memset(nErrorClampData,0,sizeof(MyErrorType)*DETA_LEN);
//			memset(nErrorBACKData,0,sizeof(MyErrorType)*DETA_LEN);
//
//			memcpy(nErrorFristData,ptr[0],sizeof(MyErrorType)*DETA_LEN);
//			memcpy(nErrorClampData,ptr[1],sizeof(MyErrorType)*DETA_LEN);
//			memcpy(nErrorBACKData ,ptr[2],sizeof(MyErrorType)*DETA_LEN);
//			
//			for(int i=0;i<DETA_LEN;i++) // 通过循环所有综合数据保存在 nErrorFristData中
//			{
//				if(		( nErrorFristData[i].nType >=ERRORTYPE_MAX_COUNT && nErrorFristData[i].nType < 0 )
//					||	( nErrorClampData[i].nType >=ERRORTYPE_MAX_COUNT && nErrorClampData[i].nType < 0 )
//					||	( nErrorBACKData[i].nType  >=ERRORTYPE_MAX_COUNT && nErrorBACKData[i].nType  < 0 ) )
//				{
//					CLogFile::write(QString("Handle Date Error Type"),CheckLog);
//					continue;
//				}
//				if(		( nErrorFristData[i].nMoldNo >=MOLDNO_MAX_COUNT &&	nErrorFristData[i].nMoldNo < 0 )
//					||	( nErrorClampData[i].nMoldNo >=MOLDNO_MAX_COUNT &&  nErrorClampData[i].nMoldNo < 0 )
//					||	( nErrorBACKData[i].nMoldNo >=MOLDNO_MAX_COUNT  &&	nErrorBACKData[i].nMoldNo  < 0 ) )
//				{	
//					CLogFile::write(QString("Handle Date Error nMoldNo"),CheckLog);
//					continue;
//				}
//				pMainFrm->nRunInfo.iAllCount += 1;
//				pMainFrm->m_sMoldNoCount.m_iMoldNoAllCount += 1;
//				if(nErrorFristData[i].nType != 0 || nErrorClampData[i].nType != 0 || nErrorBACKData[i].nType != 0)//综合有缺陷，计数加1
//				{
//					pMainFrm->nRunInfo.iFailCount += 1;
//					pMainFrm->m_sMoldNoCount.m_iMoldNoFailCount += 1;
//					pMainFrm->m_sMoldNoCount.m_iMoldNoCount[nErrorClampData[i].nMoldNo] += 1;
//					if(nErrorFristData[i].nErrorArea <= nErrorClampData[i].nErrorArea && nErrorClampData[i].nErrorArea >= nErrorBACKData[i].nErrorArea)
//					{
//						pMainFrm->nRunInfo.ClampCount += 1;
//						pMainFrm->nRunInfo.iClampErrorByType[nErrorClampData[i].nType] += 1;
//						pMainFrm->nRunInfo.iErrorByType[nErrorClampData[i].nType] += 1;
//
//						pMainFrm->m_sMoldNoCount.m_iMoldNoTypeCount[nErrorClampData[i].nType][nErrorClampData[i].nMoldNo] += 1;
//					}
//					else if(nErrorFristData[i].nErrorArea <= nErrorBACKData[i].nErrorArea && nErrorClampData[i].nErrorArea <= nErrorBACKData[i].nErrorArea)
//					{
//						pMainFrm->nRunInfo.RearCount += 1;
//						pMainFrm->nRunInfo.iRearErrorByType[nErrorBACKData[i].nType] += 1;
//						pMainFrm->nRunInfo.iErrorByType[nErrorBACKData[i].nType] += 1;
//
//						pMainFrm->m_sMoldNoCount.m_iMoldNoTypeCount[nErrorBACKData[i].nType][nErrorClampData[i].nMoldNo] += 1;
//					}
//					else if(nErrorFristData[i].nErrorArea >= nErrorClampData[i].nErrorArea && nErrorFristData[i].nErrorArea >= nErrorBACKData[i].nErrorArea)
//					{
//						pMainFrm->nRunInfo.FrontCount += 1;
//						pMainFrm->nRunInfo.iFrontErrorByType[nErrorFristData[i].nType] += 1;
//						pMainFrm->nRunInfo.iErrorByType[nErrorFristData[i].nType] += 1;
//
//						pMainFrm->m_sMoldNoCount.m_iMoldNoTypeCount[nErrorFristData[i].nType][nErrorClampData[i].nMoldNo] += 1;
//					}
//				}
//			}
//			emit pMainFrm->signals_updateCount(false);
//			//pMainFrm->UpdateCountForShow();
//		}
//		else{
//			for(int i=0;i<3;i++)
//			{
//				if(pMainFrm->nDataCount[i].count()>500)
//				{
//					pMainFrm->nDataCount[i].pop_front();
//				}
//			}
//			Sleep(1);
//		}
//
//	}
//	return TRUE;
//}
//
//DWORD SysMainUI::SendDetect1(void* param)
//{
//	int lastNumber1 = 0;
//	int nSignalNo1 = 0 ;
//	int nlastNumber1 = 0;
//	int nCurrentNum1 = 0;
//
//	char *m_reportPtr1 = new char[DETA_LEN * sizeof(MyErrorType) + sizeof(MyStruct)];
//	memset(m_reportPtr1,0,DETA_LEN * sizeof(MyErrorType) + sizeof(MyStruct));
//	
//	MyStruct nTempStruct;
//	memset(&nTempStruct,0,sizeof(MyStruct));
//
//	char* nTPtr1=NULL;
//
//	MyErrorType nCheckSendData1[DETA_LEN]={0};
//	int count1=0;
//
//	bool isFirst=true;
//
//	while(!pMainFrm->m_bIsThreadDead)
//	{
//		if(pMainFrm->m_sRunningInfo.m_bCheck && pMainFrm->m_sSystemInfo.m_bIsIOCardOK)
//		{
//			nCurrentNum1 = pMainFrm->m_vIOCard[0]->ReadCounter(3);
//			if(nCurrentNum1 != lastNumber1)
//			{
//				lastNumber1 = nCurrentNum1;
//				if(nCurrentNum1<=20 && isFirst)
//					continue;
//				isFirst=false;
//				nSignalNo1= pMainFrm->m_vIOCard[0]->ReadCounter(26); //根据预先保存的图像号来找对应的缺陷
//				CLogFile::write(QString("IOCard1 IN3:%1,count26:%2").arg(nCurrentNum1).arg(nSignalNo1),CheckLog);
//				if(nSignalNo1 != nlastNumber1)//排除加减速造成的图像号一致问题
//				{
//// 					int tmpImg=nSignalNo1;
//// 					if(nSignalNo1 < nlastNumber1)
//// 						tmpImg = nSignalNo1 +256;
//// 					for (int i=nlastNumber1+1; i<=tmpImg;i++)
//// 					{
//// 						int curImgNo = i % 256;
//						int curImgNo = nSignalNo1;
//						CLogFile::write(QString("Save Date1 ImgNo:%1,id:%2,type:%3,area:%4").arg(curImgNo).arg(pMainFrm->nSendData1[curImgNo].id).arg(pMainFrm->nSendData1[curImgNo].nType).arg(pMainFrm->nSendData1[curImgNo].nErrorArea),CheckLog);
//						nCheckSendData1[pMainFrm->nCountNumber1] = pMainFrm->nSendData1[curImgNo];
//						pMainFrm->nSendData1[curImgNo].id = 0;
//						pMainFrm->nSendData1[curImgNo].nType = 0;
//						pMainFrm->nSendData1[curImgNo].nErrorArea = 0;
//						pMainFrm->nSendData1[curImgNo].nMoldNo = 0;
//						pMainFrm->nCountNumber1++;
//						if(pMainFrm->nCountNumber1 == DETA_LEN)//发送DETA_LEN个数据到服务器进行汇总
//						{
//							pMainFrm->nCountNumber1 = 0;
//							if(pMainFrm->m_sSystemInfo.m_iDeviceOnInitial == 2)
//								nTempStruct.nUnit = CLAMPING;
//							else
//								nTempStruct.nUnit = LEADING;
//							nTempStruct.nState = SENDDATA;
//							nTempStruct.nCount = DETA_LEN;
//							memcpy(m_reportPtr1,&nTempStruct,sizeof(MyStruct));
//							nTPtr1 = m_reportPtr1;
//							nTPtr1 +=sizeof(MyStruct);
//							memcpy(nTPtr1,nCheckSendData1,DETA_LEN * sizeof(MyErrorType));
//							memset(nCheckSendData1,0,DETA_LEN * sizeof(MyErrorType));
//							QByteArray ba(m_reportPtr1, DETA_LEN * sizeof(MyErrorType)+sizeof(MyStruct));
//							pMainFrm->ncSocketWriteData1.push_back(ba);
//							memset(m_reportPtr1,0,DETA_LEN * sizeof(MyErrorType)+sizeof(MyStruct));
//							//CLogFile::write(QString("Save Date1,count:%1").arg(++count1),CheckLog);
//						}
////					}
//					nlastNumber1 = nSignalNo1;				
//				}
//			}
//		}
//		Sleep(1);
//	}
//	return TRUE;
//}
//
//DWORD SysMainUI::SendDetect2(void* param)
//{
//	int lastNumber2 = 0;
//	int nSignalNo2 = 0 ;
//	int nlastNumber2 = 0;
//	int nCurrentNum2 = 0;
//
//	char *m_reportPtr2 = new char[DETA_LEN * sizeof(MyErrorType) + sizeof(MyStruct)];
//	memset(m_reportPtr2,0,DETA_LEN * sizeof(MyErrorType) + sizeof(MyStruct));
//
//	MyStruct nTempStruct;
//	memset(&nTempStruct,0,sizeof(MyStruct));
//
//	char *nTPtr2=NULL;
//
//	MyErrorType nCheckSendData2[DETA_LEN]={0};
//	int count2=0;
//
//	bool isFirst=true;
//
//	while(!pMainFrm->m_bIsThreadDead)
//	{
//		if(pMainFrm->m_sRunningInfo.m_bCheck && pMainFrm->m_sSystemInfo.m_bIsIOCardOK)
//		{
//			if (pMainFrm->m_sSystemInfo.m_iDeviceOnInitial == 1 && pMainFrm->m_sSystemInfo.iIOCardCount == 2 )
//			{
//				nCurrentNum2 = pMainFrm->m_vIOCard[1]->ReadCounter(3);
//				if(nCurrentNum2 != lastNumber2)
//				{
//					lastNumber2 = nCurrentNum2;
//					if(nCurrentNum2<=20 && isFirst)
//						continue;
//					isFirst=false;
//					nSignalNo2= pMainFrm->m_vIOCard[1]->ReadCounter(26); //根据预先保存的图像号来找对应的缺陷
//					CLogFile::write(QString("IOCard2 IN3:%1,count26:%2").arg(nCurrentNum2).arg(nSignalNo2),CheckLog);
//					if(nSignalNo2 != nlastNumber2)//排除加减速造成的图像号一致问题
//					{
//// 						int tmpImg=nSignalNo2;
//// 						if(nSignalNo2 < nlastNumber2)
//// 							tmpImg = nSignalNo2 +256;
//// 						for (int i=nlastNumber2+1; i<=tmpImg;i++)
//// 						{
//// 							int curImgNo = i % 256;
//							int curImgNo = nSignalNo2;
//							CLogFile::write(QString("Save Date2 ImgNo:%1,id:%2,type:%3,area:%4").arg(curImgNo).arg(pMainFrm->nSendData2[curImgNo].id).arg(pMainFrm->nSendData2[curImgNo].nType).arg(pMainFrm->nSendData2[curImgNo].nErrorArea),CheckLog);
//							nCheckSendData2[pMainFrm->nCountNumber2] = pMainFrm->nSendData2[curImgNo];
//							pMainFrm->nSendData2[curImgNo].id = 0;
//							pMainFrm->nSendData2[curImgNo].nType = 0;
//							pMainFrm->nSendData2[curImgNo].nErrorArea = 0;
//							pMainFrm->nSendData2[curImgNo].nMoldNo = 0;
//							pMainFrm->nCountNumber2++;
//							if(pMainFrm->nCountNumber2 == DETA_LEN)//发送DETA_LEN个数据到服务器进行汇总
//							{
//								pMainFrm->nCountNumber2 = 0;
//								nTempStruct.nUnit  = BACKING;
//								nTempStruct.nState = SENDDATA;
//								nTempStruct.nCount = DETA_LEN;
//								memcpy(m_reportPtr2,&nTempStruct,sizeof(MyStruct));
//								nTPtr2 = m_reportPtr2;
//								nTPtr2 +=sizeof(MyStruct);
//								memcpy(nTPtr2,nCheckSendData2,DETA_LEN*sizeof(MyErrorType));
//								memset(nCheckSendData2,0,DETA_LEN*sizeof(MyErrorType));
//								QByteArray ba(m_reportPtr2, DETA_LEN*sizeof(MyErrorType)+sizeof(MyStruct));
//								pMainFrm->ncSocketWriteData2.push_back(ba);
//								memset(m_reportPtr2,0,DETA_LEN*sizeof(MyErrorType)+sizeof(MyStruct));
//								//CLogFile::write(QString("Save Date2,count:%1").arg(++count2),CheckLog);
//							}
////						}
//						nlastNumber2 = nSignalNo2;
//						
//					}
//				}
//			}
//		}
//		Sleep(1);
//	}
//	return TRUE;
//}


SysMainUI::SysMainUI(QWidget *parent, Qt::WFlags flags)
	: QDialog(parent)
{
	pMainFrm = this;
	for (int i=0;i<CAMERA_MAX_COUNT;i++)
	{
		nQueue[i].listDetect.clear();
		pdetthread[i] = NULL;
		m_SavePicture[i].pThat=NULL;
		m_sRunningInfo.m_bIsCheck[i] = TRUE;
		pHandles[i] = CreateEvent(NULL,FALSE,NULL,NULL);
	}
	m_vcolorTable.clear();
	for (int i = 0; i < 256; i++)  
	{  
		nSendData1[i].id = 0;
		nSendData1[i].nErrorArea = 0;
		nSendData1[i].nType = 0;
		nSendData1[i].nMoldNo = 0;

		nSendData2[i].id = 0;
		nSendData2[i].nErrorArea = 0;
		nSendData2[i].nType = 0;
		nSendData2[i].nMoldNo = 0;

		m_vcolorTable.append(qRgb(i, i, i)); 
	}
	CherkerAry.pCheckerlist=NULL;
	surplusDays=0;

	nCountNumber1 = 0;
	nCountNumber2 = 0;
	
	//网络通信初始化
	nIOCard = new int[IOCard_MAX_COUNT*24];

    for(int i = 0; i < IOCard_MAX_COUNT;i++)
    {
        m_cCombine[i] = new CCombineRlt;
        m_vIOCard[i] = nullptr;
    }

}
QString SysMainUI::getVersion()const
{
    //根据程序编译器,显示第三位是32还是64
    QString sVer = GetLocalPogramVersion();
    if(!sVer.isEmpty())
    {
        QStringList tstrl = sVer.split(".",QString::SkipEmptyParts);
        QString verRes = tr("Version:");
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
        sVer = verRes;
    }
    return sVer;
}
SysMainUI::~SysMainUI()
{
	delete nIOCard;
    for(int i = 0; i < IOCard_MAX_COUNT;i++)
    {
        delete m_cCombine[i];
    }
}
void SysMainUI::Init()
{
	InitParameter();
	ReadIniInformation();
	Initialize();
	initInterface();
	CLogFile::write(tr("startdetect!"),OperationLog);
	StartDetectThread();
	StartCamGrab();
	//initSocket();

	CLogFile::write(tr("Enter system!"),OperationLog);
}
//初始化
void SysMainUI::Initialize()
{
	LoadParameterAndCam();
	InitImage();
	InitIOCard();
	InitCheckSet();
	initDetectThread();
}
//采集回调函数
void WINAPI GlobalGrabOverCallback (const s_GBSIGNALINFO* SigInfo)
{
	if (SigInfo && SigInfo->Context)
	{
		SysMainUI* pGlasswareDetectSystem = (SysMainUI*) SigInfo->Context;
		pGlasswareDetectSystem->GrabCallBack(SigInfo);
	}
}
//采集回调函数
void SysMainUI::GrabCallBack(const s_GBSIGNALINFO *SigInfo)
{
	int iRealCameraSN = SigInfo->nGrabberSN;
	if (iRealCameraSN==-1 || !m_sRunningInfo.m_bCheck)
	{
		return;
	}
    m_iGrabCounter[iRealCameraSN]++;
	if( m_iGrabCounter[iRealCameraSN] == 65536)
		 m_iGrabCounter[iRealCameraSN] = 0;
	if(SigInfo->nErrorCode != GBOK)
	{
		s_GBERRORINFO ErrorInfo;
		m_sRealCamInfo[iRealCameraSN].m_pGrabber->GetLastErrorInfo(&ErrorInfo);
		QString str = QString((QString("Camera:%1,")+QString("Error code:%2,")+QString("Error description:%3,")+QString("Additional information:%4")).arg(iRealCameraSN+1).arg(ErrorInfo.nErrorCode).arg(ErrorInfo.strErrorDescription).arg(ErrorInfo.strErrorRemark));		
		CLogFile::write(QString("GrabCallBack:") + str ,CheckLog);
		return;
	}

	int imgNumber = 0;
	int tempI = 0;
	if (m_sSystemInfo.m_bIsIOCardOK)
	{
		imgNumber = ReadImageSignal(iRealCameraSN);
        if(m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast != imgNumber)
        {
            m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast = imgNumber;
            m_sRealCamInfo[iRealCameraSN].m_iGrabImageCount = 1;
        }
        else
        {
            m_sRealCamInfo[iRealCameraSN].m_iGrabImageCount++;
        }
	}else
	{
        if (-1 == m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast)
        {
            m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast++;
        }
        if (m_sRealCamInfo[iRealCameraSN].m_iGrabImageCount >= m_sRealCamInfo[iRealCameraSN].m_iImageTargetNo)
        {
            m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast++;
            m_sRealCamInfo[iRealCameraSN].m_iGrabImageCount = 0;
        }
        m_sRealCamInfo[iRealCameraSN].m_iGrabImageCount++;
        if (m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast >= 256)
        {
            m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast = 0;
        }
        imgNumber = m_sRealCamInfo[iRealCameraSN].m_iImageIdxLast;
    }
    //CLogFile::write(QString("Camera:%1 ImgSN:%2 Image Read.").arg(iRealCameraSN).arg(imgNumber), DebugLog,0);
	//******************采集:得到图像缓冲区地址****************************//
	int tempCamera = iRealCameraSN;
	uchar* pImageBuffer = NULL;
	int nAddr = 0;
	int nWidth, nHeight;

    mutexDetectElement[iRealCameraSN].lock();
    m_sRealCamInfo[iRealCameraSN].m_pGrabber->GetParamInt(GBImageBufferAddr, nAddr);
    m_sRealCamInfo[iRealCameraSN].m_pGrabber->GetParamInt(GBImageWidth, nWidth);
    m_sRealCamInfo[iRealCameraSN].m_pGrabber->GetParamInt(GBImageHeight, nHeight);
#if WIN_32_1_10
	pImageBuffer = (uchar*)nAddr;
#else
	int nAddr2;
	__int64 lAddr, lAddr2, lAddr3;
	if (m_sRealCamInfo[iRealCameraSN].m_pGrabber->GetParamInt(GBImageBufferAddr2, nAddr2)) //只有sg加了该功能
	{
		lAddr  = (__int64)nAddr & 0xFFFFFFFF;
		lAddr2 = ((__int64)nAddr2) << 32;
		lAddr3 = lAddr | lAddr2;
		pImageBuffer = (uchar*)lAddr3;
	}
	else
	{
		pImageBuffer = (uchar*)nAddr;
	}
#endif
    mutexDetectElement[iRealCameraSN].unlock();

	CGrabElement *pGrabElement = NULL;
	if(nQueue[tempCamera].listGrab.count()>0)
	{
		pGrabElement = (CGrabElement *) nQueue[tempCamera].listGrab.first();
        nQueue[tempCamera].listGrab.removeFirst();
		pGrabElement->bHaveImage=TRUE;
		pGrabElement->nCheckRet = FALSE;
		pGrabElement->cErrorParaList.clear();
		pGrabElement->nWidth = nWidth;
		pGrabElement->nHeight = nHeight;
		pGrabElement->nSignalNo = imgNumber;
		pGrabElement->nCamSN = tempCamera;
		try
		{
			memcpy(pGrabElement->SourceImage->bits(),pImageBuffer,nWidth*nHeight);
		}
		catch(...)
		{
			CLogFile::write(QString("m_pGrabber->GetParamInt(GBImageBufferAddr, nAddr):: nAddr is NULL!CamNo:%1,ImgNo:%2").arg(iRealCameraSN).arg(imgNumber),CheckLog);
			pImageBuffer=new uchar[nWidth*nHeight];
			memset(pImageBuffer,0,nWidth*nHeight);
			memcpy(pGrabElement->SourceImage->bits(),pImageBuffer,nWidth*nHeight);
			delete pImageBuffer;
			pImageBuffer=NULL;
		}

		if (nQueue[tempCamera].InitID == pGrabElement->initID)
		{
			m_detectElement[tempCamera].ImageNormal = pGrabElement;
			m_detectElement[tempCamera].iCameraNormal = tempCamera;
			if(1 == m_sCarvedCamInfo[tempCamera].m_iStress)
			{
				m_detectElement[tempCamera].iType = 1;
			}else{
				m_detectElement[tempCamera].iType = 0;
			}
			nQueue[tempCamera].mDetectLocker.lock();
			nQueue[tempCamera].listDetect.append(m_detectElement[tempCamera]);
            nQueue[tempCamera].mDetectLocker.unlock();
            //CLogFile::write(QString("Camera:%1 ImgSN:%2 Image come.").arg(tempCamera).arg(pGrabElement->nSignalNo), DebugLog,0);
		}
		else
		{
			delete pGrabElement;
		}
	}
}
//配置初始化信息
void SysMainUI::InitParameter()
{
	// 注册s_MSGBoxInfo至元对象系统,否则s_MSGBoxInfo,s_ImgWidgetShowInfo，s_StatisticsInfo无法作为参数进行传递
	qRegisterMetaType<s_MSGBoxInfo>("s_MSGBoxInfo"); 
	qRegisterMetaType<e_SaveLogType>("e_SaveLogType");  
	qRegisterMetaType<QList<QRect>>("QList<QRect>"); 
    qRegisterMetaType<BottleResult>("BottleResult");
	//初始化路径
	QString path = QApplication::applicationFilePath();  
	appPath = path.left(path.findRev("/")+1);
	m_sConfigInfo.m_strAppPath = appPath;
	CLogFile::write(tr("Get Version"),AbnormityLog);

	//获取文件版本号
	sVersion = getVersion();

	//配置文件在run目录中位置
	m_sConfigInfo.m_strConfigPath = "Config/Config.ini";
	m_sConfigInfo.m_strDataPath = "Config/Data.ini";
	if (sLanguage == 0)
	{
		m_sConfigInfo.m_strErrorTypePath = "Config/ErrorType.ini";
	}else{
		m_sConfigInfo.m_strErrorTypePath = "Config/ErrorType-en.ini";
	}
	m_sConfigInfo.m_strPLCStatusTypePath = "Config/PLCStatusType.ini";
	m_sConfigInfo.m_sAlgFilePath = "ModelInfo";// 算法路径 [10/26/2010 GZ]	
	m_sConfigInfo.m_sRuntimePath = "Config/runtime.ini";

	//配置文件绝对路径
	m_sConfigInfo.m_strConfigPath = appPath + m_sConfigInfo.m_strConfigPath;
	m_sConfigInfo.m_strDataPath = appPath + m_sConfigInfo.m_strDataPath;
	m_sConfigInfo.m_strErrorTypePath = appPath + m_sConfigInfo.m_strErrorTypePath;
	m_sConfigInfo.m_strPLCStatusTypePath = appPath + m_sConfigInfo.m_strPLCStatusTypePath;
	m_sConfigInfo.m_sAlgFilePath = appPath + m_sConfigInfo.m_sAlgFilePath;
	m_sConfigInfo.m_sRuntimePath = appPath + m_sConfigInfo.m_sRuntimePath;

	SaveDataPath=appPath+"./LastData.ini";
	
	//初始化相机参数
	for (int i = 0;i<CAMERA_MAX_COUNT;i++)
	{
		m_sRealCamInfo[i].m_bGrabIsStart = FALSE;
	}

}
//
//void SysMainUI::onSocketDataReady()
//{
//	QTcpSocket* nTcpSocket = dynamic_cast<QTcpSocket*>(sender());
//	int byteAvi=nTcpSocket->bytesAvailable();
//	if (byteAvi < DETA_LEN * sizeof(MyErrorType)+sizeof(MyStruct) )
//	{
//		CLogFile::write(QString("bytesAvailable small,%1").arg(byteAvi),AbnormityLog);
//		return;
//	}
//	int PacketCount= byteAvi / (DETA_LEN * sizeof(MyErrorType)+sizeof(MyStruct));
//	QByteArray buffer = nTcpSocket->read(PacketCount * (DETA_LEN * sizeof(MyErrorType)+sizeof(MyStruct)));
//	char* t_ptr = buffer.data();
//	PacketCount = buffer.size() / (DETA_LEN * sizeof(MyErrorType)+sizeof(MyStruct)) ;
//	for(int i=0;i<PacketCount;i++)
//	{
//		t_ptr += i *  (DETA_LEN * sizeof(MyErrorType)+sizeof(MyStruct));
//		QByteArray Tmpdate(t_ptr,DETA_LEN * sizeof(MyErrorType)+sizeof(MyStruct));
//		if(((MyStruct*)Tmpdate.data())->nState == CONNECT)
//		{
//			if(m_sSystemInfo.m_iDeviceOnInitial != 2)
//			{	//同步服务器端过检总数和剔废数
//				m_sRunningInfo.m_checkedNum = ((MyStruct*)t_ptr)->nCount;
//				m_sRunningInfo.m_failureNum = ((MyStruct*)t_ptr)->nFail;
//			}
//		}
//		else if( ((MyStruct*)Tmpdate.data())->nState == PLCWARN )
//		{
//			if(m_sSystemInfo.m_iDeviceOnInitial != 2)
//			{	//同步服务器端PLC报警
//				emit signals_plcstatus( ((MyStruct*)Tmpdate.data())->nPLCStatus);
//			}
//		}
//		else if( ((MyStruct*)Tmpdate.data())->nState == STARTDETECT )
//		{
//			if(m_sSystemInfo.m_iDeviceOnInitial != 2)
//			{	//与服务器端同步开启或关闭检测
//				if(((MyStruct*)Tmpdate.data())->nStartDecect == 0)
//				{
//					if (m_sRunningInfo.m_bCheck)
//						emit signals_startdetect(true);
//				}
//				else if(((MyStruct*)Tmpdate.data())->nStartDecect == 1)
//				{
//					if (!m_sRunningInfo.m_bCheck)
//						emit signals_startdetect(true);
//				}
//			}	
//		}
//	}
//	
//}
//
//void SysMainUI::onServerDataReady()
//{
//	QTcpSocket* nTcpSocket = dynamic_cast<QTcpSocket*>(sender());
//	int byteAvi=nTcpSocket->bytesAvailable();
//	if (byteAvi < DETA_LEN * sizeof(MyErrorType)+sizeof(MyStruct) )
//	{
//		CLogFile::write(QString("bytesAvailable small"),AbnormityLog);
//		return;
//	}
//	int PacketCount= byteAvi / (DETA_LEN * sizeof(MyErrorType)+sizeof(MyStruct));
//	QByteArray buffer = nTcpSocket->read(PacketCount * (DETA_LEN * sizeof(MyErrorType)+sizeof(MyStruct)));
//	char* t_ptr = buffer.data();
//	//CLogFile::write(QString("bytesAvailable : %2 ,buffer:%1").arg(buffer.size()).arg(byteAvi),AbnormityLog);
//	PacketCount = buffer.size() / (DETA_LEN * sizeof(MyErrorType)+sizeof(MyStruct)) ;
//	for(int i=0;i<PacketCount;i++)
//	{
//		t_ptr += i *  (DETA_LEN * sizeof(MyErrorType)+sizeof(MyStruct));
//		QByteArray Tmpdate(t_ptr,DETA_LEN * sizeof(MyErrorType)+sizeof(MyStruct));
//		if(((MyStruct*)Tmpdate.data())->nState == CONNECT)
//		{
//			QTime nTime =  QTime::currentTime();
//			for(int i=0;i<2;i++)
//			{
//				if(nTcpSocket->peerAddress().toString() == IPAddress[i].ipAddress)
//				{	//心跳包，包含实时的过检总数和剔废数
//					IPAddress[i].nstate = true;
//					IPAddress[i].startTime = nTime.second();
//					MyStruct temp;
//					temp.nState = CONNECT;
//					temp.nCount = pMainFrm->m_sRunningInfo.m_checkedNum;
//					temp.nFail = pMainFrm->m_sRunningInfo.m_failureNum;
//					static char *m_CntPtr=new char[DETA_LEN * sizeof(MyErrorType) + sizeof(MyStruct)];
//					memset(m_CntPtr,0,DETA_LEN * sizeof(MyErrorType) + sizeof(MyStruct));
//					memcpy(m_CntPtr,&temp,sizeof(MyStruct));
//					int ret = nTcpSocket->write(m_CntPtr,DETA_LEN * sizeof(MyErrorType)+sizeof(MyStruct));
//					//CLogFile::write(QString("tcpserver cnt Packet Recv IP:%1 , time:%2 ,writeLen:%3").arg(IPAddress[i].ipAddress).arg(IPAddress[i].startTime).arg(ret),AbnormityLog);
//				}
//			}
//		}
//		else if(((MyStruct*)Tmpdate.data())->nState == SENDDATA)
//		{	//保存客户端检测数据
//			//nCountLock.lock();
//			if(((MyStruct*)Tmpdate.data())->nUnit == LEADING)
//			{
//// 				static int date1Count = 0;
//// 				CLogFile::write(QString("get data1 Len:%1,Count:%2").arg(Tmpdate.size()).arg(++date1Count),AbnormityLog);
//				nDataCount[0].push_back(Tmpdate);
//			}
//			else if(((MyStruct*)Tmpdate.data())->nUnit == CLAMPING)
//			{
//// 				static int date2Count = 0;
//// 				CLogFile::write(QString("get data2 Len:%1,Count:%2").arg(Tmpdate.size()).arg(++date2Count),AbnormityLog);
//				nDataCount[1].push_back(Tmpdate);
//			}
//			else if(((MyStruct*)Tmpdate.data())->nUnit == BACKING)
//			{
//// 				static int date3Count = 0;
//// 				CLogFile::write(QString("get data3 Len:%1,Count:%2").arg(Tmpdate.size()).arg(++date3Count),AbnormityLog);
//				nDataCount[2].push_back(Tmpdate);
//			}
//			//nCountLock.unlock();
//		}
//		else if(((MyStruct*)Tmpdate.data())->nState == STARTDETECT)
//		{	//与客户端端同步开启或关闭检测
//			if(((MyStruct*)Tmpdate.data())->nStartDecect == 0)
//			{
//				if (m_sRunningInfo.m_bCheck)
//					emit signals_startdetect(true);
//			}
//			else if(((MyStruct*)Tmpdate.data())->nStartDecect == 1)
//			{
//				if (!m_sRunningInfo.m_bCheck)
//					emit signals_startdetect(true);
//			}
//		}
//	}
//	
//}
//
//void SysMainUI::ServerNewConnection()
//{
//	QTcpSocket* tcp = m_tcpServer->nextPendingConnection(); //获取新的客户端信息
//	if(tcp->peerAddress().toString() == m_sSystemInfo.i_IP2 )
//		m_tcpClient = tcp ;			//保存客户端socket,用于给客户端发送消息
//	onServerConnected(tcp->peerAddress().toString(),true);
//	connect(tcp, SIGNAL(readyRead()), this, SLOT(onServerDataReady()));
//}
//
//void SysMainUI::onServerConnected(QString IPAddress,bool nState)
//{
//	if(IPAddress == m_sSystemInfo.i_IP1)
//	{
//		if(nState)
//			sockLed1->turnOn();
//		else
//			sockLed1->turnOff();
//	}
//	else if(IPAddress == m_sSystemInfo.i_IP2)
//	{
//		if(nState)
//			sockLed2->turnOn();
//		else
//			sockLed2->turnOff();
//	}
//	
//}
//
//void SysMainUI::SendStartDecect(int pStartDetect)
//{
//	MyStruct nTempStruct;
//	nTempStruct.nState = STARTDETECT;
//	nTempStruct.nPLCStatus = pStartDetect;
//	static char *m_PLCPacket=new char[DETA_LEN * sizeof(MyErrorType) + sizeof(MyStruct)];;
//	memset(m_PLCPacket,0,DETA_LEN * sizeof(MyErrorType) + sizeof(MyStruct));
//	memcpy(m_PLCPacket,&nTempStruct,sizeof(MyStruct));
//	if(m_sSystemInfo.m_iDeviceOnInitial == 2 && m_tcpClient != NULL )
//	{
//		int ret = m_tcpClient->write(m_PLCPacket,DETA_LEN * sizeof(MyErrorType) + sizeof(MyStruct));
//		CLogFile::write(QString("clamp SendStartDecect ret=%1").arg(ret),AbnormityLog);
//	}
//	else if(m_sSystemInfo.m_iDeviceOnInitial == 1 && m_tcpSocket != NULL)
//	{
//		int ret = m_tcpSocket->write(m_PLCPacket,DETA_LEN * sizeof(MyErrorType) + sizeof(MyStruct));
//		CLogFile::write(QString("front SendStartDecect ret=%1").arg(ret),AbnormityLog);
//	}
//}

void SysMainUI::CountDefectIOCard(int iCamera, int ImageNumber, int tmpResult)
{
    if(m_sCarvedCamInfo[iCamera].m_iIOCardSN < 0 || m_sCarvedCamInfo[iCamera].m_iIOCardSN>= IOCard_MAX_COUNT ||
        m_cCombine[m_sCarvedCamInfo[iCamera].m_iIOCardSN] == NULL)
    {
        return;
    }
    int nCardSN = m_sCarvedCamInfo[iCamera].m_iIOCardSN;
    auto pCard = m_vIOCard[nCardSN];
    int comResult = -1;//综合后的结果
    auto cCombine = m_cCombine[m_sCarvedCamInfo[iCamera].m_iIOCardSN];
	cCombine->AddResult(ImageNumber, iCamera, tmpResult);
	if (cCombine->ConbineResult(ImageNumber, 0, comResult))//图像都拍完后结果综合
	{
        //处理当前图像号之前的5个图像号
        for	(int i = ImageNumber - 5; i<ImageNumber ;i++)
        {
        	if (!cCombine->IsReject((i+256)%256))
        	{
        		m_sRunningInfo.nCompensateKick[nCardSN]++;
                CombineBottle(iCamera, i, cCombine->m_Rlts[(i+256)%256].iResult, true);
            }
        }
        //重置接下来的5个图像号剔废状态
		for	(int i = ImageNumber; i < ImageNumber + 5;i++)
		{
			cCombine->SetReject(i%256,false);
        }

        CombineBottle(iCamera, ImageNumber, comResult);		
	}
}

void SysMainUI::CombineBottle(int iCamera, int ImageNumber, int tmpResult, bool CompensateKick)
{
    if(m_sCarvedCamInfo[iCamera].m_iIOCardSN < 0 || m_sCarvedCamInfo[iCamera].m_iIOCardSN>= IOCard_MAX_COUNT ||
        m_cCombine[m_sCarvedCamInfo[iCamera].m_iIOCardSN] == NULL)
    {
        return;
    }
    int nCardSN = m_sCarvedCamInfo[iCamera].m_iIOCardSN;
    auto pCard = m_vIOCard[nCardSN];
    int comResult = -1;//综合后的结果
    auto cCombine = m_cCombine[m_sCarvedCamInfo[iCamera].m_iIOCardSN];
    BottleResult result;
    if(m_bCombine.ConbineResult(ImageNumber, result))
    {
        if(result.idLine == "" )
        {//未识别
            if(pMainFrm->m_sSystemInfo.bKickReadFailed)
            {
                //NOTE:统计未识别剔废
                tmpResult = 1;
            }
        }else
        {
            //得到识别正确的相机列表
            QList<int> camList = m_bCombine.successCameraList(result);
            m_sRunningInfo.nRead ++;
            m_sRunningInfo.nInTime ++;
            emit signals_updateResult(result);
            if(pMainFrm->m_sSystemInfo.bKickMarked)
            {
                if(pMainFrm->m_sSystemInfo.markedID.contains(result.idLine))
                {//标记ID剔废,设置剔废
                    //NOTE:统计标记ID剔废
                    tmpResult = 1;
                }
                else if(pMainFrm->m_sSystemInfo.markedCavity.contains(result.cavityNum))
                {//标记来源产线剔废
                    //NOTE:统计标记来源产线剔废
                    tmpResult = 1;
                }
            }
        }
        emit signals_updateResult(result);
        s_ResultInfo sResultInfo;
        sResultInfo.tmpResult = valueByKickMode(nCardSN, tmpResult);
        sResultInfo.nImgNo = ImageNumber;
        sResultInfo.nIOCardNum = 0;
        if (m_sSystemInfo.m_bIsIOCardOK)
        {
            pCard->SendResult(sResultInfo);
            if(CompensateKick)
            {
                //暂时使用无用变量作为总的综合踢废数目 by zl
                m_sRunningInfo.nCompensateCardKick[2] += 1;
                if(sResultInfo.tmpResult == 1)
                {
                    CLogFile::write(QString("CompensateKick IMGSN:%1 nCardSN:%2 Kick:%3")
                        .arg(sResultInfo.nImgNo).arg(nCardSN).arg(sResultInfo.tmpResult), DebugLog, 0);
                }
            }
        }
        cCombine->SetReject(ImageNumber);
        cCombine->RemoveOneResult(ImageNumber);
        m_bCombine.RemoveOneResult(ImageNumber);

        if(CompensateKick)//按照原逻辑补踢不计数
            return;
        if (m_sRunningInfo.m_bCheck)	
        {
            m_sRunningInfo.nTotal[nCardSN]++;
            if(!m_sSystemInfo.m_bIsIOCardOK )
            {//模拟下 无相机情况下,只在综合里增加计数
                m_sRunningInfo.m_checkedNum++;
                m_sRunningInfo.nTotalBot++;
                if(comResult)
                {//模拟下的剔废计数
                    m_sRunningInfo.m_failureNum ++;
                    m_sRunningInfo.nRejectBot ++;
                }
            }
            //获取此图像号的模号
            int pTmpMoldNo = cCombine->GetMoldNo(ImageNumber);
            cCombine->RemoveMoldNo(ImageNumber);

            int iErrorCamera = cCombine->ErrorCamera(ImageNumber);
            s_ErrorPara sComErrorpara = cCombine->ConbineError(ImageNumber);
            if (pMainFrm->m_sRunningInfo.m_cErrorTypeInfo[iErrorCamera].ErrorTypeJudge(sComErrorpara.nErrorType))
            {
                pMainFrm->m_sRunningInfo.m_cErrorTypeInfo[iErrorCamera].iErrorCountByType[sComErrorpara.nErrorType]+=1;
                pMainFrm->m_sRunningInfo.m_iErrorCamCount[iErrorCamera] += 1;
                //暂时使用无用变量作为总的综合踢废数目 by zl
                m_sRunningInfo.nKick[nCardSN]++;
                pMainFrm->m_sRunningInfo.m_iErrorTypeCount[sComErrorpara.nErrorType] +=1;

                //if( pMainFrm->m_sSystemInfo.m_bIsIOCardOK )
                //{
                //	if (nCardSN == 0)
                //	{
                //		pMainFrm->nSendData1[ImageNumber].id = iErrorCamera;
                //		pMainFrm->nSendData1[ImageNumber].nType = sComErrorpara.nErrorType;
                //		pMainFrm->nSendData1[ImageNumber].nErrorArea = sComErrorpara.nArea;
                //		if (pMainFrm->m_sSystemInfo.m_iDeviceOnInitial == 2)//只有夹持瓶底读模号，前后壁设置为0
                //			pMainFrm->nSendData1[ImageNumber].nMoldNo = pTmpMoldNo;
                //		else
                //			pMainFrm->nSendData1[ImageNumber].nMoldNo = 0;

                //	}
                //	else
                //	{
                //		pMainFrm->nSendData2[ImageNumber].id = iErrorCamera;
                //		pMainFrm->nSendData2[ImageNumber].nType = sComErrorpara.nErrorType;
                //		pMainFrm->nSendData2[ImageNumber].nErrorArea = sComErrorpara.nArea;
                //		pMainFrm->nSendData2[ImageNumber].nMoldNo = 0;
                //	}
                //	//CLogFile::write(QString("IOCard:%1,ImgNo:%2,CamNo:%3,ErrorType:%4,ErrorArea:%5").arg(nCardSN).arg(ImageNumber).arg(iErrorCamera).arg(sComErrorpara.nErrorType).arg(sComErrorpara.nArea),CheckLog);
                //}
            }
            else
            {
                pMainFrm->m_sRunningInfo.m_cErrorTypeInfo[iErrorCamera].iErrorCountByType[0]+=1;
                pMainFrm->m_sRunningInfo.m_iErrorTypeCount[0] +=1;
            }
        }
    }
}

bool SysMainUI::isLock() const
{
    return bLock;
}

bool SysMainUI::checkLock()
{
    if(!isLock())
        return true;
    CPasswordDlg PassDlg;
    PassDlg.exec();
    return PassDlg.isOK();
}

//读取配置信息
void SysMainUI::ReadIniInformation()
{
	QSettings iniset(m_sConfigInfo.m_strConfigPath,QSettings::IniFormat);
	iniset.setIniCodec(QTextCodec::codecForName("GBK"));
	QSettings erroriniset(m_sConfigInfo.m_strErrorTypePath,QSettings::IniFormat);
	erroriniset.setIniCodec(QTextCodec::codecForName("GBK"));
	QSettings PLCStatusiniset(m_sConfigInfo.m_strPLCStatusTypePath,QSettings::IniFormat);
	PLCStatusiniset.setIniCodec(QTextCodec::codecForName("GBK"));
	QSettings iniDataSet(m_sConfigInfo.m_strDataPath,QSettings::IniFormat);
	iniDataSet.setIniCodec(QTextCodec::codecForName("GBK"));

	QSettings runtimeCfg(pMainFrm->m_sConfigInfo.m_sRuntimePath,QSettings::IniFormat);
	runtimeCfg.setIniCodec(QTextCodec::codecForName("GBK"));

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
			m_sErrorInfo.m_vstrErrorType.append(QString::fromLocal8Bit(erroriniset.value(strSession,"NULL").toString()));//.toLatin1().data()));
			m_sErrorInfo.m_cErrorReject.iErrorCountByType[i] = 0;
		}
	}
	m_sErrorInfo.m_vstrErrorType.append(tr("Unknown Defect"));//.toLatin1().data()));

	strSession = QString("/StatusType/total");
	int  StatusTypeNumber= PLCStatusiniset.value(strSession,0).toInt();
	for (int i=0;i<StatusTypeNumber;i++)
	{
		strSession = QString("/StatusType/%1").arg(i);
		m_vstrPLCInfoType.append(QString::fromLocal8Bit(PLCStatusiniset.value(strSession,"NULL").toString()));//.toLatin1().data()));
	}
	//读取系统参数
	m_sSystemInfo.IsCarve = iniset.value("/system/IsCarve",false).toBool();
	m_sSystemInfo.nLoginHoldTime = iniset.value("/system/nLoginHoldTime",10).toInt();	//是否报警统计
	m_sSystemInfo.m_strWindowTitle = QObject::tr("Glass Bottle Detect System");//读取系统标题
	m_sSystemInfo.m_iTest = iniset.value("/system/Test",0).toInt();
	m_sSystemInfo.m_bDebugMode = iniset.value("/system/DebugMode",0).toInt();	//读取是否debug

	m_sSystemInfo.m_bIsIOCardOK=iniset.value("/system/isUseIOCard",1).toInt();	//是否使用IO卡
	m_sSystemInfo.m_bIsStopNeedPermission=iniset.value("/system/IsStopPermission",0).toBool();	//是否使用IO卡
    m_sSystemInfo.iIOCardCount=iniset.value("/system/iIOCardCount",1).toInt();	//读取IO卡个数
	m_sSystemInfo.iIOCardID=iniset.value("/system/iIOCardID",0).toInt();	//读取1张卡IO卡ID
	m_sSystemInfo.iRealCamCount = iniset.value("/GarbCardParameter/DeviceNum",2).toInt();	//真实相机个数
	
	m_sSystemInfo.m_bIsTest = iniset.value("/system/IsTest",0).toInt();//是否是测试模式
	m_sSystemInfo.iIsButtomStress = iniset.value("/system/IsButtomStree",0).toInt();//是否有瓶底应力
	m_sSystemInfo.iIsSaveCountInfoByTime = iniset.value("/system/IsSaveCountInfoByTime",0).toInt();//是否保存指定时间段内的统计信息
	m_sSystemInfo.iIsSample = iniset.value("/system/IsSample",0).toInt();//是否取样
	m_sSystemInfo.iIsCameraCount = iniset.value("/system/IsCameraCount",0).toInt();//是否统计各相机踢废率
	m_sSystemInfo.LastModelName = iniset.value("/system/LastModelName","default").toString();	//读取上次使用模板
	m_sSystemInfo.m_iIsTrackStatistics = iniset.value("/system/isTrackStatistics",0).toInt();	//是否报警统计
	m_sSystemInfo.m_iTrackNumber = iniset.value("/system/TrackNumber",10).toInt();	//报警统计个数
	m_sSystemInfo.m_NoKickIfNoFind = iniset.value("/system/NoKickIfNoFind",0).toInt();	//报警类型
	m_sSystemInfo.m_NoKickIfROIFail = iniset.value("/system/NoKickIfROIFail",0).toInt();	//报警类型	

	m_sSystemInfo.m_iImageStretch = iniset.value("/system/ImageStretch",1).toInt();	//图像横向排布还是上下排布
	m_sSystemInfo.m_iSaveNormalErrorImageByTime = iniset.value("/system/SaveNormalErrorImageByTime",0).toInt();	
	m_sSystemInfo.m_iSaveStressErrorImageByTime = iniset.value("/system/SaveStressErrorImageByTime",0).toInt();	
	m_sSystemInfo.m_iStopOnConveyorStoped = iniset.value("/system/stopCheckWhenConveyorStoped",0).toBool();	//输送带停止是否停止检测
	m_sSystemInfo.fPressScale = iniset.value("/system/fPressScale",1).toDouble();	//瓶身应力增强系数
	m_sSystemInfo.fBasePressScale = iniset.value("/system/fBasePressScale",1).toDouble();	//瓶底应力增强系数
	m_sSystemInfo.m_strModelName = m_sSystemInfo.LastModelName;

	m_sSystemInfo.bCameraOffLineSurveillance = iniset.value("/system/bCameraOffLineSurveillance",0).toBool();	
	m_sSystemInfo.bCameraContinueRejectSurveillance = iniset.value("/system/bCameraContinueRejectSurveillance",0).toBool();	
	m_sSystemInfo.iCamOfflineNo = iniset.value("/system/iCamOfflineNo",10).toInt();	
	m_sSystemInfo.iCamContinueRejectNumber = iniset.value("/system/iCamContinueRejectNumber",10).toInt();	
	m_sSystemInfo.iReadMouldIDCamNo = iniset.value("/system/ReadMouldIDCamNo",-1).toInt(); //读取模号的相机号 默认是8号相机
	m_sSystemInfo.iMaxReadMouldIDCount = iniset.value("/system/MaxReadMouldIDCount",1000).toInt();
	

	m_sSystemInfo.i_IP1 = iniset.value("system/ip1","192.168.250.200").toString();
	m_sSystemInfo.i_IP2 = iniset.value("system/ip2","192.168.250.201").toString();
	/*IpStruct nIptemp;
	nIptemp.ipAddress=m_sSystemInfo.i_IP1 ;
	nIptemp.nstate = false;
	IPAddress<<nIptemp;
	nIptemp.ipAddress=m_sSystemInfo.i_IP2 ;
	nIptemp.nstate = false;
	IPAddress<<nIptemp;*/

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
	//shift
	m_sSystemInfo.iSaveRecordInterval=iniset.value("system/iSaveRecordInterval",30).toInt();
	m_sSystemInfo.bSaveRecord = iniset.value("system/isSaveRecord",true).toBool();
	m_sSystemInfo.bAutoSetZero = iniset.value("system/isAutoClear",true).toBool();
	m_sSystemInfo.shift1Time = QTime::fromString(iniset.value("/system/shift1Time","060000").toString(),"hhmmss");
	m_sSystemInfo.shift2Time = QTime::fromString(iniset.value("/system/shift2Time","150000").toString(),"hhmmss");
	m_sSystemInfo.shift3Time = QTime::fromString(iniset.value("/system/shift3Time","230000").toString(),"hhmmss");

	QTime currentTm=QTime::currentTime();
	if(currentTm >= m_sSystemInfo.shift1Time &&  currentTm < m_sSystemInfo.shift2Time)
		currentShift = 0;
	else if(currentTm >= m_sSystemInfo.shift2Time &&  currentTm < m_sSystemInfo.shift3Time)
		currentShift = 1;
	else if(currentTm <m_sSystemInfo.shift1Time || currentTm >= m_sSystemInfo.shift3Time)
		currentShift = 2;

	
	//设置剪切参数路径
	m_sConfigInfo.m_strGrabInfoPath = m_sConfigInfo.m_strAppPath + "ModelInfo/" + m_sSystemInfo.m_strModelName + "/GrabInfo.ini";

	//切割后相机个数
	m_sSystemInfo.iCamCount = iniset.value("/system/CarveDeviceCount",1).toInt();

	for(int i=0; i < pMainFrm->m_sSystemInfo.iCamCount; i++)
	{
		if(i >= pMainFrm->m_sSystemInfo.iRealCamCount)
		{
			m_sCarvedCamInfo[i].m_iToRealCamera = i - pMainFrm->m_sSystemInfo.iRealCamCount;
		}else
		{
			m_sCarvedCamInfo[i].m_iToRealCamera = i;
		}
	}
	for (int i=0;i<m_sSystemInfo.iRealCamCount;i++)
	{
		struGrabCardPara[i].iGrabberTypeSN = EGT_Dalsa_X64;
		strSession = QString("/GarbCardParameter/Device%1ID").arg(i+1);
        struGrabCardPara[i].nGrabberSN = iniset.value(strSession,-1).toInt();
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
	}

	QSettings iniCameraSet(m_sConfigInfo.m_strGrabInfoPath,QSettings::IniFormat);
	QString strShuter,strTrigger;
	for(int i = 0; i < m_sSystemInfo.iRealCamCount; i++)
	{
		strShuter = QString("/Shuter/Grab_%1").arg(i);
		strTrigger = QString("/Trigger/Grab_%1").arg(i);
		m_sRealCamInfo[i].m_iShuter=iniCameraSet.value(strShuter,20).toInt();
		m_sRealCamInfo[i].m_iTrigger=iniCameraSet.value(strTrigger,1).toInt();//默认外触发
	}
	
	//read Equipment maintenance Config
	m_sRuntimeInfo.isEnable = runtimeCfg.value("EquipAlarm/Enable",false).toBool();
	m_sRuntimeInfo.total = runtimeCfg.value("EquipAlarm/total",20).toInt();
	for (int i=0;i<m_sRuntimeInfo.total;i++)
	{
		m_sRuntimeInfo.AlarmsEnable << runtimeCfg.value(QString("EquipAlarm/Alarm%1_Enable").arg(i+1) , false ).toBool();
		m_sRuntimeInfo.AlarmsDays << runtimeCfg.value(QString("EquipAlarm/Alarm%1_Days").arg(i+1) , 0 ).toInt();
		m_sRuntimeInfo.AlarmsInfo << runtimeCfg.value(QString("EquipAlarm/Alarm%1_Info").arg(i+1) , "" ).toString();
	}

    m_sRunningInfo.refCardSN = m_sSystemInfo.iIOCardCount-1;
    //读取新加的剔废相关信息
    m_sSystemInfo.bKickReadFailed = iniset.value("system/KickReadFailed", false).toBool();
    m_sSystemInfo.bKickMarked = iniset.value("system/KickMarked", false).toBool();
    QString tmp = iniset.value("system/BottleIDList", "").toString();
    m_sSystemInfo.idlist = tmp.split(",", QString::SkipEmptyParts);
    tmp = iniset.value("system/MarkedIDList", "").toString();
    m_sSystemInfo.markedID = tmp.split(",", QString::SkipEmptyParts).toSet();
    tmp = iniset.value("system/BottleFromCavity", "").toString();
    m_sSystemInfo.cavityList = tmp.split(",", QString::SkipEmptyParts);
    tmp = iniset.value("system/MarkedCavityList", "").toString();
    m_sSystemInfo.markedCavity = tmp.split(",", QString::SkipEmptyParts).toSet();
}
//读取切割信息
void SysMainUI::ReadCorveConfig()
{
	QSettings iniCarveSet(m_sConfigInfo.m_strGrabInfoPath,QSettings::IniFormat);
	QString strSession;
	for(int i=0; i < pMainFrm->m_sSystemInfo.iCamCount; i++)
	{
		//加载剪切后参数
		strSession = QString("/angle/Grab_%1").arg(i);
		m_sCarvedCamInfo[i].m_iImageAngle = iniCarveSet.value(strSession,0).toInt();
		strSession = QString("/Stress/Device_%1").arg(i);
		m_sCarvedCamInfo[i].m_iStress = iniCarveSet.value(strSession,0).toInt();
		strSession = QString("/tonormal/Grab_%1").arg(i);
		m_sCarvedCamInfo[i].m_iToNormalCamera = iniCarveSet.value(strSession,i).toInt();
		strSession = QString("/pointx/Grab_%1").arg(i);
		m_sCarvedCamInfo[i].i_ImageX = iniCarveSet.value(strSession,i).toInt();
		strSession = QString("/pointy/Grab_%1").arg(i);
		m_sCarvedCamInfo[i].i_ImageY = iniCarveSet.value(strSession,i).toInt();
		strSession = QString("/width/Grab_%1").arg(i);
		m_sCarvedCamInfo[i].m_iImageWidth = iniCarveSet.value(strSession,i).toInt();
		strSession = QString("/height/Grab_%1").arg(i);
		m_sCarvedCamInfo[i].m_iImageHeight = iniCarveSet.value(strSession,i).toInt();
		strSession = QString("/convert/Grab_%1").arg(i);
		m_sCarvedCamInfo[i].m_iToRealCamera = iniCarveSet.value(strSession,i).toInt();

		if ((m_sCarvedCamInfo[i].i_ImageX + m_sCarvedCamInfo[i].m_iImageWidth) > m_sRealCamInfo[i].m_iImageWidth)
		{
			m_sCarvedCamInfo[i].i_ImageX = 0;
			m_sCarvedCamInfo[i].m_iImageWidth = m_sRealCamInfo[i].m_iImageWidth;
		}
		if ((m_sCarvedCamInfo[i].i_ImageY + m_sCarvedCamInfo[i].m_iImageHeight) > m_sRealCamInfo[i].m_iImageHeight)
		{
			m_sCarvedCamInfo[i].i_ImageY = 0;
			m_sCarvedCamInfo[i].m_iImageHeight = m_sRealCamInfo[i].m_iImageHeight;
		}
	}
}
//加载参数和相机
void SysMainUI::LoadParameterAndCam()
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
//初始化采集卡（：初始化相机）
void SysMainUI::InitGrabCard(s_GBINITSTRUCT struGrabCardPara,int index)
{
	QString strDeviceName = QString(struGrabCardPara.strDeviceName);
	if (strDeviceName=="SimulaGrab")
	{
		m_sRealCamInfo[index].m_pGrabber = new CDHGrabberSG;
		m_sRealCamInfo[index].m_bSmuGrabber = true;
		m_sRealCamInfo[index].m_iGrabType = 0;
	}
	else if (strDeviceName == "MER")
	{
		m_sRealCamInfo[index].m_pGrabber = new CDHGrabberMER;
		m_sRealCamInfo[index].m_bSmuGrabber = false;
		m_sRealCamInfo[index].m_iGrabType = 8;
	}else if (strDeviceName == "AVT")
	{
		m_sRealCamInfo[index].m_pGrabber = new CDHGrabberAVT;
		m_sRealCamInfo[index].m_bSmuGrabber = false;
		m_sRealCamInfo[index].m_iGrabType = 1;
	}
	BOOL bRet = FALSE;
	int iErrorPosition = 0;
	try
	{
		bRet = m_sRealCamInfo[index].m_pGrabber->Init(&struGrabCardPara);	

		if(bRet)
		{
			iErrorPosition = 1;
			m_sRealCamInfo[index].m_bCameraInitSuccess=TRUE;
            int temp=0;
			bRet = m_sRealCamInfo[index].m_pGrabber->GetParamInt(GBImageWidth, temp);
            m_sRealCamInfo[index].m_iImageWidth = temp;
			if(bRet)
			{
				iErrorPosition = 2;
				bRet = m_sRealCamInfo[index].m_pGrabber->GetParamInt(GBImageHeight, temp);
                m_sRealCamInfo[index].m_iImageHeight=temp;
				if(bRet)
				{
					iErrorPosition = 3;
					bRet = m_sRealCamInfo[index].m_pGrabber->GetParamInt(GBImageBufferSize,temp);	
                     m_sRealCamInfo[index].m_iImageSize=temp;
					if(bRet)
					{
						iErrorPosition = 4;
						int nImagePixelSize = 0;
						bRet = m_sRealCamInfo[index].m_pGrabber->GetParamInt(GBImagePixelSize, nImagePixelSize);
						if(bRet)
						{
							int result=0;
							bRet = m_sRealCamInfo[index].m_pGrabber->GetParamInt(GBImageBufferAddr, result);
							if(bRet)
							{
								iErrorPosition = 5;
								m_sRealCamInfo[index].m_iImageBitCount =8* nImagePixelSize;
								if(strDeviceName == "MER")
								{
									iErrorPosition = 6;
									((CDHGrabberMER*)m_sRealCamInfo[index].m_pGrabber)->MERSetParamInt(MERSnapMode,1);
								}
								if(strDeviceName == "AVT")
								{
									iErrorPosition = 6;
									((CDHGrabberAVT*)m_sRealCamInfo[index].m_pGrabber)->AVTSetTriggerParam(AVTTriggerOn);
								}
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
		strError = QString("catch camera%1 initial error").arg(index);
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
			tr("ErrorDescription:%3").arg(ErrorInfo.strErrorDescription)+"\n"+\
			tr("ErrorRemark:%4\n").arg(ErrorInfo.strErrorRemark)+"\n"+\
			tr("ErrorPosition:%5\n").arg(iErrorPosition);
		QMessageBox::information(this,tr("Error"),str);
		QString strError;
		strError = QString("camera%1initial error,ErrorPosition%2").arg(index).arg(iErrorPosition);
		CLogFile::write(strError,OperationLog);
		m_sRealCamInfo[index].m_strErrorInfo = str;
	}
}
//初始化相机（设置曝光时间和触发方式）
void SysMainUI::InitCam()
{
	for(int i = 0; i < m_sSystemInfo.iRealCamCount; i++)
	{
		if(m_sRealCamInfo[i].m_iGrabType == 8)
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
		if(m_sRealCamInfo[i].m_iGrabType == 1)
		{
			if(m_sRealCamInfo[i].m_iTrigger == 1)
			{
				((CDHGrabberAVT*)m_sRealCamInfo[i].m_pGrabber)->AVTSetTriggerParam(AVTTriggerOn);
				m_sRealCamInfo[i].m_bGrabIsTrigger = true;
	}
			else if(m_sRealCamInfo[i].m_iTrigger == 0)
			{
				((CDHGrabberAVT*)m_sRealCamInfo[i].m_pGrabber)->AVTSetTriggerParam(AVTTriggerOff);
				m_sRealCamInfo[i].m_bGrabIsTrigger = false;
			}
			((CDHGrabberAVT*)m_sRealCamInfo[i].m_pGrabber)->AVTSetParamPro(AVTShutter,m_sRealCamInfo[i].m_iShuter);
		}
	}
}
//初始化图像（：读取切割信息:初始化图像队列和剪切后相机参数）
void SysMainUI::InitImage()
{
	for (int i = 0;i<pMainFrm->m_sSystemInfo.iCamCount;i++)
	{
		int iRealCameraSN = m_sCarvedCamInfo[i].m_iToRealCamera;
		m_sCarvedCamInfo[i].m_iResImageWidth = m_sRealCamInfo[iRealCameraSN].m_iImageWidth;
		m_sCarvedCamInfo[i].m_iResImageHeight = m_sRealCamInfo[iRealCameraSN].m_iImageHeight;
		m_sCarvedCamInfo[i].m_iImageType = m_sRealCamInfo[iRealCameraSN].m_iImageType;
		m_sCarvedCamInfo[i].m_iIOCardSN =  m_sRealCamInfo[iRealCameraSN].m_iIOCardSN;
	}

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
	ReadCorveConfig();
	QString strSession;
	for(int i = 0 ; i < m_sSystemInfo.iCamCount; i++)
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

		m_sCarvedCamInfo[i].m_iImageBitCount = m_sRealCamInfo[i].m_iImageBitCount;   //图像位数从相机处继承[8/7/2013 nanjc]
		m_sCarvedCamInfo[i].m_iImageRoAngle = m_sRealCamInfo[i].m_iImageRoAngle;
		// 错误统计用类
		m_sRunningInfo.m_cErrorTypeInfo[i].m_iErrorTypeCount = m_sErrorInfo.m_iErrorTypeCount;

		//实时显示用, 预分配QImage空间，每切出相机一个
		if (m_sCarvedCamInfo[i].m_pActiveImage!=NULL)
		{
			delete m_sCarvedCamInfo[i].m_pActiveImage;
			m_sCarvedCamInfo[i].m_pActiveImage = NULL;
		}
		
		m_sCarvedCamInfo[i].m_pActiveImage=new QImage(m_sCarvedCamInfo[i].m_iImageWidth,m_sCarvedCamInfo[i].m_iImageHeight,m_sCarvedCamInfo[i].m_iImageBitCount);// 用于实时显示
		
		m_sCarvedCamInfo[i].m_pActiveImage->setColorTable(m_vcolorTable);
		//开始采集前补一张黑图
		BYTE* pByte = m_sCarvedCamInfo[i].m_pActiveImage->bits();
		int iLength = m_sCarvedCamInfo[i].m_pActiveImage->byteCount();
		memset((pByte),0,(iLength));
		//分配图像剪切内存区域,大小等于真实相机大小
		if (m_sCarvedCamInfo[i].m_pGrabTemp!=NULL)
		{
			delete m_sCarvedCamInfo[i].m_pGrabTemp; 
			m_sCarvedCamInfo[i].m_pGrabTemp = NULL;
		}
		m_sCarvedCamInfo[i].m_pGrabTemp = new BYTE[m_sRealCamInfo[i].m_iImageWidth*m_sRealCamInfo[i].m_iImageHeight];
		//分配元素链表中图像的内存，每剪切出来的相机10个。
		nQueue[i].InitCarveQueue(m_sCarvedCamInfo[i].m_iImageWidth, m_sCarvedCamInfo[i].m_iImageHeight,m_sRealCamInfo[i].m_iImageWidth,m_sRealCamInfo[i].m_iImageHeight,m_sCarvedCamInfo[i].m_iImageBitCount, 10, true);
		for (int k = 0; k < 256;k++)
		{
            if(m_sCarvedCamInfo[i].sImageLocInfo[k].m_AlgImageLocInfos.sXldPoint.nColsAry != NULL)
			    delete []m_sCarvedCamInfo[i].sImageLocInfo[k].m_AlgImageLocInfos.sXldPoint.nColsAry;
            if(m_sCarvedCamInfo[i].sImageLocInfo[k].m_AlgImageLocInfos.sXldPoint.nRowsAry != NULL)
			    delete []m_sCarvedCamInfo[i].sImageLocInfo[k].m_AlgImageLocInfos.sXldPoint.nRowsAry;

			m_sCarvedCamInfo[i].sImageLocInfo[k].m_iHaveInfo = 0;
			m_sCarvedCamInfo[i].sImageLocInfo[k].m_AlgImageLocInfos.sXldPoint.nCount = 0;
			m_sCarvedCamInfo[i].sImageLocInfo[k].m_AlgImageLocInfos.sXldPoint.nRowsAry = new int[BOTTLEXLD_POINTNUM];
			m_sCarvedCamInfo[i].sImageLocInfo[k].m_AlgImageLocInfos.sXldPoint.nColsAry = new int[BOTTLEXLD_POINTNUM];
			memset(m_sCarvedCamInfo[i].sImageLocInfo[k].m_AlgImageLocInfos.sXldPoint.nRowsAry,0, 4*BOTTLEXLD_POINTNUM);
			memset(m_sCarvedCamInfo[i].sImageLocInfo[k].m_AlgImageLocInfos.sXldPoint.nColsAry,0, 4*BOTTLEXLD_POINTNUM);
			// 				memset
		}
	}
	SetCarvedCamInfo();
	//初始化缺陷图像列表
	m_ErrorList.initErrorList(m_sSystemInfo.m_iMaxCameraImageWidth,m_sSystemInfo.m_iMaxCameraImageHeight,m_sSystemInfo.m_iMaxCameraImagePixelSize*8,ERROR_IMAGE_COUNT,true);
}
//设置剪切后相机的参数
void SysMainUI::SetCarvedCamInfo()
{
	for (int i = 0;i<m_sSystemInfo.iCamCount;i++)
	{
		int iRealCameraSN = m_sCarvedCamInfo[i].m_iToRealCamera;
		m_sCarvedCamInfo[i].m_iResImageWidth = m_sRealCamInfo[iRealCameraSN].m_iImageWidth;
		m_sCarvedCamInfo[i].m_iResImageHeight = m_sRealCamInfo[iRealCameraSN].m_iImageHeight;
		m_sCarvedCamInfo[i].m_iImageType = m_sRealCamInfo[iRealCameraSN].m_iImageType;
		m_sCarvedCamInfo[i].m_iIOCardSN =  m_sRealCamInfo[iRealCameraSN].m_iIOCardSN;
		m_sCarvedCamInfo[i].m_iShuter = m_sRealCamInfo[iRealCameraSN].m_iShuter;
		m_sCarvedCamInfo[i].m_iTrigger = m_sRealCamInfo[iRealCameraSN].m_iTrigger;
		m_sCarvedCamInfo[i].m_iGrabPosition = m_sRealCamInfo[iRealCameraSN].m_iGrabPosition;
	}
	SetCombineInfo();
}
//设置图像综合参数
void SysMainUI::SetCombineInfo()
{
    //初始化结果综合参数
    for (int i = 0;i<m_sSystemInfo.iCamCount;i++)
    {
        for(int j = 0; j < IOCard_MAX_COUNT;j++)
        {
            if(m_sCarvedCamInfo[i].m_iIOCardSN == j)
            {
                m_cCombine[m_sCarvedCamInfo[i].m_iIOCardSN]->SetCombineCamera(i,true);
                m_sSystemInfo.IOCardiCamCount[m_sCarvedCamInfo[i].m_iIOCardSN]++;
            }
            else
            {
                m_cCombine[j]->SetCombineCamera(i,false);
            }
        }
        m_bCombine.SetCombineCamera(i, true);
    }
    for(int i = 0; i < IOCard_MAX_COUNT;i++)
    {
        m_cCombine[i]->Inital(m_sSystemInfo.IOCardiCamCount[i]);
    }
    m_bCombine.Inital(m_sSystemInfo.iCamCount);
}
//初始化IO卡
void SysMainUI::InitIOCard()
{
	if (m_sSystemInfo.m_bIsIOCardOK)
	{
        for(int i = 0; i < m_sSystemInfo.iIOCardCount && i < IOCard_MAX_COUNT;i++)
        {
            if(i == 0)
            {
                m_sSystemInfo.m_sConfigIOCardInfo[i].strCardInitFile = QString("./PIO24B_reg_init.txt");
                m_sSystemInfo.m_sConfigIOCardInfo[i].strCardName = QString("PIO24B");
            }
            else
            {
                m_sSystemInfo.m_sConfigIOCardInfo[i].strCardInitFile = QString("./PIO24B_reg_init%1.txt").arg(i);
                m_sSystemInfo.m_sConfigIOCardInfo[i].strCardName = QString("PIO24B").arg(i);
            }
            m_sSystemInfo.m_sConfigIOCardInfo[i].iCardID = i;
            m_vIOCard[i] = new CIOCard(m_sSystemInfo.m_sConfigIOCardInfo[i],i);
            connect(m_vIOCard[i],SIGNAL(emitMessageBoxMainThread(s_MSGBoxInfo)),this,SLOT(slots_MessageBoxMainThread(s_MSGBoxInfo)));
            s_IOCardErrorInfo sIOCardErrorInfo = m_vIOCard[i]->InitIOCard();
            //Sleep(200);
            if (!sIOCardErrorInfo.bResult)
            {
                m_sSystemInfo.m_bIsIOCardOK = false;
                CLogFile::write(tr("Error in init IOCard:%1").arg(i),AbnormityLog);
            }
            else
            {
                SetCardKickMode(i, 2);
            }
        }
	}
}
//初始化算法
int SysMainUI::InitCheckSet()
{
	//算法初始化，模板调入等 [8/4/2010 GZ]
	s_Status  sReturnStatus;
	s_AlgInitParam   sAlgInitParam;	
	//	QSettings iniAlgSet(m_sConfigInfo.m_strConfigPath,QSettings::IniFormat);
	if(m_sSystemInfo.m_iMaxCameraImageWidth>m_sSystemInfo.m_iMaxCameraImageHeight)
	{
		sReturnStatus = init_bottle_module(m_sSystemInfo.m_iMaxCameraImageWidth,m_sSystemInfo.m_iMaxCameraImageWidth,1);
	}
	else
	{
		sReturnStatus = init_bottle_module(m_sSystemInfo.m_iMaxCameraImageHeight,m_sSystemInfo.m_iMaxCameraImageHeight,1);
	}
	if (sReturnStatus.nErrorID != RETURN_OK)
	{
		CLogFile::write(tr("----load model error----"),AbnormityLog);
		return -1;
	}	
	for (int i=0;i<m_sSystemInfo.iCamCount;i++)
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
			return -1;
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
	// 算法初始化，模板调入等 [8/4/2010 GZ]
	//////////////////////////////////////////////////////////////////////////
	if (CherkerAry.pCheckerlist != NULL)
	{
		delete[] CherkerAry.pCheckerlist;
	}
	CherkerAry.iValidNum = m_sSystemInfo.iCamCount;
	CherkerAry.pCheckerlist = new s_CheckerList[CherkerAry.iValidNum];

    return 0;
}
//开启相机采集
void SysMainUI::StartCamGrab()
{ 
	for (int i = 0;i<m_sSystemInfo.iRealCamCount;i++)
	{
		m_sRealCamInfo[i].m_pGrabber->StartGrab();
		m_sRealCamInfo[i].m_bGrabIsStart=TRUE;
	}
}
//开启检测线程
void SysMainUI::StartDetectThread()
{
	m_bIsThreadDead = FALSE;
	for (int i=0;i<m_sSystemInfo.iCamCount;i++)
	{
		pdetthread[i]->start();
	}
}
void SysMainUI::initDetectThread()
{
	//m_bIsThreadDead = FALSE;
	//CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)SendDetect1, this, 0, NULL );
	//if (pMainFrm->m_sSystemInfo.m_iDeviceOnInitial == 1 && pMainFrm->m_sSystemInfo.iIOCardCount == 2 )
	//	CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)SendDetect2, this, 0, NULL );
	//CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)SendIOCard, this, 0, NULL );
	//if(m_sSystemInfo.m_iDeviceOnInitial == 2)
	//	CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)DataCountThread, this, 0, NULL );

	for (int i=0;i<m_sSystemInfo.iCamCount;i++)
	{
		pdetthread[i] = new DetectThread(this,i);
	}
}
//初始化界面
void SysMainUI::initInterface()
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog  | Qt::WindowSystemMenuHint);//去掉标题栏
	QDesktopWidget* desktopWidget = QApplication::desktop();
	QRect screenRect = desktopWidget->screenGeometry();
	setMinimumSize(screenRect.width(),screenRect.height());

	m_Datebase =new DataBase(m_sConfigInfo.m_strAppPath);

	QIcon icon;
	icon.addFile(QString::fromUtf8(":/sys/icon"), QSize(), QIcon::Normal, QIcon::Off);
	setWindowIcon(icon);

    buttonVisible = QVector<bool>((ENaviToolExit+1), true);

	statked_widget = new QStackedWidget();
	statked_widget->setObjectName("mainStacked");
    title_widget = new WidgetTitle(this);
    //操作页面
    auto idlst = slots_getDeviceList(1);
    widget_operation = new UIOperation(idlst);
    //算法设置页面
    widget_alg = new QWidget(this);
    widget_alg->setObjectName("widget_alg");
    //模板设置页面
	widget_article = new WidgetManagement;
    //系统设置页面
	widget_settings = new WidgetTest(this);
	//widget_settings->slots_intoWidget();
    //历史页面
	widget_history = new widget_count(this);
	widget_inout = new QWidget(this);
    widget_Alarm = new QWidget(this);
    auto widget_light = new QWidget(this);
    widget_light->setStyleSheet("border-image:url(:/sys/Light.png);");

    widget_Warning = new WidgetWarning(this);
	QPalette palette;
	palette.setBrush(QPalette::Window, QBrush(Qt::white));
	statked_widget->setPalette(palette);
	statked_widget->setAutoFillBackground(true);

	statked_widget->addWidget(widget_operation);
    statked_widget->addWidget(widget_alg);
    statked_widget->addWidget(widget_article);
    statked_widget->addWidget(widget_settings);
	statked_widget->addWidget(widget_history);
    statked_widget->addWidget(widget_inout);
    statked_widget->addWidget(widget_Alarm);
	statked_widget->addWidget(widget_light);
	//状态栏
	stateBar = new QWidget(this);
	stateBar->setFixedHeight(40);
    //stateBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	QGridLayout* gridLayoutStatusLight = new QGridLayout;
	for (int i = 0;i<pMainFrm->m_sSystemInfo.iCamCount;i++)
	{
		CameraStatusLabel *cameraStatus = new CameraStatusLabel(stateBar);
		cameraStatus->setObjectName("toolButtonCamera");
		cameraStatus->setAlignment(Qt::AlignCenter);
		cameraStatus->setText(QString::number(i+1));

		cameraStatus_list.append(cameraStatus);
		gridLayoutStatusLight->addWidget(cameraStatus,i%2,i/2);
	}
	//检查相机状态
	for (int i = 0; i < pMainFrm->m_sSystemInfo.iCamCount; i++)
	{
		int iRealCameraSN = pMainFrm->m_sCarvedCamInfo[i].m_iToRealCamera;
		CameraStatusLabel *cameraStatus = pMainFrm->cameraStatus_list.at(i);
		if (!pMainFrm->m_sRealCamInfo[iRealCameraSN].m_bCameraInitSuccess)
		{
			cameraStatus->SetCameraStatus(1);
		}
		else
		{
			cameraStatus->SetCameraStatus(0);
		}
	}

	/*QGridLayout *socketStatusLayout=new QGridLayout;
	sockLed1=new RLed;
	sockLed2=new RLed;
	QLabel *led1Label=new QLabel(tr("Clamp"));
	QLabel *led2Label=new QLabel(tr("Body"));
	socketStatusLayout->addWidget(sockLed1,0,0);
	socketStatusLayout->addWidget(led1Label,1,0);
	socketStatusLayout->addColSpacing(1,20);
	socketStatusLayout->addWidget(sockLed2,0,2);
	socketStatusLayout->addWidget(led2Label,1,2);*/

	QFont fontCoder;
	fontCoder.setPixelSize(28);
	labelCoder = new QLabel(stateBar);
	labelCoder->setFont(fontCoder);
	timerUpdateCoder = new QTimer(this);
	timerUpdateCoder->setInterval(1000);
	timerUpdateCoder->start();
	nConnectTimer = new QTimer(this);
	nConnectTimer->setInterval(10000);
	nConnectTimer->start();

	QHBoxLayout* hLayoutStateBar = new QHBoxLayout(stateBar);
	hLayoutStateBar->addLayout(gridLayoutStatusLight);
	hLayoutStateBar->addSpacing(30);
	//if(m_sSystemInfo.m_iDeviceOnInitial == 2)
	//	hLayoutStateBar->addLayout(socketStatusLayout);
	hLayoutStateBar->addStretch();
	hLayoutStateBar->addWidget(labelCoder);
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
	main_layout->setContentsMargins(0, 0, 0, 0);

	setLayout(main_layout);

    connect(this,SIGNAL(signals_updateResult(BottleResult)),widget_operation, SIGNAL(signals_updateResult(BottleResult)));
    connect(this,SIGNAL(signals_clear()),widget_operation, SIGNAL(signals_clear()));
    connect(this,SIGNAL(signals_updateCount(int, int, int, int, int)),widget_operation, SIGNAL(signals_updateCount(int, int, int, int, int)));

    connect(widget_settings,SIGNAL(signals_sendAlarm(int, QString)),widget_Warning,SLOT(slots_ShowWarning(int, QString)));
    connect(this,SIGNAL(signals_ShowWarning(int , QString )),widget_Warning,SLOT(slots_ShowWarning(int , QString )));	
    connect(this,SIGNAL(signals_HideWarning(int)),widget_Warning,SLOT(slots_HideWarning(int)));	
    connect(widget_settings,SIGNAL(signals_ShowWarning(int , QString )),widget_Warning,SLOT(slots_ShowWarning(int , QString )));	

	connect(title_widget, SIGNAL(showMin()), this, SLOT(showMinimized()));
	connect(title_widget, SIGNAL(closeWidget()), this, SLOT(slots_OnExit()));
    connect(title_widget, SIGNAL(turnPage(int)), this, SLOT(slots_turnPage(int)));
    connect(this, SIGNAL(signals_setNaviLockSt(bool)), title_widget, SLOT(slots_setLockState(bool)));
	connect(this,SIGNAL(signals_intoManagementWidget()),widget_article,SLOT(slots_intoWidget()));	
	connect(this,SIGNAL(signals_intoTestWidget()),widget_settings,SLOT(slots_intoWidget()));	
    connect(timerUpdateCoder, SIGNAL(timeout()), this, SLOT(slots_UpdateCoderNumber()));  

	connect(widget_history ,SIGNAL(updateRecordSet()),this,SLOT(slots_UpdateRecordSet()));
	connect(widget_history ,SIGNAL(updateShiftSet()),this,SLOT(slots_UpdateShiftSet()));
	//connect(timerUpdateCoder, SIGNAL(timeout()), widget_count, SLOT(slots_updateInfo()));    

	//connect(this,SIGNAL(signals_clear()),widget_count,SLOT(slots_ClearCountInfo()));	

	//connect(nConnectTimer, SIGNAL(timeout()), this, SLOT(slot_ConnectedSock()));   
    connect(widget_article, SIGNAL(signals_clearTable()),this, SIGNAL(signals_clearTable()));
	connect(this,SIGNAL(signals_startdetect(bool)),this,SLOT(slots_OnBtnStar(bool)));
 	for (int i = 0;i < pMainFrm->m_sSystemInfo.iCamCount;i++)
	{
		connect(pMainFrm->pdetthread[i], SIGNAL(signals_upDateCamera(int,int)), this, SLOT(slots_updateCameraState(int,int)));
	}
    connect(widget_operation->image_widget, SIGNAL(signals_SetCameraStatus(int,int)), this, SLOT(slots_SetCameraStatus(int,int)));
    connect(this, SIGNAL(signals_clearTable()), widget_operation->errorList_widget, SLOT(slots_clearTable()));

	timerSaveCount=new QTimer(this);
	timerSaveCount->setInterval(60*1000);//每分钟触发一次
	connect(timerSaveCount, SIGNAL(timeout()), this, SLOT(slots_SaveCountBytime()));  
	connect(timerSaveCount, SIGNAL(timeout()), this, SLOT(slots_SaveCountByShift()));  
	timerSaveCount->start();

	connect(this,SIGNAL(signals_updateCount(bool)),this,SLOT(UpdateCountForShow(bool)));
	//Load last Data
	m_Datebase->queryLastMoldNoData(m_sMoldNoCount);
	m_Datebase->queryLastData(m_sRunningInfo.m_checkedNum,m_sRunningInfo.m_failureNum,nRunInfo);
	LastRunInfo = nRunInfo;
	LastMoldNoCount = m_sMoldNoCount;
	emit signals_updateCount(true);


	//初始权限
	m_eLastMainPage = ENaviPageOperation;
	iLastPage = ENaviPageOperation;
	title_widget->turnPage("0");
	skin.fill(QColor(90,90,90,120));
}

//void SysMainUI::initSocket()
//{
//	if (m_sSystemInfo.m_iDeviceOnInitial == 2)
//	{
//		m_tcpServer = new QTcpServer(this);
//		m_tcpServer->listen(QHostAddress::Any,8088);
//		connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(ServerNewConnection()));
//	}
//	m_tcpSocket = new QTcpSocket(this);
//	m_tcpSocket->connectToHost(m_sSystemInfo.i_IP1,8088);
//	if(m_tcpSocket->waitForConnected(3000))
//	{
//		connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(onSocketDataReady()));
//	}else{
//		m_tcpSocket->abort();
//	}
//	//time(&nConnectStartTime);
//}

//bool SysMainUI::SendDataToSever(int nSendCount,StateEnum nState)
//{
//	MyStruct nTempStruct;
//	nTempStruct.nState = nState;
//	static char *m_CnttmpPtr=new char[DETA_LEN * sizeof(MyErrorType) + sizeof(MyStruct)];;
//	memset(m_CnttmpPtr,0,DETA_LEN * sizeof(MyErrorType) + sizeof(MyStruct));
//	memcpy(m_CnttmpPtr,&nTempStruct,sizeof(MyStruct));
//
//	int ret = m_tcpSocket->write(m_CnttmpPtr,DETA_LEN * sizeof(MyErrorType) + sizeof(MyStruct));
//	if(ret == -1)
//		return false;
//	else
//		return true;
//
//	//CLogFile::write(QString("TcpSocket Send Connect"),DebugLog);
//}

int SysMainUI::valueByKickMode(int cardSN, int result)const
{
    int tmpResult=0;
    switch (m_sRunningInfo.m_iKickMode[cardSN])
    {
    case 0:			// 连续踢 
        return 1;
        break;
    case 1:			// 持续好
        return 0;
        break;
    case 2:			// 正常踢
        return result;
        break;
    }
    return result;
}

void SysMainUI::SetCardKickMode(int cardSN, int mode)
{
    if(cardSN >= IOCard_MAX_COUNT )
    {
        return;
    }
    if(mode < 0 || mode > 2)
        mode = 2;
    /*if(m_sSystemInfo.m_bIsIOCardOK && m_vIOCard[cardSN] != nullptr)
    {
    m_vIOCard[cardSN]->SetOutMode(0, mode);
    }*/
    m_sRunningInfo.m_iKickMode[cardSN] = mode;
    
}

void SysMainUI::UpdateCountForShow(bool isFirst/*=false*/)
{
	if(isFirst)
		nRunInfo.iFailCount = nRunInfo.GetFailCount();
	widget_history->slots_updateCountInfo(nRunInfo.iAllCount,nRunInfo.iFailCount,0);
	widget_history->slots_UpdateTable1(nRunInfo);
	widget_history->slots_updateMoldNoCount();
// 	if (!isFirst)
// 	{ 
// 		m_Datebase->insertLastData(m_sRunningInfo.m_checkedNum,m_sRunningInfo.m_failureNum,nRunInfo);
// 		m_Datebase->insetLastMoldNoData(m_sMoldNoCount);
// 	}
}

void SysMainUI::SaveCountInfo()
{
	bool bIsEmptyFile = false;
	QString strFileName;
	strFileName = m_sConfigInfo.m_strAppPath + "CountInfo/timeCount/";
	QDir temp;
	bool exist = temp.exists(strFileName);
	if(!exist)
		temp.mkpath(strFileName);

	QDate date = QDate::currentDate();
	strFileName = strFileName +	date.toString(Qt::ISODate) + ".txt";
	if(!QFile::exists(strFileName))
	{
		QFile createFile(strFileName);
		if(!createFile.open(QFile::WriteOnly | QIODevice::Text))
		{
			return;
		}
		bIsEmptyFile = true;
		createFile.close();
	}
	QFile readFile(strFileName);
	if (!readFile.open(QFile::Append | QIODevice::Text))
	{
		return;
	}
	QFile writeFile(strFileName);
	//读入流和写入流
	QTextStream writeStream(&writeFile);
	if (!bIsEmptyFile)
	{
		writeStream<<"\n";
	}
	QTime time = QTime::currentTime();
	writeStream<<tr("Time:  %1:%2:%3").arg(time.hour()).arg(time.minute()).arg(time.second())<<"\t";
	writeStream<<tr("All Count:  %1").arg(nTmpcountData.iAllCount)<<"\t";
	writeStream<<tr("Fail Count:  %1").arg(nTmpcountData.GetFailCount())<<"\t";
	writeStream<<tr("Fail Rate:  %1%").arg(nTmpcountData.GetFailRate()*100 ,2,'f',2)<<"\n";

	writeStream<<tr("Front Count:  %1").arg(nTmpcountData.GetFrontCount())<<"\t";
	writeStream<<tr("Clamp Count:  %1").arg(nTmpcountData.GetClampCount())<<"\t";
	writeStream<<tr("Rear Count:  %1").arg(nTmpcountData.GetRearCount())<<"\n";

	for(int i=1;i<m_sErrorInfo.m_iErrorTypeCount;i++)
	{
		writeStream<<m_sErrorInfo.m_vstrErrorType[i] + ":" + QString::number(nTmpcountData.GetErrorByTypeCount(i))<<"\t";
	}
	writeStream<<"\n";
	writeStream<<tr("Results")<<"\t\t";
	writeStream<<tr("Count")<<"\t";
	writeStream<<tr("front")<<"\t";
	writeStream<<tr("clamp")<<"\t";
	writeStream<<tr("rear") <<"\t";
	writeStream<<"\n";

	for(int i=1;i<m_sErrorInfo.m_iErrorTypeCount;i++)
	{
		int pErrorByType = nTmpcountData.GetErrorByTypeCount(i);
		if(pErrorByType != 0)
		{
			QString tempString=m_sErrorInfo.m_vstrErrorType[i];
			writeStream<<tempString;
			if (tempString.length() <= 4 )
				writeStream<<"\t\t";
			else
				writeStream<<"\t";
			writeStream<<QString::number(pErrorByType)<<"\t";
			writeStream<<QString::number(nTmpcountData.iFrontErrorByType[i])<<"\t";
			writeStream<<QString::number(nTmpcountData.iClampErrorByType[i])<<"\t";
			writeStream<<QString::number(nTmpcountData.iRearErrorByType[i])<<"\n";
		}
	}
	writeStream<<"\n";
	if (!writeFile.open(QFile::Append | QIODevice::Text))
	{
		return;
	}
	writeStream.flush();//写入流到文件
	writeFile.close();

	return;
}

void SysMainUI::SaveToDatebase()
{
	QDateTime dateTime = QDateTime::currentDateTime();
	nTmpcountData += nRunInfo - LastRunInfo;
	bool ret = m_Datebase->insert(dateTime.toString("yyyyMMddhhmm"),nTmpcountData);
	LastRunInfo = nRunInfo;
	QDate dateSelecte = QDate::currentDate();
	QString temp = dateSelecte.toString(Qt::ISODate);
	temp.replace("-", "");
	QList<long long> tmpTimes;
	QList<cErrorInfo> tmpInfos;
	m_Datebase->queryByDay(temp ,tmpTimes,tmpInfos);
	widget_history->slots_UpdateTable2(tmpTimes,tmpInfos);

	//将每小时的模号统计存入数据库
	nTmpMoldNoCount += m_sMoldNoCount - LastMoldNoCount ;
	ret = m_Datebase->insertMoldNoData(dateTime.toString("yyyyMMddhhmm"),nTmpMoldNoCount);
	LastMoldNoCount = m_sMoldNoCount;
	QList<long long> tmpMoldNoTimes;
	QList<cMoldNoErrorInfo> tmpMoldNoInfos;
	m_Datebase->queryMoldNoByDay(temp ,tmpMoldNoTimes,tmpMoldNoInfos);
	widget_history->slots_updateMoldNoInfo(tmpMoldNoTimes,tmpMoldNoInfos);

	return;
}

void SysMainUI::ClearCount(bool isChangeShift /*= true*/)
{
	if(!isChangeShift)
	{
		if (QMessageBox::No == QMessageBox::question(this,tr("clear"),
			tr("Are you sure to clear?"),
			QMessageBox::Yes | QMessageBox::No))	
		{
			return;
		}

// 		nTmpcountData.Clear();
// 		//nTmpcountData += nRunInfo - LastRunInfo;
// 		nRunInfo.Clear();
// 		LastRunInfo.Clear();
// 
// 		nTmpMoldNoCount.clear();
// 		//nTmpMoldNoCount += m_sMoldNoCount -LastMoldNoCount;
// 		LastMoldNoCount.clear();
// 		m_sMoldNoCount.clear();
	}
	nCountLock.lock();
	nTmpcountData.Clear();
	nRunInfo.Clear();
	LastRunInfo.Clear();

	nTmpMoldNoCount.clear();
	LastMoldNoCount.clear();
	m_sMoldNoCount.clear();

	for(int i=0;i<3;i++)
	{
		nDataCount[i].clear();
	}

	ncSocketWriteData1.clear();
	ncSocketWriteData2.clear();

	nCountLock.unlock();
	m_sRunningInfo.m_checkedNum = 0;
	m_sRunningInfo.m_failureNum = 0;

	emit signals_updateCount(false);
}

void SysMainUI::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Escape:
        break;
    default:
        QDialog::keyPressEvent(e);
    }
}

void SysMainUI::slots_UpdateCoderNumber()
{
	int nCodeNum=0,nCheckNum=0,nSignNum=0;//nShowNum =0;

    if(m_sRunningInfo.m_bCheck)
    {
        if (m_sSystemInfo.m_bIsIOCardOK)
        {//通过读卡,更新软件的实际统计数据
            nCheckNum = m_vIOCard[0]->ReadCounter(3);	 
            nSignNum  = m_vIOCard[0]->ReadCounter(4);
            //nSignNum = m_vIOCard[1]->ReadCounter(4);
            nCodeNum = m_vIOCard[0]->ReadCounter(13);
            if (m_sRunningInfo.m_iKickMode[1] != 2)
            {
                m_sRunningInfo.m_passNum = nCheckNum;
                m_sRunningInfo.m_kickoutNumber = nSignNum;
            }
            else
            {
                if((nCheckNum - m_sRunningInfo.m_passNum>0)&&(nCheckNum - m_sRunningInfo.m_passNum<50))
                {	//计算总的过检总数 = 以前的过检总数 + 当前过瓶数 - 之前的过瓶数
                    m_sRunningInfo.m_checkedNum = m_sRunningInfo.m_checkedNum + nCheckNum - m_sRunningInfo.m_passNum;
                }
                m_sRunningInfo.m_passNum = nCheckNum;
                if ((nSignNum - m_sRunningInfo.m_kickoutNumber > 0) && (nSignNum - m_sRunningInfo.m_kickoutNumber < 50))
                {
                    m_sRunningInfo.m_failureNum = m_sRunningInfo.m_failureNum + nSignNum - m_sRunningInfo.m_kickoutNumber;
                }
                m_sRunningInfo.m_kickoutNumber = nSignNum;
            }		
        }      
    }
	QString strValue,strEncoder,strTime;
	strValue ="	";
	strEncoder += QString(tr("Speed:") +m_sRunningInfo.strSpeed+strValue+tr("Coder Number")+":%1").arg( nCodeNum);
	strTime = strValue+QString(tr("Time:"))+QTime::currentTime().toString() + strValue+sVersion;
	
	if(surplusDays>0)
	{
		labelCoder->setText(strEncoder+strTime+tr("Remaining days of use:%1 ").arg(surplusDays)); //剩余使用天数：%1
	}else{
		labelCoder->setText(strEncoder+strTime);
	}

    emit signals_updateCount(m_sRunningInfo.nTotalBot, m_sRunningInfo.nRejectBot, m_sRunningInfo.nRead, m_sRunningInfo.nInTime, m_sRunningInfo.nEngraved);
}
void SysMainUI::slots_updateCameraState(int nCam,int mode)
{
	cameraStatus_list.at(nCam)->BlinkCameraStatus(mode);
}
void SysMainUI::slots_SetCameraStatus(int nCam,int mode)
{
	cameraStatus_list.at(nCam)->SetCameraStatus(mode);
}
//裁剪
void SysMainUI::CarveImage(uchar* pRes,uchar* pTar,int iResWidth,int iResHeight,int iTarX,int iTarY,int iTarWidth,int iTarHeight)
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
	}
	catch(...)
	{
		CLogFile::write(tr("Error in image carve "),AbnormityLog);
	}
}

void SysMainUI::slots_turnPage(int current_page, int iPara)
{
	if (iLastPage == current_page)
	{
		return;
    }
    //离开之前页面
    switch(iLastPage)
    {
    case ENaviPageAlgSet:
        {
            s_Status  sReturnStatus;
            sReturnStatus = m_cBottleModel.CloseModelDlg();
            if (0 != sReturnStatus.nErrorID)
            {
                return ;
            }
        }
        break;
    case ENaviPageSettings:
        {
            widget_settings->leaveWidget();
        }
        break;
    }
    //进入当前页面
	switch ((ENavigation)current_page)
	{
    case ENaviPageArticle:
		emit signals_intoManagementWidget();
		break;
	case ENaviPageSettings:
		emit signals_intoTestWidget();
		break;
    case ENaviPageAlgSet:
        m_eCurrentMainPage = (ENavigation)current_page;
        statked_widget->setCurrentIndex(current_page);
        ShowCheckSet(iPara);
        m_eLastMainPage = m_eCurrentMainPage;
        iLastPage = current_page;
		break;
	case ENaviToolResetCount:
		ClearCount(false);
        break;
    case ENaviToolStart:
        slots_OnBtnStar();
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
    case ENaviToolExit:
        slots_OnExit();
        break;
	default:
		break;
    }
    if(current_page <= ENaviPageEndIndex && current_page != ENaviPageAlgSet && current_page != ENaviToolResetCount  )
    {
        m_eCurrentMainPage = (ENavigation)current_page;
        statked_widget->setCurrentIndex(current_page);
        m_eLastMainPage = m_eCurrentMainPage;
        iLastPage = current_page;
    }
}

void SysMainUI::slots_OnBtnStar(bool isRecv)
{
	if (m_sSystemInfo.m_bIsTest)
	{
		QMessageBox::information(this,tr("Infomation"),tr("Please Stop Test First!"));
		return;
	}
	ToolButton *TBtn = title_widget->button_list.at(ENaviToolStart);
	if (!m_sRunningInfo.m_bCheck )//开始检测
	{
		//图像综合清零
        for(int i = 0; i < IOCard_MAX_COUNT; i++)
        {
            m_cCombine[i]->m_MutexCombin.lock();
            m_cCombine[i]->RemovAllResult();
            m_cCombine[i]->RemovAllError();
            m_cCombine[i]->m_MutexCombin.unlock();
        }
		for(int i=0;i<3;i++)
		{
			nDataCount[i].clear();
		}
		ncSocketWriteData1.clear();
		ncSocketWriteData2.clear();

		if (m_sSystemInfo.m_bLoadModel)
		{
			//// 使能接口卡
			if (m_sSystemInfo.m_bIsIOCardOK)
			{
                for(int i = 0; i < m_sSystemInfo.iIOCardCount;i++)
                {
                    CLogFile::write(QString(tr("OpenIOCard%1")).arg(m_sSystemInfo.m_sConfigIOCardInfo[i].iCardID),OperationLog,0);
                    m_vIOCard[i]->enable(true);
                }
			}
			for (int i = 0; i < m_sSystemInfo.iRealCamCount;i++)
            {
                m_sRealCamInfo[i].m_iImageIdxLast = -1;
                m_iGrabCounter[i] = 0;
			}
			CLogFile::write(tr("Start Check"),OperationLog);
			m_sRunningInfo.m_bCheck = true;
        }
		else
		{
			QMessageBox::information(this,tr("Error"),tr("No Model,Please Load Model!"));
			return;
		}
		QPixmap pixmap(":/toolWidget/stop");
		TBtn->setText(tr("Stop"));
		TBtn->setIcon(pixmap);
		TBtn->bStatus = true;
	}
	else if (m_sRunningInfo.m_bCheck)//停止检测
	{
		if (m_sSystemInfo.m_bIsIOCardOK)
		{
            for(int i = 0; i < m_sSystemInfo.iIOCardCount;i++)
            {
                CLogFile::write(QString(tr("CloseIOCard%1")).arg(m_sSystemInfo.m_sConfigIOCardInfo[i].iCardID),OperationLog,0);
                m_vIOCard[i]->enable(false);
            }
		}
		// 停止算法检测 
		m_sRunningInfo.m_bCheck = false;
		CLogFile::write(tr("Stop Check"),OperationLog);

		QPixmap pixmap(":/toolWidget/start");
		TBtn->setText(tr("Start"));
		TBtn->setIcon(pixmap);

		for (int i = 0;i<m_sSystemInfo.iCamCount;i++)
		{
			s_SystemInfoforAlg sSystemInfoforAlg;
			sSystemInfoforAlg.bIsChecking = false;
			m_cBottleCheck[i].setsSystemInfo(sSystemInfoforAlg);
		}
		TBtn->bStatus = false;
	}
}

void SysMainUI::paintEvent(QPaintEvent *event)
{
	QWidget::paintEvent(event);
	QPainter painter(this);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::lightGray);
	painter.drawPixmap(QRect(0, 0, this->width(), this->height()), QPixmap(skin));
}
//弹出提示信息对话框
void SysMainUI::slots_MessageBoxMainThread(s_MSGBoxInfo msgbox)
{
	QMessageBox::information(this,msgbox.strMsgtitle,msgbox.strMsgInfo);	
}
//释放IO卡
void  SysMainUI::ReleaseIOCard()
{
	if (m_sSystemInfo.m_bIsIOCardOK)
	{
        for(int i = 0; i < IOCard_MAX_COUNT;i++)
        {
            if(m_vIOCard[i] != nullptr)
            {
                m_vIOCard[i]->CloseIOCard();
                delete m_vIOCard[i];
            }
        }
	}
}
// 关闭相机 [11/11/2010 zhaodt]
void SysMainUI::CloseCam()
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
	Sleep(1000);
	for (int i=0;i<m_sSystemInfo.iRealCamCount;i++)
	{
		if (m_sRealCamInfo[i].m_pGrabber!=NULL)
		{
			m_sRealCamInfo[i].m_pGrabber->Close();
		}
	}
}
//释放图像资源
void SysMainUI::ReleaseImage()
{
	for(int i = 0 ; i < m_sSystemInfo.iRealCamCount; i++)
	{
		delete m_sRealCamInfo[i].m_pRealImage;
		m_sRealCamInfo[i].m_pRealImage = NULL;
	}
	for(int i = 0 ; i < m_sSystemInfo.iCamCount; i++)
	{
		delete m_sCarvedCamInfo[i].m_pActiveImage;
		m_sCarvedCamInfo[i].m_pActiveImage = NULL;

		delete[] m_sCarvedCamInfo[i].m_pGrabTemp;
		m_sCarvedCamInfo[i].m_pGrabTemp = NULL;
		nQueue[i].releaseMemory();
		if (m_detectElement[i].bIsImageNormalCompelet)
		{
			delete m_detectElement[i].ImageNormal->myImage;
		}
	}
}
//释放所有资源
void SysMainUI::ReleaseAll()
{
	m_bIsThreadDead = TRUE;
	for(int i = 0; i < m_sSystemInfo.iCamCount; i++)
	{
		s_Status sReturnStatus = m_cBottleCheck[i].Free();
		for (int j = 0; j < 256;j++)
		{
			delete []m_sCarvedCamInfo[i].sImageLocInfo[j].m_AlgImageLocInfos.sXldPoint.nRowsAry;
			delete []m_sCarvedCamInfo[i].sImageLocInfo[j].m_AlgImageLocInfos.sXldPoint.nColsAry;
		}
	}
	if (CherkerAry.pCheckerlist != NULL)
	{
		delete[] CherkerAry.pCheckerlist;
	}
	ReleaseImage();
}

void SysMainUI::directoryChanged(QString path)
{
	QMessageBox::information(NULL, tr("Directory change"), path);
}

//功能：动态切换系统语言
bool SysMainUI::changeLanguage(int nLangIdx)
{
	QSettings sysSet("daheng","GlassDetectSystem");
	static QTranslator *translator = NULL, *qtDlgCN = NULL;
	bool bRtn = true;
	if (nLangIdx == 0)//中文
	{
		translator = new QTranslator;
		qtDlgCN = new QTranslator;
		if (translator->load("glasswaredetectsystem_zh.qm"))
		{
			qApp->installTranslator(translator);
			//中文成功后，加载Qt对话框标准翻译文件，20141202
			if (qtDlgCN->load("glasswaredetectsystem_zh.qm"))
			{
				qApp->installTranslator(qtDlgCN);
			}
			//保存设置
			sysSet.setValue("nLangIdx",nLangIdx);
		}
		else
		{
			QMessageBox::information(this,tr("Information"),tr("Load Language pack [glasswaredetectsystem_zh.qm] fail!"));
			//保存设置
			sysSet.setValue("nLangIdx",1);
			bRtn = false;
		}
	}
	return bRtn;
}

void SysMainUI::ShowCheckSet(int nCamIdx,int signalNumber)
{
	try
	{
		s_AlgModelPara  sAlgModelPara;	
		QImage tempIamge;

		if(slots_deviceType(nCamIdx) == 1 && widget_operation->image_widget->slots_checkCameraShowError(nCamIdx)&&pMainFrm->m_SavePicture[nCamIdx].pThat!=NULL)
		{
			tempIamge=pMainFrm->m_SavePicture[nCamIdx].m_Picture;
			sAlgModelPara.sImgLocInfo = widget_operation->image_widget->sAlgImageLocInfo[nCamIdx];
		}
        else{
			pMainFrm->nQueue[nCamIdx].mGrabLocker.lock();
			if(pMainFrm->nQueue[nCamIdx].listGrab.size()==0)
			{
				pMainFrm->nQueue[nCamIdx].mGrabLocker.unlock();
				return;
			}
			CGrabElement *pElement = pMainFrm->nQueue[nCamIdx].listGrab.last();
			tempIamge = (*pElement->myImage);
			sAlgModelPara.sImgLocInfo = pElement->sImgLocInfo;
			pMainFrm->nQueue[nCamIdx].mGrabLocker.unlock();
		}
		m_cBottleModel.CloseModelDlg();
		sAlgModelPara.sImgPara.nChannel = 1;
		sAlgModelPara.sImgPara.nHeight = tempIamge.height();
		sAlgModelPara.sImgPara.nWidth = tempIamge.width();
		sAlgModelPara.sImgPara.pcData = (char*)tempIamge.bits();
		
		if (sAlgModelPara.sImgPara.nHeight != pMainFrm->m_sCarvedCamInfo[nCamIdx].m_iImageHeight)
		{
			return;
		}
		if (sAlgModelPara.sImgPara.nWidth != pMainFrm->m_sCarvedCamInfo[nCamIdx].m_iImageWidth)
		{
			return;
		}		
		
		for (int i=0;i<m_sSystemInfo.iCamCount;i++)
		{
			CherkerAry.pCheckerlist[i].nID = i;
			CherkerAry.pCheckerlist[i].pChecker = &m_cBottleCheck[i];
		}	
		int widthd = widget_alg->geometry().width();
		int heightd	= widget_alg->geometry().height();
		if (widthd < 150 || heightd < 150)
		{
			return;
		}	
		s_Status  sReturnStatus = m_cBottleModel.SetModelDlg(sAlgModelPara,&m_cBottleCheck[nCamIdx],CherkerAry,widget_alg);
		if (sReturnStatus.nErrorID != RETURN_OK)
		{
			return;
		}
		statked_widget->setCurrentWidget(widget_alg);
		m_eCurrentMainPage = ENaviPageAlgSet;
		m_eLastMainPage = ENaviPageAlgSet;
		iLastPage = 3;
	}
	catch (...)
	{
	}
	CLogFile::write(tr("In to Alg Page")+tr("CamraNo:%1").arg(nCamIdx),OperationLog,0);
	return;	
}
void SysMainUI::slots_OnExit(bool ifyanz)
{
	if (ifyanz || QMessageBox::Yes == QMessageBox::question(this,tr("Exit"),
		tr("Are you sure to exit?"),
		QMessageBox::Yes | QMessageBox::No))	
	{
		if (m_sSystemInfo.m_bIsTest)
		{
			QMessageBox::information(this,tr("Infomation"),tr("Please Stop Test First!"));
			return;
		}
		if (m_sRunningInfo.m_bCheck )//开始检测
		{
			QMessageBox::information(this,tr("Infomation"),tr("Please Stop Detection First!"));
			return;		
		}
		//保存历史数据
		m_Datebase->insertLastData(m_sRunningInfo.m_checkedNum,m_sRunningInfo.m_failureNum,nRunInfo);
		m_Datebase->insetLastMoldNoData(m_sMoldNoCount);

		//emit widget_count->widgetCountSet->ui.pushButton_save;
		EquipRuntime::Instance()->EquipExitLogFile();
		ToolButton *TBtn = title_widget->button_list.at(ENaviToolExit);
		CLogFile::write(tr("Close ModelDlg!"),OperationLog);
		s_Status  sReturnStatus = m_cBottleModel.CloseModelDlg();
		if (sReturnStatus.nErrorID != RETURN_OK)
		{
			CLogFile::write(tr("Error in Close ModelDlg--OnExit"),AbnormityLog);
			return;
		}
		CloseCam();
		ReleaseAll();
		//QSettings iniset(m_sConfigInfo.m_strDataPath,QSettings::IniFormat);
		//iniset.setIniCodec(QTextCodec::codecForName("GBK"));
		//iniset.setValue("/system/failureNum",m_sRunningInfo.m_failureNumFromIOcard);
		//iniset.setValue("/system/checkedNum",m_sRunningInfo.m_checkedNum);
		ReleaseIOCard();
		exit(0);
		//close();
	}
}

int SysMainUI::slots_deviceType(int i)
{
    if(i < m_sSystemInfo.iRealCamCount)
    {
        return m_sRealCamInfo[i].m_iDevType;
    }
    else if(i >= m_sSystemInfo.iRealCamCount && i < m_sSystemInfo.iCamCount)
    {
        return m_sRealCamInfo[i - m_sSystemInfo.iRealCamCount].m_iDevType;
    }
    return -1;
}

QList<int> SysMainUI::slots_getDeviceList(int t)
{
    QList<int> res;
    for(int i = 0; i < m_sSystemInfo.iCamCount; i ++)
    {
        res.append(i);
    }
    return res;
}

void SysMainUI::writeLogText(QString string,e_SaveLogType eSaveLogType)
{
	emit signals_writeLogText(string, eSaveLogType);
}

int SysMainUI::ReadImageSignal(int camNO)
{
    auto trigCardID = m_sRealCamInfo[camNO].m_iIOCardSN;
    auto TrigPort = m_sRealCamInfo[camNO].m_iGrabPosition;
    switch(TrigPort-1) 
    {
    case 0:
        return pMainFrm->m_vIOCard[trigCardID]->ReadImageSignal(28);
        break;
    case 1:
        return pMainFrm->m_vIOCard[trigCardID]->ReadImageSignal(29);
        break;
    case 2:
        return pMainFrm->m_vIOCard[trigCardID]->ReadImageSignal(30);
        break;
    case 3:
        return pMainFrm->m_vIOCard[trigCardID]->ReadImageSignal(31);
        break;
    case 4:
        return pMainFrm->m_vIOCard[trigCardID]->ReadImageSignal(33);
        break;
    case 5:
        return pMainFrm->m_vIOCard[trigCardID]->ReadImageSignal(32);
        break;
    case 6:
        return pMainFrm->m_vIOCard[trigCardID]->ReadImageSignal(34);
        break;
    default:
        CLogFile::write(tr("NO Position%1").arg(TrigPort),AbnormityLog);
        return 0;
    }
    return 1;
}

void SysMainUI::InitCamImage(int iCameraNo)
{
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
	ReadCorveConfig();

	int i = iCameraNo;
	m_sCarvedCamInfo[i].m_iImageBitCount = m_sRealCamInfo[i].m_iImageBitCount;   //图像位数从相机处继承[8/7/2013 nanjc]
	m_sCarvedCamInfo[i].m_iImageRoAngle = m_sRealCamInfo[i].m_iImageRoAngle;
	m_sRunningInfo.m_cErrorTypeInfo[i].m_iErrorTypeCount = m_sErrorInfo.m_iErrorTypeCount;
	if (m_sCarvedCamInfo[i].m_pActiveImage!=NULL)
	{
		delete m_sCarvedCamInfo[i].m_pActiveImage; 
		m_sCarvedCamInfo[i].m_pActiveImage = NULL;
	}
	m_sCarvedCamInfo[i].m_pActiveImage=new QImage(m_sCarvedCamInfo[i].m_iImageWidth,m_sCarvedCamInfo[i].m_iImageHeight,m_sCarvedCamInfo[i].m_iImageBitCount);// 用于实时显示

	m_sCarvedCamInfo[i].m_pActiveImage->setColorTable(m_vcolorTable);
	BYTE* pByte = m_sCarvedCamInfo[i].m_pActiveImage->bits();
	int iLength = m_sCarvedCamInfo[i].m_pActiveImage->byteCount();
	memset((pByte),0,(iLength));
	
	nQueue[i].mDetectLocker.lock();
	nQueue[i].mGrabLocker.lock();
	nQueue[i].InitCarveQueue(m_sCarvedCamInfo[i].m_iImageWidth, m_sCarvedCamInfo[i].m_iImageHeight,m_sRealCamInfo[i].m_iImageWidth,m_sRealCamInfo[i].m_iImageHeight,m_sCarvedCamInfo[i].m_iImageBitCount, 10, true);
	nQueue[i].mGrabLocker.unlock();
	nQueue[i].mDetectLocker.unlock();
	//SetCarvedCamInfo();
}
bool SysMainUI::RoAngle(uchar* pRes,uchar* pTar,int iResWidth,int iResHeight,int iAngle)
{
	int iTarWidth;
	int iTarHeight;
	if(pRes == NULL || iResWidth == 0 || iResHeight == 0)
	{
		return FALSE;
	}
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
	if (iAngle == 270)
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
	if (iAngle == 180)
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

void SysMainUI::SetLanguage(int pLang)
{
	sLanguage = pLang;
}
//
//void SysMainUI::slot_ConnectedSock()
//{	
//	bool ret = SendDataToSever(0,CONNECT);
//	if(!ret)
//	{
//		CLogFile::write(QString("tcpsocket cnt Packet send Error!"),AbnormityLog);
//		m_tcpSocket->connectToHost(m_sSystemInfo.i_IP1,8088);
//		if(m_tcpSocket->waitForConnected(500))
//		{
//			connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(onSocketDataReady()));
//			CLogFile::write(QString("TcpSocket Cnt Succesed"),AbnormityLog);
//		}
//		else
//		{
//			CLogFile::write(QString("network failed!"),AbnormityLog);
//			m_tcpSocket->abort();
//		}
//	}
//
//	if (m_sSystemInfo.m_iDeviceOnInitial == 2)
//	{
//		for(int i=0;i<2;i++)
//		{
//			int timeLength = QTime::currentTime().second();
//			if(IPAddress[i].nstate)
//			{
//				if((timeLength-IPAddress[i].startTime+60)%60 > 25 )
//				{
//					CLogFile::write(QString("Overtime:%3 , IP:%1 , lastTime:%2 ").arg(IPAddress[i].ipAddress).arg(IPAddress[i].startTime).arg((timeLength-IPAddress[i].startTime+60)%60),AbnormityLog);
//					onServerConnected(IPAddress[i].ipAddress,false);
//					IPAddress[i].nstate = false;
//				}else{
//					onServerConnected(IPAddress[i].ipAddress,true);
//				}
//			}
//		}
//	}
//}
//
//void SysMainUI::slots_ShowPLCStatus(int iStatus)
//{
//    if (iLastStatus == iStatus)
//    {
//        return;
//    }
//    else
//    {
//        iLastStatus = iStatus;
//    }
//    if (iStatus<m_vstrPLCInfoType.size())
//    {
//        //报警框信息
//        if (0 >= iStatus)
//        {
//            emit signals_HideWarning(2);
//        }
//        else
//        {
//            emit signals_ShowWarning(2,m_vstrPLCInfoType.at(iStatus));
//        }
//
//		if(m_sSystemInfo.m_iDeviceOnInitial == 2 && m_tcpClient != NULL )
//		{
//			MyStruct nTempStruct;
//			nTempStruct.nState = PLCWARN;
//			nTempStruct.nPLCStatus = iStatus;
//			static char *m_PLCPacket=new char[DETA_LEN * sizeof(MyErrorType) + sizeof(MyStruct)];;
//			memset(m_PLCPacket,0,DETA_LEN * sizeof(MyErrorType) + sizeof(MyStruct));
//			memcpy(m_PLCPacket,&nTempStruct,sizeof(MyStruct));
//			m_tcpClient->write(m_PLCPacket,DETA_LEN * sizeof(MyErrorType) + sizeof(MyStruct));
//		}
//    }
//}

void SysMainUI::slots_UpdateRecordSet()
{
	QSettings SystemConfigSet(m_sConfigInfo.m_strConfigPath,QSettings::IniFormat);
	SystemConfigSet.setIniCodec(QTextCodec::codecForName("GBK"));
	m_sSystemInfo.iSaveRecordInterval=SystemConfigSet.value("system/iSaveRecordInterval",30).toInt();
	m_sSystemInfo.bSaveRecord = SystemConfigSet.value("system/isSaveRecord",true).toBool();
}

void SysMainUI::slots_UpdateShiftSet()
{
	QSettings SystemConfigSet(m_sConfigInfo.m_strConfigPath,QSettings::IniFormat);
	SystemConfigSet.setIniCodec(QTextCodec::codecForName("GBK"));
	m_sSystemInfo.shift1Time = QTime::fromString(SystemConfigSet.value("System/shift1Time","060000").toString(),"hhmmss");
	m_sSystemInfo.shift2Time = QTime::fromString(SystemConfigSet.value("System/shift2Time","150000").toString(),"hhmmss");
	m_sSystemInfo.shift3Time = QTime::fromString(SystemConfigSet.value("System/shift3Time","230000").toString(),"hhmmss");
	m_sSystemInfo.bAutoSetZero = SystemConfigSet.value("System/isAutoClear",true).toBool();
}

void SysMainUI::slots_SaveCountBytime()
{
	if(!m_sSystemInfo.bSaveRecord)
		return;
	static bool isSave=false;

	QTime time = QTime::currentTime();
	if(m_sSystemInfo.iSaveRecordInterval == 30)
	{
		if ( 30 == time.minute()||0 == time.minute() )
		{
			if (!isSave)
			{
				SaveToDatebase();
				SaveCountInfo();
				isSave =true;
			}
		}
		else
			isSave =false;
	}
	else
	{
		if (0 == time.minute())
		{
			if (!isSave)  
			{
				SaveToDatebase();
				SaveCountInfo();
				isSave =true;
			}
		}
		else
			isSave =false;
	}

}

void SysMainUI::slots_SaveCountByShift()
{
	if(!m_sSystemInfo.bAutoSetZero)
		return;
	QTime currentTm=QTime::currentTime();
	if(currentTm >m_sSystemInfo.shift1Time &&  currentTm < m_sSystemInfo.shift2Time)
	{
		if(currentShift != 0)
		{
			//qDebug()<<"Shift1";
			ClearCount();
			currentShift = 0;
		}
	}
	else if(currentTm >m_sSystemInfo.shift2Time &&  currentTm < m_sSystemInfo.shift3Time)
	{
		if(currentShift != 1)
		{
			//qDebug()<<"Shift2";
			ClearCount();
			currentShift = 1;
		}
	}
	else if(currentTm <m_sSystemInfo.shift1Time || currentTm > m_sSystemInfo.shift3Time)
	{
		if(currentShift != 2)
		{
			//qDebug()<<"Shift3";
			ClearCount();
			currentShift = 2;
		}
	}
}

#ifdef JIAMI_INITIA
void SysMainUI::MonitorLicense()
{
	QString  g_UidChar = "06a6914a-d863-43e1-800e-7e2eece22fd7";
	ver_code uucode;
	m_ProgramLicense.GetVerCode(&uucode);
	QString strCode = QString("%1-%2-%3-%4%5-%6%7%8%9%10%11")
		.arg(uucode.Data1,8,16,QChar('0')).arg(uucode.Data2,4,16,QChar('0')).arg(uucode.Data3,4,16,QChar('0'))
		.arg((int)uucode.Data4[0],2,16,QChar('0')).arg((int)uucode.Data4[1],2,16,QChar('0'))
		.arg((int)uucode.Data4[2],2,16,QChar('0')).arg((int)uucode.Data4[3],2,16,QChar('0'))
		.arg((int)uucode.Data4[4],2,16,QChar('0')).arg((int)uucode.Data4[5],2,16,QChar('0'))
		.arg((int)uucode.Data4[6],2,16,QChar('0')).arg((int)uucode.Data4[7],2,16,QChar('0'));
	if (g_UidChar == strCode)
	{
		//验证License
		s_KeyVerfResult res = m_ProgramLicense.CheckLicenseValid(true);
		if (res.nError <= 0)
		{
			//int m_nLicenseDays = res.nDays;
			int m_nLicenseDays = m_ProgramLicense.ReadHardwareID("getexpdate");
			if (m_nLicenseDays<=10 && m_nLicenseDays>0)
			{
				//弹出提示框
				showAllert();
			}
			//更新剩余时间
			surplusDays = m_nLicenseDays;
		}else{
			m_sSystemInfo.m_bIsTest = false;
			m_sRunningInfo.m_bCheck = true;
			slots_OnBtnStar();
			slots_OnExit(true);
		}
	}else{
		QMessageBox::information(this,tr("Error"),tr("Encryption verification failed, will exit the program!"));//加密验证失败,即将退出程序!
		m_sSystemInfo.m_bIsTest = false;
		m_sRunningInfo.m_bCheck = true;
		slots_OnBtnStar();
		slots_OnExit(true);
	}
}
void SysMainUI::showAllert()
{
	pushLicense* m_tempLicense=new pushLicense; 
	m_tempLicense->slots_ShowWarning(0,tr("The authorization to use the equipment is about to expire \n, please contact business personnel!")); //设备使用授权即将到期\n请联系商务人员！
	m_tempLicense->MaxNumber->start();
}
bool SysMainUI::CheckLicense()
{
	QString  g_UidChar = "06a6914a-d863-43e1-800e-7e2eece22fd7";
	ver_code uucode;
	m_ProgramLicense.GetVerCode(&uucode);
	QString strCode = QString("%1-%2-%3-%4%5-%6%7%8%9%10%11")
		.arg(uucode.Data1,8,16,QChar('0')).arg(uucode.Data2,4,16,QChar('0')).arg(uucode.Data3,4,16,QChar('0'))
		.arg((int)uucode.Data4[0],2,16,QChar('0')).arg((int)uucode.Data4[1],2,16,QChar('0'))
		.arg((int)uucode.Data4[2],2,16,QChar('0')).arg((int)uucode.Data4[3],2,16,QChar('0'))
		.arg((int)uucode.Data4[4],2,16,QChar('0')).arg((int)uucode.Data4[5],2,16,QChar('0'))
		.arg((int)uucode.Data4[6],2,16,QChar('0')).arg((int)uucode.Data4[7],2,16,QChar('0'));
	if (g_UidChar == strCode)
	{
		//验证License
		s_KeyVerfResult res = m_ProgramLicense.CheckLicenseValid(true);
		if (res.nError <= 0)//未超时
		{
			int nDogValue = (int)m_ProgramLicense.ReadDog();
			if (nDogValue == 0)
			{
				//读取加密狗参数异常 代码：22
				QMessageBox::information(this,tr("Error"),tr("License expired or dongle abnormal! Error code: 22"));//License过期或加密狗异常！错误代码：22
				return false;
			}
			int m_nLicenseDays = m_ProgramLicense.ReadHardwareID("getexpdate");
			//m_nLicenseDays = res.nDays;
			surplusDays = m_nLicenseDays;
			if (m_nLicenseDays<=10 && m_nLicenseDays>0)
			{
				showAllert();
			}
			//传递窗口句柄
			m_ProgramLicense.SetMainWnd((HWND)this->winId());
			return true; 
		}
		else
		{
			QMessageBox::information(this,tr("Error"),tr("License expired or dongle abnormal! Error code: %1").arg(res.nError)); //License过期或加密狗异常！错误代码：%1
			return false;
		}
	}
	else
	{
		QMessageBox::information(this,tr("Error"),tr("Encryption authentication failed!")); //加密验证失败
		return false;
	}
	return true;
}
#else
void SysMainUI::MonitorLicense()
{
}
bool SysMainUI::CheckLicense()
{
	return TRUE;
}
#endif // JIAMI_INITIA


