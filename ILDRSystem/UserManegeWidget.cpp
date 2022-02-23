#include "UserManegeWidget.h"

#include "math.h"

#include <QToolButton>

#include "clogfile.h"
#include "widget_info.h"
#include "ILDRSystem.h"
extern SysMainUI *pMainFrm;

UserManegeWidget::UserManegeWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	iUserAdmin = 0;
	layoutPerm = new QGridLayout(ui.widget_account);

	signal_mapper = new QSignalMapper(this);//工具栏的信号管理

	connect(ui.pushButton_OK,SIGNAL(clicked()),this,SLOT(slots_OK()));
	connect(ui.pushButton_Cancel,SIGNAL(clicked()),this,SLOT(slots_Cancel()));
	connect(ui.pushButton_New,SIGNAL(clicked()),this,SLOT(slots_new()));
	connect(ui.pushButton_Delete,SIGNAL(clicked()),this,SLOT(slots_delete()));
	connect(ui.pushButton_OK2,SIGNAL(clicked()),this,SLOT(slots_OK2()));
	connect(ui.pushButton_Cancel2,SIGNAL(clicked()),this,SLOT(slots_Cancel2()));

	connect(signal_mapper, SIGNAL(mapped(int)), this, SLOT(slots_clickAccont(int)));

	initial();
 	initPerm();	

}

UserManegeWidget::~UserManegeWidget()
{
	CLogFile::write(QObject::tr("ReleaseUserManage"),OperationLog);
	listCheckBoxPerm.clear();

}
void UserManegeWidget::initial()
{
	strUserList = pMainFrm->info_widget->widgetUser->strUserList;
	strPasswordList = pMainFrm->info_widget->widgetUser->strPasswordList;
	nPermissionsList = pMainFrm->info_widget->widgetUser->nPermissionsList;

	ui.widget_maneger->setVisible(true);
	ui.widget_Set->setVisible(false);

	getUserInfo();

}
void UserManegeWidget::getUserInfo()
{
	int iUserNumber = strUserList.size();

	//int itemCount = layout()->count(); // layout是你前一个布局 
	int itemCount = layoutPerm->count();
	for (int i = (itemCount - 1); i >= 0; --i) //从末尾开始是因为你删除会影响布局的顺序。例如你删掉第一个，后面的会往前移，第二就变成第一个，然后这时你要是++i的话，就是删掉原来布局里的第三个，这第二个被跳过了。 
	 { 
		QLayoutItem *item = layoutPerm->takeAt(i); 
		if (item != 0) 
		{ 
			layoutPerm->removeWidget(item->widget()); 
			delete item->widget(); //（ps：如果是子控件不是QWidget，这里会出错，要注意）

		}
	 }
	listStrUserName.clear();
	listToolButtonUser.clear();
	for (int i = 0;i<iUserNumber;i++)
	{
		QString strUserName = strUserList.at(i);
		listStrUserName.append(strUserName);
		QToolButton *toolBotton = new QToolButton();
		toolBotton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

		QPixmap pixmap(":/sys/UserAfterLogin");
		toolBotton->setIcon(pixmap);
		toolBotton->setIconSize(pixmap.size());
		toolBotton->setFixedSize(pixmap.width()+150, pixmap.height()+15);
//		toolBotton->setAutoRaise(true);
		toolBotton->setText(strUserName);

		listToolButtonUser.append(toolBotton);
		connect(toolBotton, SIGNAL(clicked()), signal_mapper, SLOT(map()));
		signal_mapper->setMapping(toolBotton, i);
		//signal_mapper->setMapping(toolBotton, QString::number(i, 10));

		layoutPerm->addWidget(toolBotton,i/2,i%2);
	}

}
void UserManegeWidget::getUserPerm()
{
	strUserName = strUserList.at(nUser);
	strPassWord = strPasswordList.at(nUser);
	int nPermission = nPermissionsList.at(nUser);
	iUserAdmin = (nPermission & 0x0100)>>8;
	if (strUserName == "Admin")
	{
		pMainFrm->m_sRunningInfo.m_iPermission = 10;

	}
	else if (iUserAdmin)
	{
		pMainFrm->m_sRunningInfo.m_iPermission = 5;
	}
	else
	{
		pMainFrm->m_sRunningInfo.m_iPermission = 1;
	}
//	pMainFrm->m_sRunningInfo.m_iPermission = (nPermission & 0x0100)>>8;

	pMainFrm->sPermission.iClear = (nPermission & 0x0080)>>7;
	pMainFrm->sPermission.iExit = (nPermission & 0x0040)>>6;
	pMainFrm->sPermission.iStartStop = (nPermission & 0x0020)>>5;
	pMainFrm->sPermission.iImageCarve = (nPermission & 0x0010)>>4;
	pMainFrm->sPermission.iVarietyManege = (nPermission & 0x0008)>>3;
	pMainFrm->sPermission.iSystemSet = (nPermission & 0x0004)>>2;
	pMainFrm->sPermission.iPLCSet = (nPermission & 0x0002)>>1;
	pMainFrm->sPermission.iAlgSet = (nPermission & 0x0001)>>0;

	if (iUserAdmin>=1){
		ui.checkBox_IsAdmin->setChecked(true);
	}
	else{
		ui.checkBox_IsAdmin->setChecked(false);
	}
	ui.lineEdit_SN->setText(strPassWord);
	if (pMainFrm->sPermission.iClear){
		listCheckBoxPerm.at(0)->setChecked(true);
	}
	else{
		listCheckBoxPerm.at(0)->setChecked(false);
	}
	if (pMainFrm->sPermission.iExit){
		listCheckBoxPerm.at(1)->setChecked(true);
	}	
	else{
		listCheckBoxPerm.at(1)->setChecked(false);
	}
	if (pMainFrm->sPermission.iStartStop){
		listCheckBoxPerm.at(2)->setChecked(true);
	}	
	else{
		listCheckBoxPerm.at(2)->setChecked(false);
	}
	if (pMainFrm->sPermission.iImageCarve){
		listCheckBoxPerm.at(3)->setChecked(true);
	}	
	else{
		listCheckBoxPerm.at(3)->setChecked(false);
	}
	if (pMainFrm->sPermission.iVarietyManege){
		listCheckBoxPerm.at(4)->setChecked(true);
	}	
	else{
		listCheckBoxPerm.at(4)->setChecked(false);
	}
	if (pMainFrm->sPermission.iSystemSet){
		listCheckBoxPerm.at(5)->setChecked(true);
	}	
	else{
		listCheckBoxPerm.at(5)->setChecked(false);
	}
	if (pMainFrm->sPermission.iPLCSet){
		listCheckBoxPerm.at(6)->setChecked(true);
	}	
	else{
		listCheckBoxPerm.at(6)->setChecked(false);
	}
	if (pMainFrm->sPermission.iAlgSet){
		listCheckBoxPerm.at(7)->setChecked(true);
	}
	else{
		listCheckBoxPerm.at(7)->setChecked(false);
	}
}

