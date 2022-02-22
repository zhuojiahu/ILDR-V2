#ifndef CPASSWORDDLG_H
#define CPASSWORDDLG_H

#include <QDialog>
#include "ui_cpassworddlg.h"

class CPasswordDlg : public QDialog
{
	Q_OBJECT

public:
	CPasswordDlg(QWidget *parent = 0);
	~CPasswordDlg();

public:
	QString GetPassword();
	bool isOK();

private slots:
	void slot_OKBtn();
	void slot_CancelBtn();
	void slot_ChangePwdBtn(bool isChecked);
	void slot_PwdEditTextChange(QString pText);

private:
	Ui::CPasswordDlg ui;
	bool isCheckOK;
	bool isChangePwd;
};

#endif // CPASSWORDDLG_H
