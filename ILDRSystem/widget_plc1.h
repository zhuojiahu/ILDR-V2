#ifndef WIDGET_PLC1_H
#define WIDGET_PLC1_H

#include "qt_windows.h"
#include <QWidget>
#include <QTimer>
#include <QAction>
#include <QMenu>
#include <QPushButton>
#include <QThread>

#include "common.h"
#include "camerasatuslabel.h"

#include "HMIDLL.h"
#pragma comment(lib,"HMI.lib")

class SensorStatusLabel : public CameraStatusLabel
{
	Q_OBJECT

public:
	SensorStatusLabel(QWidget *parent);
	~SensorStatusLabel();
public:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void setMaxWidth(int width);
	void setMaxHeight(int height);
	void contextMenuEvent(QContextMenuEvent *event);

private slots:
	void slots_action_hide();
private:
	int iMaxWidth;
	int iMaxHeight;

	QPoint *pointPosition; 
	QPoint *mousePosition; 
	
	QAction *action_hide;
	QMenu *menu_SensorStatus;

};

class Widget_PLCImage : public QWidget
{
	Q_OBJECT

 public:
 	Widget_PLCImage(QWidget *parent);
 	~Widget_PLCImage();
	void contextMenuEvent(QContextMenuEvent *event);
protected:
	void paintEvent(QPaintEvent *event);
public:
	QAction *action_ResetSensorPosition;
	QAction *action_SaveSensorPosition;
	QAction *action_SetPLCPara;
private:
	QMenu *menu_PLCImage;
};

class ReadPLCThread : public QThread
{
	Q_OBJECT

public:
	ReadPLCThread(QObject *parent);
	~ReadPLCThread();

private:
	QString m_strPLCInfoPath;
	CHMIDLL *PLCContorl;
	bool m_bStopThread;
	QMutex mutexPLC;
signals:
	void signalsUpdatePLCStatus(s_PLCStatus);
public slots:
	void slots_SetPLCPara();
	void slots_SendPLCAlarm(int iAlarm);
	
public:
	void run();
	void StartThread();
	void StopThread();
	void WaitThreadStop();
};

class Widget_PLC1 : public QWidget
{
	Q_OBJECT

public:
	Widget_PLC1(QWidget *parent);
	~Widget_PLC1();
	bool leaveWidget();
public slots:
	void slots_intoWidget();
	void slots_ResetSensorPosition();
	void slots_SaveSensorPosition();
	void slots_SetPLCPara();
public slots:
	void resizeEvent(QResizeEvent *event);
signals:
	void signals_sendPLCStatus(int);
private:
	void initial();
	void setLabelText();
private slots:
	void slots_updatePLCInfo(s_PLCStatus);

public:
	ReadPLCThread* pThreadReadPLCStatus; 
private:
	Widget_PLCImage *widgetPLCImage;
	QWidget *widgetPLCInfo;
	QLabel *labelCoderCounter;
	QList<SensorStatusLabel *> listStatuslabel;

	QList<CameraStatusLabel *> listStatuslabelIN0;
	QList<CameraStatusLabel *> listStatuslabelIN1;
	QList<CameraStatusLabel *> listStatuslabelOUT0;
	QList<CameraStatusLabel *> listStatuslabelOUT1;

	QList<QLabel *> listSensorTextIN0;
	QList<QLabel *> listSensorTextIN1;
	QList<QLabel *> listSensorTextOUT0;
	QList<QLabel *> listSensorTextOUT1;

	QList<QString > listPLCInfoIN0;
	QList<QString > listPLCInfoIN1;
	QList<QString > listPLCInfoOUT0;
	QList<QString > listPLCInfoOUT1;

	QString m_strPLCInfoPath;
	QString m_strSensorPositionPath;
	int SensorPositionX[10];
	int SensorPositionY[10];

	int iMoveLabel;
};

#endif // WIDGET_PLC_H
