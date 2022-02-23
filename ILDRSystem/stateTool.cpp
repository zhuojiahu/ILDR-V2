#include "stateTool.h"

#include <QTextStream>
#include <QSettings>
#include <QFontMetrics>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QDir>

//加载支持相关相机库
#pragma comment(lib,"DHGrabberForSG.lib")
#pragma comment(lib,"DHGrabberForMER.lib")
#pragma comment(lib,"DHGrabberForAVT.lib")
#pragma comment(lib,"RotEmptyBottle.lib")

bool StateTool::writeInit(QString path, QString user_key, QString user_value)
{
	if(path.isEmpty() || user_key.isEmpty())
	{
		return false;
	}
	else
	{
		//创建配置文件操作对象
		QSettings *config = new QSettings(path, QSettings::IniFormat);

		//将信息写入配置文件
		config->beginGroup("config");
		config->setValue(user_key, user_value);
		config->endGroup();

		return true;
	} 
}

bool StateTool::readInit(QString path, QString user_key, QString &user_value)
{
	user_value = QString("");
	if(path.isEmpty() || user_key.isEmpty())
	{
		return false;
	}
	else
	{
		//创建配置文件操作对象
		QSettings *config = new QSettings(path, QSettings::IniFormat);

		//读取用户配置信息
		user_value = config->value(QString("config/") + user_key).toString();

		return true;
	}  
}

bool StateTool::updateText(QString text, int max_width, QString &elided_text)
{
	elided_text = QString("");
	if(text.isEmpty() || max_width <= 0)
	{
		return false;
	}

	QFont ft;
	QFontMetrics fm(ft);
	elided_text = fm.elidedText(text, Qt::ElideRight, max_width);

	return true;
}

QString StateTool::getSkinName()
{
	QString skin_name = DEFAULT_SKIN;
//	bool is_read = StateTool::readInit(QString("./user.ini"), QString("skin"), skin_name);

	return skin_name;
}

bool StateTool::WritePrivateProfileQString(QString strSectionName, QString strKeyName, QString strValue, QString strFileName)
{
	bool ret = false;

	//section是null则直接返回false
	if(strSectionName.isEmpty())
	{
		return ret;
	}

	bool flagFindSection = false;//是否找到了section
	int pos = 0;
	QString strSection("[" + strSectionName + "]");
	QString strKey(strKeyName + "  =");
	QString strKey1(strKeyName + "=");
	QString strKey2(strKeyName + " =");

	//文件不存在，则创建，且直接写入
	if(!QFile::exists(strFileName))
	{
		QFile createFile(strFileName);
		if(!createFile.open(QFile::WriteOnly | QIODevice::Text))
		{
			return ret;
		}
		//如果key和value不是null，才写内容
		if(!strKeyName.isEmpty() && !strValue.isEmpty())
		{
			QTextStream createStream(&createFile);
			createStream<<strSection<<"\n";
			createStream<<strKey<<strValue<<"\n";
			createStream.flush();

			ret = true;
		}
		createFile.close();
		return ret;
	}

	QFile readFile(strFileName);

	if (!readFile.open(QFile::ReadOnly | QIODevice::Text))
	{
		ret = false;
		return ret;
	}

	QFile writeFile(strFileName);

	//读入流和写入流
	//写入流文件在最后才WriteOnly打开
	QTextStream readStream(&readFile);
	QTextStream writeStream(&writeFile);

	//查找每一行是否包含section
	while (!readStream.atEnd())
	{
		QString line(readStream.readLine());

		if (line.indexOf(strSection) != 0) //该行不包含section，直接写入流
		{
			writeStream<<line<<"\n";
		}
		else  
		{
			flagFindSection = true; //查到section
			ret = true;

			//key是null，则跳过该section,写入其余section
			if (strKeyName.isEmpty())
			{
				do //跳过该section
				{
					line = QString(readStream.readLine());
				} while (line.indexOf("[") != 0 && !readStream.atEnd());

				if(readStream.atEnd())
				{
					break;
				}
				else //写入其余section
				{
					writeStream<<line<<"\n";
					while(!readStream.atEnd())
					{
						writeStream<<readStream.readLine()<<"\n";
					}
					break;
				}
			}

			writeStream<<line<<"\n";//section写入流
			line = QString(readStream.readLine());
			while(line.indexOf(strKey) != 0 && line.indexOf(strKey1) != 0 && line.indexOf(strKey2) != 0 && line.indexOf("[") != 0 && !readStream.atEnd())
			{
				writeStream<<line<<"\n";
				line = QString(readStream.readLine());
			}

			if(line.indexOf(strKeyName) == 0)//查到key，若value!=null则修改value
			{
				if(!strValue.isEmpty())
				{
					line = line.mid(0, line.indexOf("=") + 1) + strValue;
					writeStream<<line<<"\n";
				}
				while(!readStream.atEnd()) //剩余行写入流中
				{
					writeStream<<readStream.readLine()<<"\n";
				}
				break;
			}
			else if(line.indexOf("[") == 0)//查到下一个section，若value!=null则在下一个section前直接加上key
			{
				if(!strValue.isEmpty())
				{
					writeStream<<strKey<<strValue<<"\n";
				}
				writeStream<<line<<"\n";
				while(!readStream.atEnd()) //剩余行写入流中
				{
					writeStream<<readStream.readLine()<<"\n";
				}
				break;
			}
			else if(readStream.atEnd())//文件末尾，若value!=null则直接加上key
			{
				//直接加上
				if(!strValue.isEmpty())
				{

					writeStream<<line<<"\n"<<strKey<<strValue<<"\n";
				}
			}

		}
	}
	if(!flagFindSection)//若未查到该section，且key和value！=null，写入section和key=value
	{
		if(!strKeyName.isEmpty() && !strValue.isEmpty())
		{
			writeStream<<strSection<<"\n";
			writeStream<<strKey<<strValue<<"\n";
		}
	}
	readFile.close();

	//写打开文件
	if (!writeFile.open(QFile::WriteOnly | QIODevice::Text))
	{
		ret = false;
		return ret;
	}
	writeStream.flush();//写入流到文件

	writeFile.close();

	return ret;
}

