#ifndef WIDGET_HELP_H
#define WIDGET_HELP_H

#include <QWidget>
#include <QPushButton>
#include "ui_widget_Help.h"

class widget_Help : public QWidget
{
	Q_OBJECT

public:
	widget_Help(QWidget *parent = 0);
	~widget_Help();
signals:
	void signal_OpenFile(int nFileNumber);
public slots:
	void slot_OpenFile(int nFileNumber);
private:
	void init();
	Ui::widget_Help ui;

	QList <QPushButton *> listPushbuttonOpenFile;
	QList <QString> listFileName;
	QString strHelpPath;


};

#endif // WIDGET_HELP_H
