#ifndef pushLicense_H
#define pushLicense_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QThread>
#include "pushButton.h"
#pragma once 
// class GlasswareDetectSystem;
class pushLicense : public QWidget
{
	Q_OBJECT

public:
	pushLicense(QWidget *parent = 0);
	~pushLicense();
	void Init();
	void slots_ShowWarning(int warningType, QString warningInfo);//warningType1:�߷��ʱ�����2PLC����
public:
	bool bIsShow;
	QLabel *labelWarningPic;
	QLabel *labelWarningInfo;

	PushButton *btnClose;
	QPushButton *btnStopWarning;
	bool bHiding;
	int iWarningType;
public slots:
	void showFirst();
public:
	QTimer* MaxNumber;
};

#endif // pushLicense_H
