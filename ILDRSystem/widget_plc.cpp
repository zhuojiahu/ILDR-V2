#include "widget_plc.h"

#include <QLayout>
#include <QGroupBox>
#include <QSettings>
#include <QTextCodec>
#include <QMouseEvent>
#include <QPainter>

Widget_PLC::Widget_PLC(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowIcon(QIcon("./Resources/LOGO.png"));
	setWindowTitle(QString::fromLocal8Bit("PLC设置"));
	connect(ui.PlcReturn,SIGNAL(clicked()),this,SLOT(slots_return()));
	connect(ui.SureButton,SIGNAL(clicked()),this,SLOT(slots_Pushbuttonsure()));
	connect(ui.pushButton_save,SIGNAL(clicked()),this,SLOT(slots_Pushbuttonsave()));
	connect(ui.pushButton_read,SIGNAL(clicked()),this,SLOT(slots_Pushbuttonread()));
	
	m_pSocket = new QUdpSocket();
	m_pSocket->connectToHost("192.168.250.1", 9600);
	if (m_pSocket->state() == QAbstractSocket::ConnectedState || m_pSocket->waitForConnected(2000))
	{
		connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(slots_readFromPLC()));
	}
	QIntValidator* IntValidator = new QIntValidator;
	IntValidator->setRange(1, 60);
	ui.lineEdit_2->setValidator(IntValidator);
	ui.lineEdit_3->setValidator(IntValidator);
	ui.lineEdit_4->setValidator(IntValidator);
	ui.lineEdit_5->setValidator(IntValidator);
	ui.lineEdit_6->setValidator(IntValidator);
	IntValidator->setRange(1,450);
	ui.lineEdit_1->setValidator(IntValidator);

	m_zTimer = new QTimer(this);
	connect(m_zTimer,SIGNAL(timeout()),this,SLOT(slots_TimeOut()));
	m_zTimer->start(1000);
	//获取PLC报警信息
	nErrorType = 0;
	for(int i=0;i<30;i++)
	{
		nHandle[i] = CreateEvent(NULL,FALSE,NULL,NULL);
		nImageNum[i] = 0;
	}
}

Widget_PLC::~Widget_PLC()
{
	delete m_pSocket;
}
void Widget_PLC::slots_Pushbuttonread()
{
	QByteArray st;
	SendMessage(20,st,1,1,44);
}
void Widget_PLC::slots_TimeOut()
{
	//获取PLC的报警信息
	QByteArray st;
	SendMessage(0,st,1,1,10);//读取报警数据
}
void Widget_PLC::slots_return()
{
	hide();
}
void Widget_PLC::SendDataToPLCHead(int address, QByteArray& st, int state,int id,int DataSize) //参数1为相机ID号，参数2为组装后的数据，参数3为读写状态,参数4为通道ID(可以为任意整数),参数5为数据大小
{
	QByteArray v_szTmp;
	v_szTmp.append(QChar(0x80).toLatin1());//ICF  display frame information
	v_szTmp.append(QChar(0x00).toLatin1());//RSV  reserved by system
	v_szTmp.append(QChar(0x02).toLatin1());//GCT  permissible number of gateways
	v_szTmp.append(QChar(0x00).toLatin1());//DNA  destination network address
	v_szTmp.append(QChar(0x01).toLatin1());//DA1  destination node address
	v_szTmp.append(QChar(0x00).toLatin1());//DA2  destination unit address
	v_szTmp.append(QChar(0x00).toLatin1());//SNA  source network address
	v_szTmp.append(QChar(0x02).toLatin1());//SA1  source node address
	v_szTmp.append(QChar(0x00).toLatin1());//SA2  source unit address
	v_szTmp.append(id);//SID  service id   预先计划赋值sendId
	v_szTmp.append(QChar(0x01).toLatin1());//MRC  main request code
	v_szTmp.append(state);//SRC  sub request code
	v_szTmp.append(QChar(0xB2).toLatin1());//H区都是B2,D区是82
	v_szTmp.append(address/256); //内存地址
	v_szTmp.append(address%256); //内存地址
	v_szTmp.append(QChar(0x00).toLatin1());
	v_szTmp.append(DataSize/2/256);//数据长度
	v_szTmp.append(DataSize/2%256);//数据长度
	v_szTmp.append(st);
	st = v_szTmp;
}

