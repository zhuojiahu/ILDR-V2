#include "userwidget.h"

#include <QSettings>

#include "clogfile.h"
#include "widget_info.h"
#include "Widget_CarveSetting.h"
#include "widget_test.h"
#include "widget_title.h"
#include "UserManegeWidget.h"
#include "ILDRSystem.h"

extern SysMainUI *pMainFrm;

UserWidget::UserWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
//	ui.lineEditUser->setText(tr("User"));
	ui.lineEdit_passWord->setEchoMode(QLineEdit::Password); 
	ui.lineEdit_passWord2->setEchoMode(QLineEdit::Password); 
	ui.lineEdit_newPassWord->setEchoMode(QLineEdit::Password); 
	ui.lineEdit_newPassWord2->setEchoMode(QLineEdit::Password); 
	ui.pushButton_login->setShortcut( QKeySequence::InsertParagraphSeparator );
	iSizeType = 0;
	ui.label_password_4->setVisible(false);
	ui.lineEdit_newPassWord2->setVisible(false);
	ui.pushButton_changePassWord->setVisible(false);
	initial();

}

UserWidget::~UserWidget()
{
	CLogFile::write(QObject::tr("ReleaseUserWidget"),OperationLog);
//	delete sqliteUser;
}
void UserWidget::initial()
{
	QPixmap pixmap = QPixmap(":/sys/UserBeforeLogin");
	ui.toolButton_accountManege->setIcon(pixmap);
	ui.toolButton_accountManege->setIconSize(pixmap.size());
	//设置大小
	ui.toolButton_accountManege->setFixedSize(pixmap.width(), pixmap.height());
	ui.toolButton_accountManege->setAutoRaise(true);

	ui.pushButton_pleaseLogin->setVisible(true);

	ui.widget_LogIn->setVisible(false);
	ui.widget_logedIn->setVisible(false);
	ui.widget_ChangePassWord->setVisible(false);
	QFont fontStatus;
	fontStatus.setPixelSize(22);
	ui.label_status->setFont(fontStatus);
	ui.label_status->setAlignment(Qt::AlignCenter);

	connect(ui.pushButton_pleaseLogin,SIGNAL(clicked()),this,SLOT(slots_pleaseLogin()));
	connect(ui.pushButton_login,SIGNAL(clicked()),this,SLOT(slots_login()));
	connect(ui.pushButton_cancel,SIGNAL(clicked()),this,SLOT(slots_cancel()));
	connect(ui.pushButton_logout,SIGNAL(clicked()),this,SLOT(slots_cancel()));
	connect(ui.pushButton_changePassWord,SIGNAL(clicked()),this,SLOT(slots_showChangePassword()));
	connect(ui.btnOK_changePassWrod,SIGNAL(clicked()),this,SLOT(slots_changePassWrod()));
	connect(ui.btnCancel_changePassWrod,SIGNAL(clicked()),this,SLOT(slots_CancelchangePassWrod()));
//	connect(ui.toolButton,SIGNAL(clicked()),this,SLOT(slots_ChangeWidgetSize()));
	connect(ui.toolButton_accountManege,SIGNAL(clicked()),this,SLOT(slots_accountManege()));

	eLoginState = BeforLogin;

	ui.widget->setWidgetName(tr("User"));
	ui.layout_title->addWidget(ui.widget->widgetName, 0, Qt::AlignTop);
	ui.layout_title->setSpacing(0);
	ui.layout_title->setContentsMargins(0, 0, 0, 0);
	//ui.widget->setWidgetNameColor(QColor(0,0,255,255));
	//setMaximumWidth(150);

	initialUserInfo();
}
void UserWidget::initialUserInfo()
{
	QSettings sysSet("daheng","GlasswareDetectSystem");
	QString strUsers = sysSet.value("Users","").toString();
	strUserList = strUsers.split(",");
	QString strPassword = sysSet.value("Password","").toString();
	strPasswordList = strPassword.split(",");
	QString strPermissions = sysSet.value("Permissions","").toString();
	QStringList strPermissionsList = strPermissions.split(",");
	nPermissionsList.clear();
	for (int i=0;i<strPermissionsList.size();i++)
	{
		nPermissionsList.append(strPermissionsList.at(i).toInt());
	}
	strUserList.removeAt(strPermissionsList.size()-1);
	strPasswordList.removeAt(strPermissionsList.size()-1);
	strPermissionsList.removeAt(strPermissionsList.size()-1);
	nPermissionsList.removeAt(nPermissionsList.size()-1);

	int iUserNumber = strUserList.size();
	//用户数为零增加2个用户
	if (0 == iUserNumber)
	{
		strUserList.append("Admin");
		strUserList.append("daheng");
		strPasswordList.append("daheng");
		strPasswordList.append("");
		nPermissionsList.append(0x01FF);
		nPermissionsList.append(0x00FF);
		//保存到注册表
		QString strUsers;
		QString strPassword;
		QString strPermissions;
		for (int i = 0;i<nPermissionsList.size();i++)
		{
			strUsers = strUsers + strUserList.at(i) + ",";
			strPassword = strPassword + strPasswordList.at(i) + ",";
			strPermissions = strPermissions + QString::number(nPermissionsList.at(i)) + ",";
		}
		QSettings sysSet("daheng","GlasswareDetectSystem");
		sysSet.setValue("Users",strUsers);
		sysSet.setValue("Password",strPassword);
		sysSet.setValue("Permissions",strPermissions);
	}
	iUserNumber =  strUserList.size();

	listUser.clear();
	ui.comboBoxUser->clear();
	for (int i = 0;i<iUserNumber;i++)
	{
		QString strUserName = strUserList.at(i);
		listUser.append(strUserName);
		ui.comboBoxUser->addItem(strUserName);
	}
	ui.comboBoxUser->setCurrentIndex(strUserList.size()-1);
}

