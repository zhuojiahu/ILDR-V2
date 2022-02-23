#ifndef USERWIDGET_H
#define USERWIDGET_H

#include <QWidget>
#include "ui_userwidget.h"
enum e_LoginStatus	
{
	BeforLogin = 0,
	Login,
	AfterLogin,
};
class UserWidget : public QWidget
{
	Q_OBJECT

public:
	UserWidget(QWidget *parent = 0);
	~UserWidget();

	void initialUserInfo();
	void logout();

	Ui::UserWidget ui;
	QList<QString> listUser;
	QStringList strUserList;
	QStringList strPasswordList;
	QList<int> nPermissionsList;

private:
	int iSizeType;
	e_LoginStatus eLoginState;
	QString strUserName;
	QString PassWord;
	QString strPassWordUser;
	QString strPassWordAdmin;
	int iUserPerm;
private:
	void initial();
private slots:
	void slots_pleaseLogin();
	void slots_login();
	void slots_cancel();
	void slots_showChangePassword();
	void slots_ChangeWidgetSize();
	void slots_changePassWrod();
	void slots_CancelchangePassWrod();
	void slots_accountManege();
};

#endif // USERWIDGET_H
