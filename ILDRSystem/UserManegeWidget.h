#ifndef USERMANEGEWIDGET_H
#define USERMANEGEWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QSignalMapper>
#include "ui_UserManegeWidget.h"
#include "common.h"

class UserManegeWidget : public QWidget
{
	Q_OBJECT

public:
	UserManegeWidget(QWidget *parent = 0);
	~UserManegeWidget();

	QString strUserName;
	QString strPassWord;
	int iUserAdmin;
	QStringList strUserList;
	QStringList strPasswordList;
	QList<int> nPermissionsList;
	int nUser;
	QList<QCheckBox*> listCheckBoxPerm;
	QList<QToolButton*> listToolButtonUser;
	QList<QString> listStrUserName;
	QSignalMapper *signal_mapper;
	SQLSTATE eSQLState;
private:
	Ui::UserManegeWidget ui;
	QGridLayout *layoutPerm;
public:
	void initial();
	void initPerm();
	void getUserInfo();
	void getUserPerm();

public slots:
	void slots_new();
	void slots_OK();
	void slots_Cancel();
	void slots_OK2();
	void slots_Cancel2();
	void slots_delete();
	void slots_clickAccont(int iIndex);

};

#endif // USERMANEGEWIDGET_H
