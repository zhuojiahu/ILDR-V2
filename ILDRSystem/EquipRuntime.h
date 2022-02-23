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

	void initLogFile();			//��ʼ��Log�ļ�
	void ReadLogFile();			//��ʼ����������ʱ��
	void ResetLogFile();		//�豸��ɱ������������ñ�����ʱ��
	void EquipExitLogFile();	//����˳���¼���������ʱ��

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
