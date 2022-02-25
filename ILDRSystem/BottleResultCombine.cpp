#include "BottleResultCombine.h"

#include "clogFile.h"

#include "ILDRSystem.h"
extern SysMainUI* pMainFrm;
CBottleRltCombine::CBottleRltCombine()
{
	i_CameraCount = 0;
	m_iLatestImageNo = 0;
	m_iLatestErrorNo = 0;
	for (int i = 0;i<CAMERA_MAX_COUNT;i++)
	{
		b_CombinCamera[i] = false;
	}
    for(int i = 0;i < 256;i++)
    {
        m_Rlts[i].result.imgNO = i;
    }
}
CBottleRltCombine::~CBottleRltCombine()
{
};

void CBottleRltCombine::Inital(int iGrabCountInQueen)
{
	i_CameraCount = iGrabCountInQueen;
}
void CBottleRltCombine::SetCombineCamera(int iCamera,bool bIfCombine)
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

int CBottleRltCombine::AddResult(int iSignalNum,int CameraNum, BottleResult tmpResult)
{
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
	if (tmpResult.idLine != "" && tmpResult.isHigher(m_Rlts[iSignalNum].result))
	{
		m_Rlts[iSignalNum].result = tmpResult;
	}
	return 0;
}

bool CBottleRltCombine::ConbineResult(int iSignalNum, BottleResult &bCombineRlt)const
{
	bool bRight = true;
    //QString tmp = QString("ConbineResult:ImgSN:%1,NoFinish:").arg(iSignalNum);
    for (int i = 0 ; i<CAMERA_MAX_COUNT;i++)
    {
        if (!m_Rlts[iSignalNum].b_Checked[i]&&b_CombinCamera[i])
        {
            bRight = false;
            //tmp += QString("Camera%1,").arg(i);
        }
    }
	if (bRight)
	{
		bCombineRlt = m_Rlts[iSignalNum].result;
		return true;
	}
    //CLogFile::write(tmp, DebugLog, 0);
    bCombineRlt.imgNO = iSignalNum;
	return false;
}

QList<int> CBottleRltCombine::successCameraList(const BottleResult& res) const
{
    return m_Rlts[res.imgNO].succeedCamList(res);
}

//	删除检测结果队列中的结果结构
void CBottleRltCombine::RemovAllResult()
{
	for(int i = 0; i < 256; i++)
	{
		m_Rlts[i].result.reset();
		for(int j = 0; j<CAMERA_MAX_COUNT; j++)
		{
			if (b_CombinCamera[j])
			{
				m_Rlts[i].b_Rlts[j].reset();
				m_Rlts[i].b_Checked[j] = false;
			}
		}
	}
}
// 	删除检测结果队列中的一个结果结构
void CBottleRltCombine::RemoveOneResult(int iSignalNum)
{
	int i = (iSignalNum+256)%256;
	m_Rlts[i].result.reset();
	for(int j = 0; j<CAMERA_MAX_COUNT; j++)
	{
		if (b_CombinCamera[j])
		{
			m_Rlts[i].b_Rlts[j].reset();
			m_Rlts[i].b_Checked[j] = false;
		}
	}
}