void cErrorTypeInfo::operator=(const cErrorTypeInfo cError)
{
    m_iErrorTypeCount = cError.m_iErrorTypeCount;
    for (int i=0; i<ERRORTYPE_MAX_COUNT; i++)
    {
        iErrorCountByType[i] = cError.iErrorCountByType[i];
    }
}

cErrorTypeInfo::cErrorTypeInfo()
{
    m_iErrorTypeCount=0;
    for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
    {
        iErrorCountByType[i]=0;
    }
}

cErrorTypeInfo::~cErrorTypeInfo()
{

}

void cErrorTypeInfo::Clear()
{
    //		m_iErrorTypeCount=0;
    for (int i=0;i<ERRORTYPE_MAX_COUNT;i++)
    {
        iErrorCountByType[i]=0;
    }
}

BOOL cErrorTypeInfo::ErrorTypeJudge(int &iErrorType)
{
    if (0<iErrorType&&iErrorType<(m_iErrorTypeCount+1))
    {
        return TRUE;
    }
    else
        return FALSE;
}

const CMachineSignal& CMachineSignal::operator=(const CMachineSignal &MachineSignal)
{
    m_iSensorCount = MachineSignal.m_iSensorCount;
    m_iImageCount  = MachineSignal.m_iImageCount;
    m_iKickCount   = MachineSignal.m_iKickCount;
    return *this;
}

CMachineSignal::CMachineSignal(const CMachineSignal &MachineSignal)
{
    m_iSensorCount = MachineSignal.m_iSensorCount;
    m_iImageCount  = MachineSignal.m_iImageCount;
    m_iKickCount   = MachineSignal.m_iKickCount;
}

CMachineSignal::CMachineSignal()
{
    m_iSensorCount = 0;       
    m_iImageCount  = 0;
    m_iKickCount = 0;       // 踢废计数
}

CMachineSignal::~CMachineSignal()
{

}

CGrabElement::CGrabElement()
{
    nCamSN = -1;
    dCostTime = 0;
    bHaveImage = FALSE;
    nCheckRet = 0;
    nSignalNo = 0;
    myImage = NULL;
    initID = 0;
    nWidth = 0;
    nHeight = 0;
    SourceImage = NULL;
}

CGrabElement::~CGrabElement()
{
    //释放算法返回错误信息链表
    if (!cErrorParaList.isEmpty())
    {
        cErrorParaList.clear();
    }
}

CDetectElement::CDetectElement()
{
    iType = 0;
    bIsImageNormalCompelet = false;
    bIsImageStressCompelet = false;
    ImageNormal = NULL;
    ImageStress = NULL;
    iCameraNormal = -1;
    iCameraStress = -1;
    iSignalNoNormal = -1;
    iSignalNoStress = -1;
}

CDetectElement::~CDetectElement()
{
    // 		delete ImageNormal;
    // 		delete ImageStress;
}

void CDetectElement::AddNormalImage(CGrabElement* sImageNormal)
{
    Q_ASSERT(sImageNormal->myImage->width()>0 );
    Q_ASSERT(sImageNormal->nSignalNo>=0 );
    Q_ASSERT(sImageNormal->nSignalNo<256 );

    ImageNormal = sImageNormal;
    iCameraNormal = sImageNormal->nCamSN;
    iSignalNoNormal = sImageNormal->nSignalNo;
    bIsImageNormalCompelet = true;
}

