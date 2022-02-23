#include "CombineResult.h"
#include "clogFile.h"
#include "ILDRSystem.h"
extern SysMainUI* pMainFrm;
CCombineRlt::CCombineRlt()
{
	i_CameraCount = 0;
	m_iLatestImageNo = 0;
	m_iLatestErrorNo = 0;
	for (int i = 0;i<CAMERA_MAX_COUNT;i++)
	{
		b_CombinCamera[i] = false;
	}
	for (int i=0;i<256;i++)
	{
 		iErrorCamera[i] = 0;
 		iErrorType[i] = 0;
		iMoldNo[i]= 0;
	}
}
CCombineRlt::~CCombineRlt()
{
};

void CCombineRlt::Inital(int iGrabCountInQueen)
{
	i_CameraCount = iGrabCountInQueen;
}
void CCombineRlt::SetCombineCamera(int iCamera,bool bIfCombine)
{
	b_CombinCamera[iCamera] = bIfCombine;
	if (bIfCombine)
	{
		i_CameraCount++;
	}
	else
	{
		i_CameraCount--;
	}
}

int CCombineRlt::AddResult(int iSignalNum,int CameraNum,int tmpResult)
{
	/*if (iSignalNum<0)
	{
	return -1;
	}*/
	int iTempSignalNum;
	if (m_iLatestImageNo - iSignalNum>100)
	{
		iTempSignalNum = iSignalNum+256;
	}
	else
	{
		iTempSignalNum = iSignalNum;
	}

	if (0 < (iTempSignalNum-m_iLatestImageNo)&&(iTempSignalNum-m_iLatestImageNo)<100)
	{
		for (int i =1 ;i <= iTempSignalNum-m_iLatestImageNo;i++)
		{
			RemoveOneResult((i+m_iLatestImageNo)%256);
		}
		m_iLatestImageNo = iTempSignalNum%256;
	}

	m_Rlts[iSignalNum].b_Rlts[CameraNum] = tmpResult;
	m_Rlts[iSignalNum].b_Checked[CameraNum] = true;
	if (tmpResult == 1)
	{
		m_Rlts[iSignalNum].iResult = 1;
	}
	return 0;
}

bool CCombineRlt::ConbineResult(int iSignalNum,int CameraCount,int &bCombineRlt)
{
	bool bRight = true;
    QString tmp = QString("ConbineResult:ImgSN:%1,NoFinish:").arg(iSignalNum);
	for (int i = 0 ; i<CAMERA_MAX_COUNT;i++)
	{
		if (!m_Rlts[iSignalNum].b_Checked[i]&&b_CombinCamera[i])
		{
			bRight = false;
            tmp += QString("Camera%1,").arg(i);
		}
	}
	if (bRight)
	{
		bCombineRlt = m_Rlts[iSignalNum].iResult;
		return true;
	}
    //CLogFile::write(tmp, DebugLog, 0);
	return false;
}
//	删除检测结果队列中的结果结构
void CCombineRlt::RemovAllResult()
{
	for(int i = 0; i < 256; i++)
	{
		m_Rlts[i].iResult = 0;
		m_Rlts[i].bSentToIOCard = true;
		for(int j = 0; j<CAMERA_MAX_COUNT; j++)
		{
			if (b_CombinCamera[j])
			{
				m_Rlts[i].b_Rlts[j] = -1;
				m_Rlts[i].b_Checked[j] = false;
			}
		}
	}
}
// 	删除检测结果队列中的一个结果结构
void CCombineRlt::RemoveOneResult(int iSignalNum)
{
	int i = (iSignalNum+256)%256;
	m_Rlts[i].iResult = 0;
	for(int j = 0; j<CAMERA_MAX_COUNT; j++)
	{
		if (b_CombinCamera[j])
		{
			m_Rlts[i].b_Rlts[j] = -1;
			m_Rlts[i].b_Checked[j] = false;
		}
	}
}

int CCombineRlt::ErrorType(int iImgNo)
{
	return iErrorType[iImgNo];
}

int CCombineRlt::ErrorCamera(int iImgNo)
{
	return iErrorCamera[iImgNo];
}
bool CCombineRlt::IsReject(int iImgNo)
{
	return m_Rlts[iImgNo].bSentToIOCard;
}
void CCombineRlt::SetReject(int iImgNo, bool bReject)
{
	m_Rlts[iImgNo].bSentToIOCard =bReject;
}


void CCombineRlt::SetMoldNo(int iImgNo,int pMoldNo)
{
	iMoldNo[iImgNo] = pMoldNo;
}


int CCombineRlt::GetMoldNo(int iImgNo)
{
	return iMoldNo[iImgNo];
}


void CCombineRlt::RemoveMoldNo(int iImgNo)
{
	iMoldNo[iImgNo] = 0;
}

void CCombineRlt::AddError(int iSignalNum,int CameraNum,s_ErrorPara  tempsErrorPara)
{
	int iTempSignalNum = iSignalNum;
	if (iSignalNum < m_iLatestErrorNo && m_iLatestErrorNo - iSignalNum>100)
	{
		iTempSignalNum = iSignalNum+256;
	}
	if (0 < (iTempSignalNum-m_iLatestErrorNo))
	{
		for (int i =1 ;i <= iTempSignalNum-m_iLatestErrorNo;i++)
		{
			RemoveOneError((i+m_iLatestErrorNo)%256);
		}
		m_iLatestErrorNo = iTempSignalNum%256;
	}

	/*if (sErrorPara[iSignalNum].nArea>0	&&1 == tempsErrorPara.nErrorType)
	{
	return;
	}*/
	if ((sErrorPara[iSignalNum].nArea<tempsErrorPara.nArea)||(tempsErrorPara.nErrorType>2&&sErrorPara[iSignalNum].nErrorType<=2))
	{
		sErrorPara[iSignalNum] = tempsErrorPara;
		iErrorCamera[iSignalNum] = CameraNum;
	}
}

s_ErrorPara CCombineRlt::ConbineError(int iSignalNum)
{
	s_ErrorPara temErrorPara = sErrorPara[iSignalNum];
   	RemoveOneError(iSignalNum);
   	if (-1 != sErrorPara[iSignalNum-1].nArea)
   	{
   		RemoveOneError(iSignalNum-1);
   	}
	return temErrorPara;
}
void CCombineRlt::RemovAllError()
{
	for(int i = 0; i < 256; i++)
	{
		sErrorPara[i].rRctError.bottom = -1;
		sErrorPara[i].rRctError.left = -1;
		sErrorPara[i].rRctError.right = -1;
		sErrorPara[i].rRctError.top = -1;
		sErrorPara[i].nErrorType = -1;
		sErrorPara[i].nArea = -1;
		sErrorPara[i].nLevel = -1;
	}
}
// 	删除检测结果队列中的一个结果结构
// 	iQueenID      相机所属的队
void CCombineRlt::RemoveOneError(int iSignalNum)
{
	int i = (iSignalNum+256)%256;

	sErrorPara[i].rRctError.bottom = -1;
	sErrorPara[i].rRctError.left = -1;
	sErrorPara[i].rRctError.right = -1;
	sErrorPara[i].rRctError.top = -1;
	sErrorPara[i].nErrorType = -1;
	sErrorPara[i].nArea = -1;
	sErrorPara[i].nLevel = -1;

	iErrorCamera[i] = 0;
	iErrorType[i] = 0;
}
