#include "qconsole.h"
#include <qDebug>
#include <windows.h>
#include <QSignalMapper>
#include "clogfile.h"
#include "ILDRSystem.h"
extern SysMainUI *pMainFrm;
QConsole::QConsole(int SystemType,QWidget *parent)
	: QMainWindow(parent)
{
    ui.setupUi(this);
    m_vIOCard = nullptr;
	setWindowIcon(QIcon("Resources/LOGO.png"));
	setWindowTitle(QString::fromLocal8Bit("天恒一体机统计系统5.64.1.0"));
	//setWindowIcon(QIcon("./Resources/LOGO.png"));
	connect(ui.pushButton_save,SIGNAL(clicked()),this,SLOT(slot_SaveCard()));
	connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(slot_OpenCard()));
	connect(ui.pushButton_PLC,SIGNAL(clicked()),this,SLOT(slot_OpenPLC()));
	s_ConfigIOCardInfo m_sSystemInfo;
	m_sSystemInfo.iCardID = 1;
	m_sSystemInfo.strCardInitFile = QString("./PIO24B_reg_init1.txt");
	m_sSystemInfo.strCardName = QString("PIO24B");
	nType = SystemType;
	if(nType == 2 && pMainFrm->m_sSystemInfo.m_bIsIOCardOK)
	{
		int temp = 0 ;
		m_vIOCard = new CIOCard(m_sSystemInfo,1);
		s_IOCardErrorInfo sIOCardErrorInfo = m_vIOCard->InitIOCard();
		if (!sIOCardErrorInfo.bResult)
		{
			pMainFrm->m_sSystemInfo.m_bIsIOCardOK = false;
			CLogFile::write(tr("Error in init IOCard"),CheckLog);
		}else{
			CLogFile::write("succeed!!!!",CheckLog);
		}
		Sleep(200);
		//m_vIOCard->enable(true);
		temp = m_vIOCard->readParam(45);
		ui.lineEdit_1->setText(QString::number(temp));
		temp = m_vIOCard->readParam(32);
		ui.lineEdit_2->setText(QString::number(temp));
		m_vIOCard->writeParam(114,256);

		nReadIOcard = new QTimer(this);
		nReadIOcard->setInterval(1000);
		connect(nReadIOcard,SIGNAL(timeout()),this,SLOT(slot_readIoCard()));
		nReadIOcard->start();
	}
	m_plc = new Widget_PLC(parent);
	connect(m_plc,SIGNAL(signals_ResetCard()),this,SLOT(slot_ResetIoCard()));
}

QConsole::~QConsole()
{
	delete m_plc;
	if(nType== 2)
	{
		m_vIOCard->CloseIOCard();
	}
}
void QConsole::slot_readIoCard()
{
	if (pMainFrm->m_sRunningInfo.m_bCheck && pMainFrm->m_sSystemInfo.m_bIsIOCardOK)
	{
		/*int nCheckNum = m_vIOCard->ReadCounter(3);
		int nFailNum = m_vIOCard->ReadCounter(4);
		if((nCheckNum - nInfo.m_passNum>0)&&(nCheckNum - nInfo.m_passNum<50))
		{
			nInfo.m_checkedNum += nCheckNum - nInfo.m_passNum;
		}
		nInfo.m_passNum = nCheckNum;
		if((nFailNum - nInfo.m_failureNum>0)&&(nFailNum - nInfo.m_failureNum<50))
		{
			nInfo.m_checkedNum2 += nFailNum - nInfo.m_failureNum;
		}
		nInfo.m_failureNum = nFailNum;*/
	}
}
void QConsole::closeEvent(QCloseEvent *event)
{
	emit signal_HideWidget();
}
void QConsole::slot_ResetIoCard()
{
	if(nType == 2)
	{
		//m_vIOCard->m_Pio24b.softReset();
	}
}
void QConsole::slot_SaveCard()
{
	if(nType== 2)
	{
		QString strValue,strPara;
		int temp = ui.lineEdit_1->text().toInt();
		strValue = strValue.setNum(temp,10);
		strPara = strPara.setNum(45,10);
		WritePrivateProfileQString("PIO24B",strPara,strValue,"./PIO24B_reg_init1.txt");
		m_vIOCard->writeParam(45,temp);

		temp = ui.lineEdit_2->text().toInt();
		strValue = strValue.setNum(temp,10);
		strPara = strPara.setNum(32,10);
		WritePrivateProfileQString("PIO24B",strPara,strValue,"./PIO24B_reg_init1.txt");
		m_vIOCard->writeParam(32,temp);
	}
}
void QConsole::slot_OpenPLC()
{
	QByteArray st;
	m_plc->SendMessage(97,st,1,2,110);//暂时获取界面显示的所有数据7*4+8+8*8
	m_plc->show();
}
void QConsole::slot_OpenCard()
{
	if(nType== 2 && m_vIOCard != nullptr)
	{
		m_vIOCard->Show_PIO24B_DebugDialog(this);
	}
}
bool QConsole::WritePrivateProfileQString(QString strSectionName, QString strKeyName, QString strValue, QString strFileName)
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
