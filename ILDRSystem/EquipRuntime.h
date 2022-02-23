#ifndef EQUIPRUNTIME_H
#define EQUIPRUNTIME_H

#include <QObject>
#include <QDateTime>
#include <QTimer>

class EquipRuntime : public QObject
{
	Q_OBJECT

public:
	static EquipRuntime *Instance();
	EquipRuntime(QObject *parent=0);
	~EquipRuntime();

	void start();
	void stop();

	void initLogFile();			//初始化Log文件
	void ReadLogFile();			//初始化各报警项时间
	void ResetLogFile();		//设备完成保养，重新设置报警项时间
	void EquipExitLogFile();	//软件退出记录软件已运行时间

signals:
	void SendAlarms(int ,bool);

protected slots:
	void Slots_timer();

private:
	static QScopedPointer<EquipRuntime> self;
	QString LogFileName;

	int saveInterval;
	QDateTime startTime;
	QTimer *m_Timer;
	QList<int> m_CumulTime;

	QList<bool> m_AlarmsSatus;
};

#endif // EQUIPRUNTIME_H