void CDetectElement::AddStressImage(CGrabElement* sImageStress)
{
    Q_ASSERT(sImageStress->myImage->width()>0 );
    Q_ASSERT(sImageStress->nSignalNo>=0 );
    Q_ASSERT(sImageStress->nSignalNo<256 );

    ImageStress = sImageStress;
    iCameraStress = sImageStress->nCamSN;
    iSignalNoStress = sImageStress->nSignalNo;
    bIsImageStressCompelet = true;
}

bool CDetectElement::IsImageNormalCompelet()
{
    return bIsImageNormalCompelet;
}

bool CDetectElement::IsImageStressCompelet()
{
    return bIsImageStressCompelet;
}

void CDetectElement::setType(int type)
{
    iType = type;
}

int CDetectElement::SignalNoNormal()
{
    return iSignalNoNormal;
}

int CDetectElement::SignalNoStress()
{
    return iSignalNoStress;
}

void CDetectElement::copyDatato(CDetectElement &cDes)
{
    Q_ASSERT(ImageNormal->myImage->width()>0 );
    Q_ASSERT(ImageStress->myImage->width()>0 );
    Q_ASSERT(ImageNormal->nSignalNo>=0 );
    Q_ASSERT(ImageStress->nSignalNo>=0 );
    Q_ASSERT(ImageNormal->nSignalNo<256 );
    Q_ASSERT(ImageStress->nSignalNo<256 );

    cDes.ImageNormal = ImageNormal;
    cDes.ImageStress = ImageStress;
    cDes.iType = iType;
    cDes.iCameraNormal = iCameraNormal;
    cDes.iCameraStress = iCameraStress;
    cDes.iSignalNoNormal = iSignalNoNormal;
    cDes.iSignalNoStress = iSignalNoStress;
    cDes.bIsImageNormalCompelet = bIsImageNormalCompelet;
    cDes.bIsImageStressCompelet = bIsImageStressCompelet;
    clear();
}

void CDetectElement::clear()
{
    iType = 0;
    bIsImageNormalCompelet = false;
    bIsImageStressCompelet = false;
    ImageNormal = NULL;
    ImageStress = NULL;
    iCameraNormal = -1;
    iCameraStress = -1;
    iSignalNoNormal = -1;
    iSignalNoStress = -1;
}

GrabberEvent::GrabberEvent()
{
    for (int i = 0;i<CAMERA_MAX_COUNT;i++)
    {
        iTimpCheck[i] = 0;
    }
}

void GrabberEvent::postMessage(int iCamera)
{
    mutexM.lock();
    iTimpCheck[iCamera]++;
    waitConditionM.wakeOne();
    mutexM.unlock();
}

int GrabberEvent::waitMessage(unsigned long time /*= ULONG_MAX*/)
{
    int bRtn = 0;
    mutexM.lock();
    for (int iCamera = 0;iCamera<CAMERA_MAX_COUNT;iCamera++)
    {
        if (iTimpCheck[iCamera]>0)
        { 
            iTimpCheck[iCamera]--;
            bRtn = true;
            mutexM.unlock();
            return iCamera;
        }
    }
    bRtn = waitConditionM.wait(&mutexM,time);
    for (int iCamera = 0;iCamera<CAMERA_MAX_COUNT;iCamera++)
    {
        if (iTimpCheck[iCamera]>0)
        {
            iTimpCheck[iCamera]--;
            bRtn = true;
            mutexM.unlock();
            return iCamera;
        }
    }
    mutexM.unlock();
    return -1;
}

CSpendTime::~CSpendTime()
{

}

CSpendTime::CSpendTime()
{
    Qpart1 = 0;
    Qpart2 = 0;
    Useingtime = 0;
    dfMinus = 0;
    dfFreq = 0;
    dfTime = 0;
    Useingtime_us = 0;
}

void CSpendTime::StartSpeedTest()
{
    //获得CPU计时器的时钟频率
    QueryPerformanceFrequency(&litmp);//取得高精度运行计数器的频率f,单位是每秒多少次（n/s）,
    dfFreq = (double)litmp.QuadPart;

    QueryPerformanceCounter(&litmp);//取得高精度运行计数器的数值
    Qpart1 = 0;
    Qpart1 = litmp.QuadPart; //开始计时
}

void CSpendTime::StopSpeedTest()
{
    QueryPerformanceCounter(&litmp);//取得高精度运行计数器的数值
    Qpart2 = litmp.QuadPart; //终止计时

    dfMinus = (double)(Qpart2 - Qpart1);//计算计数器值
    dfTime = (dfMinus / dfFreq)*1000;//获得对应时间,单位为ms,可以乘1000000精确到微秒级（us）
    Useingtime = dfTime*1000;
    Useingtime_us = dfTime*1000000;
}