int Widget_PLC::SendMessage(int address,QByteArray& send,int state,int id,int DataSize) //异步发送数据改变PLC参数
{
	SendDataToPLCHead(address,send,state,id,DataSize);
	//qDebug()<<send.toHex();
	if (m_pSocket->state() == QAbstractSocket::ConnectedState)
	{
		if (NULL != m_pSocket)
		{
			m_pSocket->write(send);
		}
	}
	return 0;
}
int Widget_PLC::GetImageNo(int nAddr,int CameraId,int& ImageNo)
{
	QByteArray send;
	SendDataToPLCHead(nAddr,send,1,CameraId,4);
	if (m_pSocket->state() == QAbstractSocket::ConnectedState) //
	{
		if (NULL != m_pSocket)
		{
			m_pSocket->write(send);
		}
	}
	WaitForSingleObject(nHandle[CameraId],2000);
	return nImageNum[CameraId];
}
void Widget_PLC::slots_readFromPLC()
{
	QByteArray v_receive = m_pSocket->readAll();
	if(v_receive.size() == 18)
	{
		int v_Itmp=0;
		ByteToData(v_receive,14,17,v_Itmp);
		WORD imgNO = 0;
		ByteToData(v_receive,8,9,imgNO);
		nImageNum[imgNO] = v_Itmp;
		SetEvent(nHandle[imgNO]);

	}else if (v_receive.size() == 124)//14+6+36+64
	{
		double v_douTemp = 0;
		int v_Itmp = 0;
		int v_bit = 14;

		WORD m_Itmp=0;
		ByteToData(v_receive,v_bit,v_bit+1,m_Itmp);  //H97
		if(m_Itmp >> 0 & 0x01)
		{
			ui.radioButton_3->setChecked(true);
			ui.radioButton_4->setChecked(false);
		}else{
			ui.radioButton_3->setChecked(false);
			ui.radioButton_4->setChecked(true);
		}
		if(m_Itmp >> 1 & 0x01)
		{
			ui.radioButton_5->setChecked(true);
			ui.radioButton_6->setChecked(false);
		}else{
			ui.radioButton_5->setChecked(false);
			ui.radioButton_6->setChecked(true);
		}
		if(m_Itmp >> 2 & 0x01)
		{
			ui.radioButton_7->setChecked(true);
			ui.radioButton_8->setChecked(false);
		}else{
			ui.radioButton_7->setChecked(false);
			ui.radioButton_8->setChecked(true);
		}
		v_bit+=2;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp); //H98
		ui.lineEdit_21->setText(QString::number(v_Itmp));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_1->setText(QString::number(v_Itmp));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		if(v_Itmp == 0)
		{
			ui.radioButton_1->setChecked(true);
			ui.radioButton_2->setChecked(false);
		}else{
			ui.radioButton_1->setChecked(false);
			ui.radioButton_2->setChecked(true);
		}
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_2->setText(QString::number(v_Itmp/100));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_3->setText(QString::number(v_Itmp/100));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_4->setText(QString::number(v_Itmp/100));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_5->setText(QString::number(v_Itmp/100));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		ui.lineEdit_6->setText(QString::number(v_Itmp/100));
		v_bit+=4;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_7->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_9->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_10->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_11->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_12->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_14->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_15->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_17->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_19->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+3,v_Itmp);
		if(v_Itmp == 1)
		{
			ui.radioButton_9->setChecked(true);
			ui.radioButton_10->setChecked(false);
		}else{
			ui.radioButton_9->setChecked(false);
			ui.radioButton_10->setChecked(true);
		}
	}else if(v_receive.size() == 24)
	{
		WORD v_Itmp=0;
		int j=0;
		int m_byte=14;
		bool Asert = true;
		for (;m_byte<18;m_byte+=2)
		{
			ByteToData(v_receive,m_byte,m_byte+1,v_Itmp);
			for(int i=0;i<16;i++)
			{
				if(v_Itmp >> i & 0x01)
				{
					nErrorType = j;
					Asert = false;
				}
				j++;
			}
		}
		if(Asert)
		{
			nErrorType = -1;
		}
		m_byte+=4;
		ByteToData(v_receive,m_byte,m_byte+1,v_Itmp);
		
		if(v_Itmp >> 0 & 0x01)
		{
			emit signals_ResetCard();
		}
	}else if(v_receive.size() == 58)//14+44
	{
		double v_douTemp;
		int v_bit=14;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_8->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_13->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_16->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_18->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		ByteToData(v_receive,v_bit,v_bit+7,v_douTemp);
		ui.lineEdit_20->setText(QString::number(v_douTemp,'f',2));
		v_bit+=8;
		int v_Temp = 0;
		ByteToData(v_receive,v_bit,v_bit+3,v_Temp);
		if(v_Temp)
		{
			ui.radioButton_9->setChecked(true);
			ui.radioButton_10->setChecked(false);
		}else{
			ui.radioButton_9->setChecked(false);
			ui.radioButton_10->setChecked(true);
		}
	}
}

