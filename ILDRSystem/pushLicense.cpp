#include "pushLicense.h"

#include <windows.h>

#include <QDesktopWidget>
#include <QApplication>
#include <QLayout>
#include <QTimer>

#include "ILDRSystem.h"
extern SysMainUI *pMainFrm;
pushLicense::pushLicense(QWidget *parent)
	: QWidget(parent)
{
	setStyleSheet(QString::fromUtf8("background-color: rgb(138,171,140);"));

	bIsShow = false;
	iWarningType = 1;
	/*btnClose = new PushButton(this);
	btnClose->setPicName(QString(":/sysButton/close"));
	connect(btnClose, SIGNAL(clicked()), this, SLOT(hide()));*/

	btnStopWarning = new QPushButton();
	QPixmap pixmapBtnStop(":/sys/Pause1");
	btnStopWarning->setPixmap(pixmapBtnStop);
	btnStopWarning->setFixedSize(pixmapBtnStop.size());
	btnStopWarning->setFlat(true);
	//btnStopWarning->setPicName(QString(":/sys/Pause1"));
	//btnStopWarning->setText(tr("Stop"));
	labelWarningPic = new QLabel();
	QPixmap pixmapWarning(":/sys/Alert");
	labelWarningPic->setPixmap(pixmapWarning);
	labelWarningPic->setFixedSize(pixmapWarning.size());

	labelWarningInfo = new QLabel();
	QFont font("宋体",15,QFont::DemiBold,false);
	labelWarningInfo->setFont(font);
	labelWarningInfo->setText(QString::fromLocal8Bit("取消提醒!"));
	labelWarningInfo->setAlignment(Qt::AlignCenter);
	
	Init();
}
void pushLicense::Init()
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);//去掉标题栏
	QDesktopWidget* desktopWidget = QApplication::desktop();
	setAttribute(Qt::WA_QuitOnClose,false);
	setMinimumSize(400,300);
	/*QHBoxLayout *layoutTitle = new QHBoxLayout();

	layoutTitle->addStretch();
	layoutTitle->addWidget(btnClose, 0, Qt::AlignTop);
	layoutTitle->setContentsMargins(0,0,0,0);*/

	QVBoxLayout *layoutInfo = new QVBoxLayout();
	layoutInfo->addWidget(labelWarningInfo,Qt::AlignCenter);
	layoutInfo->addWidget(btnStopWarning,Qt::AlignCenter);

	QHBoxLayout *layoutBody = new QHBoxLayout();
	layoutBody->addWidget(labelWarningPic,Qt::AlignCenter);
	layoutBody->addLayout(layoutInfo,Qt::AlignCenter);
	layoutBody->setContentsMargins(10,0,10,0);
	layoutBody->setStretch(0,1);
	layoutBody->setStretch(1,1);

	QVBoxLayout *layoutMain = new QVBoxLayout(this);
	//layoutMain->addLayout(layoutTitle);
	layoutMain->addLayout(layoutBody);
	layoutMain->setStretch(1,1);
	layoutMain->setContentsMargins(0,0,0,0);
	MaxNumber=new QTimer(this);
	MaxNumber->setInterval(2000);
	connect(MaxNumber, SIGNAL(timeout()), this, SLOT(showFirst()));  
}
void pushLicense::showFirst()
{
	raise();
}
void pushLicense::slots_ShowWarning(int warningType, QString warningInfo)
{
	btnStopWarning->setVisible(false);
	labelWarningInfo->setText(warningInfo);
	QDesktopWidget* desktopWidget = QApplication::desktop();
	move(desktopWidget->width()-400,desktopWidget->height()-300);
	//setMinimumSize(desktopWidget->width()-400,desktopWidget->height()-400);
	show();
	raise();
}
pushLicense::~pushLicense()
{

}