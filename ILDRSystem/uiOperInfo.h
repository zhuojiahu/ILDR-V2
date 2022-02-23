#ifndef UI_OPERATION_H_
#define UI_OPERATION_H_

#include <QWidget>
#include <QDateTime>

namespace Ui
{
    class uiOperInfo;
}
class QTimer;

class UIOperInfo : public QWidget
{
    Q_OBJECT
public:
	explicit UIOperInfo(QWidget *parent = 0);
	virtual ~UIOperInfo();

public slots:
    //UI Update
    void slots_updateResult(int _nImgNO, QString _id, QDateTime _dt, QString _cavityNum, QDateTime _gmtTime);
    void slots_updateCount(int total, int reject, int read, int intime, int engraved);

    //�������õ�ǰ����ͳ������
    void slots_resetCurrent();

private:
    Ui::uiOperInfo* ui;

    //��ǰ���
    bool bSucceed;
    int imgNO;                  //��ǰͼ���
    QString id;                 //���ID
    QDateTime dtResult;         //���ʱ��
    QString CavityNumber;       //ʶ��λ
    QDateTime gmtTimeStamp;     //���ʱ���

    //��ǰͳ�ƽ��
    int nTotal;     //��������
    int nReject;    //�޷���
    int nRead;      //�ɹ�ʶ����,ʧ����:����-ʶ����
    int nInTime;    //��ʱʶ����,��ʱ��:�ɹ�ʶ����-��ʱʶ����
    int nEngraved;  //?��֪������
};

#endif //UI_OPERATION_H_
