#ifndef WIDGETWARNING_H
#define WIDGETWARNING_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QThread>
#include "pushButton.h"
#pragma once 
// class GlasswareDetectSystem;

class HideThread : public QThread
{
	Q_OBJECT

public:
	HideThread(QObject *parent);
	~HideThread();
public:
	void run();
	//BOOL Rotate270Image(PBYTE m_pImageBuff,int m_lImageWidth,int m_lImageHeight);
	void WaitThreadStop();
signals:
	void signal_Move(int ,int );
	void signal_HideWidget();
public:
	bool m_bStopThread;					//½áÊø¼ì²â

};


class WidgetWarning : public QWidget
{
	Q_OBJECT

public:
	WidgetWarning(QWidget *parent = 0);
	~WidgetWarning();

	void Init();
 	bool IsShowWarning();
public slots:
	void slots_ShowWarning(int warningType, QString warningInfo);//warningType1:Ìß·ÏÂÊ±¨¾¯£º2PLC±¨¾¯
	void slots_HideWarning(int);
	void slots_Move(int ,int );
	void slots_HideWidget();
	void slots_StopAlert();
	void slots_InfoShow();
    void slots_ReTim();
signals:
	void signals_PauseAlert();
public:
	bool bIsShow;
	QLabel *labelWarningPic;
	QLabel *labelWarningInfo;

	PushButton *btnClose;
	QPushButton *btnStopWarning;
	HideThread *pHideThread;
	bool bHiding;
	int iWarningType;
public:
	QTimer* MaxNumber;
	QStringList InfoList;
	QTimer *InfoShowTimer;
	int InfoIndex;
};

#endif // WIDGETWARNING_H