void UserWidget::slots_pleaseLogin()
{
	QPixmap pixmap = QPixmap(":/sys/UserBeforeLogin");
	ui.toolButton_accountManege->setIcon(pixmap);
	ui.toolButton_accountManege->setIconSize(pixmap.size());
	//设置大小
	ui.toolButton_accountManege->setFixedSize(pixmap.width(), pixmap.height());
	ui.toolButton_accountManege->setAutoRaise(true);

	ui.pushButton_pleaseLogin->setVisible(false);
	ui.widget_LogIn->setVisible(true);

	ui.widget_logedIn->setVisible(false);
	ui.widget_ChangePassWord->setVisible(false);
	eLoginState = Login;
}
void UserWidget::slots_login()
{
	int nUser = ui.comboBoxUser->currentIndex();
	strUserName = strUserList.at(nUser);
	strPassWordUser = strPasswordList.at(nUser);
	PassWord = ui.lineEdit_passWord->text();
	if ("Admin" != strUserName)
	{
		if (strPassWordUser == PassWord)
		{
			ui.label_status->setText(tr("Primary Permissions"));
			ui.pushButton_pleaseLogin->setVisible(false);

			ui.widget_LogIn->setVisible(false);
			ui.widget_logedIn->setVisible(true);
			ui.widget_ChangePassWord->setVisible(false);

		}
		else
		{
			QMessageBox::information(this,tr("Error"),tr("Wrong password"));
			return;
		}
	}
	else if ("Admin" == strUserName)
	{
		if (strPassWordUser == PassWord)
		{
			ui.label_status->setText(tr("Advanced Permissions"));
			ui.pushButton_pleaseLogin->setVisible(false);

			ui.widget_LogIn->setVisible(false);
			ui.widget_logedIn->setVisible(true);
			ui.widget_ChangePassWord->setVisible(false);

		}
		else if (PASSWORD3 == PassWord&&pMainFrm->m_sSystemInfo.m_bDebugMode)
		{
			ui.label_status->setText(tr("Highset Permissions'"));
			pMainFrm->m_sRunningInfo.m_iPermission = 10;
			ui.pushButton_pleaseLogin->setVisible(false);

			ui.widget_LogIn->setVisible(false);
			ui.widget_logedIn->setVisible(true);
			ui.widget_ChangePassWord->setVisible(false);
		}	
		else
		{
			QMessageBox::information(this,tr("Error"),tr("Wrong password"));
			return;
		}
	}
	else
	{
		QMessageBox::information(this,tr("Error"),tr("Wrong password"));
		return;
	}
	int nPermission = nPermissionsList.at(nUser);
	int nUserAdmin = (nPermission & 0x0100)>>8;

	pMainFrm->sPermission.iClear = (nPermission & 0x0080)>>7;
	pMainFrm->sPermission.iExit = (nPermission & 0x0040)>>6;
	pMainFrm->sPermission.iStartStop = (nPermission & 0x0020)>>5;
	pMainFrm->sPermission.iImageCarve = (nPermission & 0x0010)>>4;
	pMainFrm->sPermission.iVarietyManege = (nPermission & 0x0008)>>3;
	pMainFrm->sPermission.iSystemSet = (nPermission & 0x0004)>>2;
	pMainFrm->sPermission.iPLCSet = (nPermission & 0x0002)>>1;
	pMainFrm->sPermission.iAlgSet = (nPermission & 0x0001)>>0;
	if (strUserName == "Admin")
	{
		pMainFrm->m_sRunningInfo.m_iPermission = 10;

	}
	else if (nUserAdmin)
	{
		pMainFrm->m_sRunningInfo.m_iPermission = 5;
	}
	else
	{
		pMainFrm->m_sRunningInfo.m_iPermission = 1;
	}
	ui.lineEdit_passWord->clear();
	eLoginState = AfterLogin;
	QPixmap pixmap = QPixmap(":/sys/UserAfterLogin");
	ui.toolButton_accountManege->setIcon(pixmap);
	ui.toolButton_accountManege->setIconSize(pixmap.size());
	//设置大小
	ui.toolButton_accountManege->setFixedSize(pixmap.width(), pixmap.height());
	ui.toolButton_accountManege->setAutoRaise(true);

	if (pMainFrm->sPermission.iClear)
	{
		pMainFrm->title_widget->button_list.at(ENaviToolResetCount)->setEnabled(true);
	}else{
		pMainFrm->title_widget->button_list.at(ENaviToolResetCount)->setEnabled(false);
	}

	if (pMainFrm->sPermission.iExit)
	{
		pMainFrm->title_widget->button_list.at(ENaviToolExit)->setEnabled(true);
	}else{
		pMainFrm->title_widget->button_list.at(ENaviToolExit)->setEnabled(false);
	}

	if (pMainFrm->sPermission.iStartStop)
	{
		pMainFrm->title_widget->button_list.at(ENaviToolStart)->setEnabled(true);
	}else{
		pMainFrm->title_widget->button_list.at(ENaviToolStart)->setEnabled(false);
	}

	if (pMainFrm->sPermission.iSystemSet)
	{
		pMainFrm->title_widget->button_list.at(ENaviPageSettings)->setEnabled(true);
	}else{
		pMainFrm->title_widget->button_list.at(ENaviPageSettings)->setEnabled(false);
	}

	if (pMainFrm->sPermission.iAlgSet)
	{
		pMainFrm->title_widget->button_list.at(ENaviPageAlgSet)->setEnabled(true);
	}else{
		pMainFrm->title_widget->button_list.at(ENaviPageAlgSet)->setEnabled(false);
	}

	if (pMainFrm->sPermission.iPLCSet)
	{
		pMainFrm->title_widget->button_list.at(PLCPage)->setEnabled(true);
	}else{
		pMainFrm->title_widget->button_list.at(PLCPage)->setEnabled(false);
	}

	if(pMainFrm->m_sSystemInfo.m_iDeviceOnInitial != 2)
	{
		if (pMainFrm->sPermission.iImageCarve)
		{
			pMainFrm->widget_frontback->image_widget->buttonShowCarve->setVisible(true);
		}else{
			pMainFrm->widget_frontback->image_widget->buttonShowCarve->setVisible(false);
		}
	}
	else
	{
		if (pMainFrm->sPermission.iImageCarve)
		{
			pMainFrm->widget_clamp->image_widget->buttonShowCarve->setVisible(true);
		}else{
			pMainFrm->widget_clamp->image_widget->buttonShowCarve->setVisible(false);
		}
	}
	



	if (pMainFrm->m_sSystemInfo.m_bDebugMode&&pMainFrm->m_sRunningInfo.m_iPermission >= 5)
	{
		pMainFrm->widget_settings->ui.checkBox_saveFailureNormalImage->setEnabled(true);
		pMainFrm->widget_settings->ui.checkBox_saveFailureStressImage->setEnabled(true);

	}
	time(&pMainFrm->info_widget->tLastOperaTime);
	CLogFile::write(tr("Log in"),OperationLog,0);
}
void UserWidget::logout()
{
	slots_cancel();
}
void UserWidget::slots_cancel()
{
	QPixmap pixmap = QPixmap(":/sys/UserBeforeLogin");
	ui.toolButton_accountManege->setIcon(pixmap);
	ui.toolButton_accountManege->setIconSize(pixmap.size());
	//设置大小
	ui.toolButton_accountManege->setFixedSize(pixmap.width(), pixmap.height());
	ui.toolButton_accountManege->setAutoRaise(true);

	ui.pushButton_pleaseLogin->setVisible(true);

	ui.widget_LogIn->setVisible(false);
	ui.widget_logedIn->setVisible(false);
	ui.widget_ChangePassWord->setVisible(false);
	eLoginState = BeforLogin;
	pMainFrm->sPermission.clear();

	if (pMainFrm->m_sRunningInfo.m_iPermission >0 )
	{
		pMainFrm->title_widget->button_list.at(ENaviPageAlgSet)->setEnabled(false);
		pMainFrm->title_widget->button_list.at(ENaviPageSettings)->setEnabled(false);
		pMainFrm->title_widget->button_list.at(ENaviToolStart)->setEnabled(false);
		pMainFrm->title_widget->button_list.at(ENaviToolExit)->setEnabled(false);
		pMainFrm->title_widget->button_list.at(PLCPage)->setEnabled(false);
		if(pMainFrm->m_sSystemInfo.m_iDeviceOnInitial != 2)
		{
			pMainFrm->widget_frontback->image_widget->buttonShowCarve->setVisible(false);
			if(pMainFrm->widget_frontback->image_widget->bIsCarveWidgetShow)
			{
				pMainFrm->widget_frontback->image_widget->slots_showCarve();
			}
		}
		else
		{
			pMainFrm->widget_clamp->image_widget->buttonShowCarve->setVisible(false);
			if(pMainFrm->widget_clamp->image_widget->bIsCarveWidgetShow)
			{
				pMainFrm->widget_clamp->image_widget->slots_showCarve();
			}
		}

		pMainFrm->m_sRunningInfo.m_iPermission = 0;
	}

}
void UserWidget::slots_showChangePassword()
{
	ui.pushButton_pleaseLogin->setVisible(false);
	ui.widget_LogIn->setVisible(false);
	ui.widget_logedIn->setVisible(false);
	ui.widget_ChangePassWord->setVisible(true);

}
void UserWidget::slots_ChangeWidgetSize()
{
	if (0 == iSizeType)
	{
		switch(eLoginState)
		{
		case BeforLogin:
			slots_cancel();
			break;
		case Login:
			slots_pleaseLogin();
			break;
		case AfterLogin:
			slots_login();
			break;
		}
		setMaximumWidth(500);
		iSizeType = 1;
	}
	else if (1 == iSizeType)
	{
		ui.pushButton_pleaseLogin->setVisible(false);

		ui.widget_LogIn->setVisible(false);
		ui.widget_logedIn->setVisible(false);
		ui.widget_ChangePassWord->setVisible(false);

		setMaximumWidth(150);

		iSizeType = 0;
	}
}
void UserWidget::slots_changePassWrod()
{
	bool setSuccess = false;
	QSettings iniPassWord("C:\\Windows\\kongpingSN.ini",QSettings::IniFormat);
	iniPassWord.setIniCodec(QTextCodec::codecForName("GBK"));

	PassWord = ui.lineEdit_passWord2->text();
	QString newPassWord = ui.lineEdit_newPassWord->text();
//	QString newPassWord2 = ui.lineEdit_newPassWord2->text();
	if ("User" == strUserName)
	{
		if (strPassWordUser == PassWord)
		{
 			if (true)
// 			if (newPassWord == newPassWord2)
			{
				iniPassWord.beginGroup("PassWord");
				iniPassWord.setValue(QString("User"),(QString)(newPassWord));
				iniPassWord.endGroup();
				strPassWordUser = newPassWord;
				setSuccess = true;
				QMessageBox::information(this,tr("Information"),tr("The new password set successfully"));

				CLogFile::write(tr("UserChange PassWord"),OperationLog,0);

			}
			else
			{
				QMessageBox::information(this,tr("Error"),tr("New password does not match"));
				return;
			}
		}
		else
		{
			QMessageBox::information(this,tr("Error"),tr("Wrong password"));
		}
	}
	else if ("Admin" == strUserName)
	{
		if (strPassWordAdmin == PassWord)
		{
			if (true)
// 			if (newPassWord == newPassWord2)
			{
				iniPassWord.beginGroup("PassWord");
				iniPassWord.setValue(QString("Admin"),(QString)(newPassWord));
				iniPassWord.endGroup();
				strPassWordAdmin = newPassWord;
				setSuccess = true;
				QMessageBox::information(this,tr("Information"),tr("The new password set successfully"));
				CLogFile::write(tr("Admin Change PassWord"),OperationLog,0);

			}
			else
			{
				QMessageBox::information(this,tr("Error"),tr("New password does not match"));
			}
		}
		else
		{
			QMessageBox::information(this,tr("Error"),tr("Wrong password"));
		}
	}
	ui.lineEdit_newPassWord->clear();
	ui.lineEdit_newPassWord2->clear();
	if (setSuccess)
	{
		ui.pushButton_pleaseLogin->setVisible(false);
		ui.widget_LogIn->setVisible(false);
		ui.widget_logedIn->setVisible(true);
		ui.widget_ChangePassWord->setVisible(false);
	}
}
void UserWidget::slots_CancelchangePassWrod()
{
	ui.pushButton_pleaseLogin->setVisible(false);

	ui.widget_LogIn->setVisible(false);
	ui.widget_logedIn->setVisible(true);
	ui.widget_ChangePassWord->setVisible(false);

}
void UserWidget::slots_accountManege()
{
	if (pMainFrm->m_sRunningInfo.m_iPermission>=5)
	{
		pMainFrm->widget_UserManege->initial();	
		pMainFrm->info_widget->setVisible(false);
		pMainFrm->widget_UserManege->setVisible(true);
		pMainFrm->widget_UserManege->getUserInfo();
	}
}
