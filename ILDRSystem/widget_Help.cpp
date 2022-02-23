#include "widget_Help.h"
#include <QFileInfoList>
#include <QDir>
#include <QGridLayout>
#include <QMessageBox>
#include <QSignalMapper>
#include <QPushButton>
#include <QProcess>
#include <Windows.h>

widget_Help::widget_Help(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	init();
}

widget_Help::~widget_Help()
{

}
void widget_Help::init()
{
	QString path = QApplication::applicationFilePath();  
	strHelpPath = path.left(path.findRev("/")+1)+"help/";

	QDir dir;
	if (!dir.exists(strHelpPath))
		dir.mkpath(strHelpPath);
	dir.setPath(strHelpPath);
	QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot);
	//
	QSignalMapper *signal_mapper = new QSignalMapper(this);//信号管理
	QGridLayout *button_layout = new QGridLayout();//布局管理器
	for (int i=0;i<fileList.size();++i)
	{
		QFileInfo fileInfo = fileList.at(i);
		listFileName<<fileInfo.completeBaseName()+"."+fileInfo.completeSuffix(); 

		QPushButton *push_button = new QPushButton(this);
		QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(push_button->sizePolicy().hasHeightForWidth());
		push_button->setSizePolicy(sizePolicy);
		push_button->setMinimumSize(150,150);
		push_button->setMaximumSize(400,400);
		push_button->setText(fileInfo.completeBaseName()+"."+fileInfo.completeSuffix());
		listPushbuttonOpenFile.append(push_button);
		connect(push_button, SIGNAL(clicked()), signal_mapper, SLOT(map()));
		signal_mapper->setMapping(push_button, i);
		button_layout->addWidget(push_button, i/8,i%8, Qt::AlignCenter);
	}
	connect(signal_mapper, SIGNAL(mapped(int)), this, SLOT(slot_OpenFile(int)));
	setLayout(button_layout);
}


void widget_Help::slot_OpenFile(int nFileNumber)
{
	QProcess *process = new QProcess(this);
	QString strPath = strHelpPath+listFileName.at(nFileNumber);
	QByteArray ba = strPath.toLocal8Bit().data();
	char* str = ba.data();
	ShellExecuteA(NULL,"open",str,NULL,NULL,SW_SHOW);
// 	bool res =  process->startDetached(strPath,QStringList());
// 	if (!res)
// 	{
// 		QMessageBox::information(this,"Error","Fail to open this File");
// 	}
}