void UserManegeWidget::initPerm()
{
	listCheckBoxPerm.clear();
	QGridLayout *layoutPerm = new QGridLayout(ui.widget_PermSet);
	QCheckBox *checkboxClear = new QCheckBox();
	checkboxClear->setText(tr("Clear"));
	listCheckBoxPerm.append(checkboxClear);

	QCheckBox *checkboxExit = new QCheckBox();
	checkboxExit->setText(tr("Exit"));
	listCheckBoxPerm.append(checkboxExit);

	QCheckBox *checkboxStartStop = new QCheckBox();
	checkboxStartStop->setText(tr("StartStop"));
	listCheckBoxPerm.append(checkboxStartStop);

	QCheckBox *checkboxCarve = new QCheckBox();
	checkboxCarve->setText(tr("ImageCarve"));
	listCheckBoxPerm.append(checkboxCarve);

	QCheckBox *checkboxManege = new QCheckBox();
	checkboxManege->setText(tr("VarietyManege"));
	listCheckBoxPerm.append(checkboxManege);

	QCheckBox *checkboxSet = new QCheckBox();
	checkboxSet->setText(tr("SystemSet"));
	listCheckBoxPerm.append(checkboxSet);

	QCheckBox *checkboxPLCSet = new QCheckBox();
	checkboxPLCSet->setText(tr("PLCSet"));
	listCheckBoxPerm.append(checkboxPLCSet);

	QCheckBox *checkboxAlg = new QCheckBox();
	checkboxAlg->setText(tr("AlgSet"));
	listCheckBoxPerm.append(checkboxAlg);
	
	for (int i = 0;i<listCheckBoxPerm.size();i++)
	{
		layoutPerm->addWidget(listCheckBoxPerm.at(i),i/2,i%2);
	}

}

