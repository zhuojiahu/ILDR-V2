#ifndef UI_OPERATION_H_
#define UI_OPERATION_H_

#include <QWidget>
#include <QDateTime>

namespace Ui
{
    class uiOperation;
}
class QTimer;

class UIOperation : public QWidget
{
    Q_OBJECT
public:
	explicit UIOperation(QWidget *parent = 0);
	virtual ~UIOperation();

public slots:
    //UI Update
    void slots_showCameraImage(int nImgNO, QImage* img);
    void slots_showResult(int nImgNO);
    void slots_updateCount();
    void slots_updateCount(int total, int reject, int read, int intime, int engraved);

    //�������õ�ǰ����ͳ������
    void slots_resetCurrent();
signals:
    //���ڴ���ȫ��ͳ�Ƶļ�������
    void signals_resetCount();

private slots:
    //���ڱ��������ð�ť
    void on_btnReset_clicked();

private:
    Ui::uiOperation* ui;

    //��ǰ���
    bool bSucceed;
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

    QTimer* trUpdate;   //���ڴ�������ˢ��(ͳ������)
};

#endif //UI_OPERATION_H_