void Widget_PLC::slots_Pushbuttonsure()
{
	QByteArray st;
	WORD TempData = 3;
	DataToByte(TempData,st);
	SendMessage(90,st,2,1,2);
}

void Widget_PLC::slots_Pushbuttonsave()
{
	QByteArray st;
	int TempData = 0;
	WORD test = 0;
	if(ui.radioButton_3->isChecked())
	{
		test = 1;
	}else{
		test = 0;
	}
	if(ui.radioButton_5->isChecked())
	{
		test += 2;
	}else{
		test += 0;
	}
	if(ui.radioButton_7->isChecked())
	{
		test += 4;
	}else{
		test += 0;
	}
	DataToByte(test,st);
	TempData = ui.lineEdit_21->text().toInt();
	DataToByte(TempData,st);
	TempData = ui.lineEdit_1->text().toInt();
	DataToByte(TempData,st);
	if(ui.radioButton_1->isChecked())
	{
		TempData = 0;
	}else{
		TempData = 1;
	}
	DataToByte(TempData,st);
	TempData = ui.lineEdit_2->text().toInt()*100;
	DataToByte(TempData,st);
	TempData = ui.lineEdit_3->text().toInt()*100;
	DataToByte(TempData,st);
	TempData = ui.lineEdit_4->text().toInt()*100;
	DataToByte(TempData,st);
	TempData = ui.lineEdit_5->text().toInt()*100;
	DataToByte(TempData,st);
	TempData = ui.lineEdit_6->text().toInt()*100;
	DataToByte(TempData,st);
	double TempSpeed = ui.lineEdit_7->text().toDouble();
	DataToByte(TempSpeed,st);

	TempSpeed = ui.lineEdit_9->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_10->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_11->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_12->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_14->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_15->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_17->text().toDouble();
	DataToByte(TempSpeed,st);
	TempSpeed = ui.lineEdit_19->text().toDouble();
	DataToByte(TempSpeed,st);
	if(ui.radioButton_9->isChecked())
	{
		TempData = 1;
	}else{
		TempData = 0;
	}
	DataToByte(TempData,st);
	SendMessage(97,st,2,1,110);
}
template<typename T>
void Widget_PLC::DataToByte(T& xx, QByteArray& st)
{
	char nChar = 0;
	char* f_pshort = reinterpret_cast<char*>(&xx);
	for (int i = 0; i < sizeof(T); ++i)
	{
		if (i % 2 == 0)
		{
			nChar = (char)(*(f_pshort + i + 1));
		}
		else
		{
			nChar = (char)(*(f_pshort + i - 1));
		}
		st.append(nChar);
	}
}
template<typename T>
void Widget_PLC::ByteToData(QByteArray& st, int nStart, int nEnd,T& xt)
{
	xt = 0;
	char* f_pshort = reinterpret_cast<char*>(&xt);
	for (int i = nStart, j = 0; i <= nEnd; ++i, ++j)
	{
		if (i % 2 == 0)
		{
			*(f_pshort + j) = st[i + 1];
		}
		else
		{
			*(f_pshort + j) = st[i - 1];
		}
	}
}