void UserManegeWidget::slots_OK()
{
	pMainFrm->info_widget->setVisible(true);
	pMainFrm->widget_UserManege->setVisible(false);
	pMainFrm->info_widget->widgetUser->initialUserInfo();
}
void UserManegeWidget::slots_Cancel()
{
	pMainFrm->info_widget->setVisible(true);
	pMainFrm->widget_UserManege->setVisible(false);
	pMainFrm->info_widget->widgetUser->initialUserInfo();
}
void UserManegeWidget::slots_OK2()
{
	ui.widget_maneger->setVisible(true);
	ui.widget_Set->setVisible(false);
	if (eSQLState == SQL_MODEFY)
	{
 		QString strName = ui.lineEdit_Name->text();
		QString strPassWord = ui.lineEdit_SN->text();
		int iPerm = 0;

		if (ui.checkBox_IsAdmin->isChecked())
		{
			iPerm = iPerm|0x0100;
		}
		for (int i = 0; i<8; i++)
		{
			if (listCheckBoxPerm.at(7-i)->isChecked())
			{
				iPerm = iPerm|(int)pow((double)2,i);
			}
		}
		strUserList.removeAt(nUser);
		strPasswordList.removeAt(nUser);
		nPermissionsList.removeAt(nUser);
		strUserList.insert(nUser,strName);
		strPasswordList.insert(nUser,strPassWord);
		nPermissionsList.insert(nUser,iPerm);


		//保存到注册表
		QString strUsers;
		QString strPassword;
		QString strPermissions;
		for (int i = 0;i<strUserList.size();i++)
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

 	if (eSQLState == SQL_NEW)
	{
		QString strName = ui.lineEdit_Name->text();
		QString strPassWord = ui.lineEdit_SN->text();

		bool iAlreadyHave = false;
		for (int i = 0;i<strUserList.size();i++)
		{
			if (strName == strUserList.at(i))
			{
				iAlreadyHave = true;
			}
		}
		if (iAlreadyHave)
		{
			QMessageBox::information(this,tr("Error"),tr("This user name is used!"));
			return;
		}

		int iPerm = 0;
		if (ui.checkBox_IsAdmin->isChecked())
		{
			iPerm = iPerm|0x0100;
		}
		for (int i = 0; i<8; i++)
		{
			if (listCheckBoxPerm.at(7-i)->isChecked())
			{
				iPerm = iPerm|(2^i);
			}
		}
		strUserList.append(strName);
		strPasswordList.append(strPassWord);
		nPermissionsList.append(iPerm);


		//保存到注册表
		QString strUsers;
		QString strPassword;
		QString strPermissions;
		for (int i = 0;i<strUserList.size();i++)
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
	if (eSQLState == SQL_DELETE)
	{
		//保存到注册表
		QString strUsers;
		QString strPassword;
		QString strPermissions;
		for (int i = 0;i<strUserList.size();i++)
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
	
	getUserInfo();
}
void UserManegeWidget::slots_Cancel2()
{
	ui.widget_maneger->setVisible(true);
	ui.widget_Set->setVisible(false);

}
void UserManegeWidget::slots_new()
{
	ui.lineEdit_Name->setText("");
	ui.pushButton_Delete->setDisabled(true);
	ui.lineEdit_Name->setEnabled(true);
	ui.pushButton_Delete->setEnabled(false);
	for (int i = 0;i < listCheckBoxPerm.size();i++)
	{
		listCheckBoxPerm.at(i)->setEnabled(true);
		listCheckBoxPerm.at(i)->setChecked(false);
	}

	ui.widget_maneger->setVisible(false);
	ui.widget_Set->setVisible(true);
	eSQLState = SQL_NEW;

}
void UserManegeWidget::slots_delete()
{
	strUserList.removeAt(nUser);
	strPasswordList.removeAt(nUser);
	nPermissionsList.removeAt(nUser);
	eSQLState = SQL_DELETE;
	slots_OK2();

}

void UserManegeWidget::slots_clickAccont(int iIndex)
{
	nUser = iIndex;
	ui.widget_maneger->setVisible(false);
	ui.widget_Set->setVisible(true);
	strUserName = listStrUserName.at(iIndex);
	ui.lineEdit_Name->setText(strUserName);
	ui.pushButton_Delete->setEnabled(true);

	getUserPerm();

	if (strUserName == "Admin")
	{
		ui.lineEdit_Name->setEnabled(false);
		ui.pushButton_Delete->setEnabled(false);
		for (int i = 0;i < listCheckBoxPerm.size();i++)
		{
			listCheckBoxPerm.at(i)->setEnabled(false);
		}
	}
	else	
	{
		ui.lineEdit_Name->setEnabled(true);
		ui.pushButton_Delete->setEnabled(true);
		for (int i = 0;i < listCheckBoxPerm.size();i++)
		{
			listCheckBoxPerm.at(i)->setEnabled(true);
		}
	}

	eSQLState = SQL_MODEFY;

}