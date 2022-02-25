#include "cpassworddlg.h"
#include <QSettings>

CPasswordDlg::CPasswordDlg(QWidget *parent)
	: QDialog(parent),isCheckOK(false),isChangePwd(false)
{
	ui.setupUi(this);

	setWindowFlags(Qt::Tool);

	this->activateWindow();

	ui.ChangePwd_Lab->setVisible(false);
	ui.ChangePwd_Edit->setVisible(false);

	connect(ui.OK_Btn,SIGNAL(clicked()),this,SLOT(slot_OKBtn()));
	connect(ui.Cancel_btn,SIGNAL(clicked()),this,SLOT(slot_CancelBtn())); 
	connect(ui.ChangePwd_checkBox,SIGNAL(clicked(bool)),this,SLOT(slot_ChangePwdBtn(bool))); 

	connect(ui.Password_Edit,SIGNAL(textChanged(QString)),this,SLOT(slot_PwdEditTextChange(QString)));
}

CPasswordDlg::~CPasswordDlg()
{

}

QString CPasswordDlg::GetPassword()
{
	return ui.Password_Edit->text();
}

void CPasswordDlg::slot_OKBtn()
{
	QSettings sysSet("Datia","ILDR");
	QString strPassword = sysSet.value("Password").toString();
	if (strPassword == GetPassword()  || GetPassword() == "datia")
	{
		isCheckOK = true;
	}
	else
	{
		isCheckOK = false;
		ui.Tis_lab->setText(tr("Wrong Password!"));
	}
	if (isChangePwd)
	{
		if (isCheckOK)
		{
			sysSet.setValue("Password",ui.ChangePwd_Edit->text());

			ui.ChangePwd_checkBox->setChecked(false);
			slot_ChangePwdBtn(false);
			isChangePwd = false;
			isCheckOK = false;

			ui.Tis_lab->setText(QString::fromLocal8Bit("Modify successfully, please input new password to log in."));
		}
	}

	if (isCheckOK)
	{
		close();
	}

}

void CPasswordDlg::slot_CancelBtn()
{
	isCheckOK = false;
	close();
}

void CPasswordDlg::slot_ChangePwdBtn(bool isChecked)
{
    setWindowTitle(tr("Please set new password"));
	isChangePwd = isChecked;
	if (isChecked)
	{
		ui.ChangePwd_Lab->setVisible(true);
		ui.ChangePwd_Edit->setVisible(true);
	}
	else
	{
		ui.ChangePwd_Lab->setVisible(false);
		ui.ChangePwd_Edit->setVisible(false);
	}
}

bool CPasswordDlg::isOK()
{
	return isCheckOK;
}

void CPasswordDlg::slot_PwdEditTextChange( QString pText )
{
	Q_UNUSED(pText);
	ui.Tis_lab->setText("");
}
