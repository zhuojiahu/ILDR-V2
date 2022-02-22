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

    //用于重置当前界面统计数据
    void slots_resetCurrent();
signals:
    //用于触发全局统计的计数清零
    void signals_resetCount();

private slots:
    //用于本界面重置按钮
    void on_btnReset_clicked();

private:
    Ui::uiOperation* ui;

    //当前结果
    bool bSucceed;
    QString id;                 //结果ID
    QDateTime dtResult;         //结果时间
    QString CavityNumber;       //识别工位
    QDateTime gmtTimeStamp;     //结果时间戳

    //当前统计结果
    int nTotal;     //过检总数
    int nReject;    //剔废数
    int nRead;      //成功识别数,失败数:总数-识别数
    int nInTime;    //及时识别数,超时数:成功识别数-及时识别数
    int nEngraved;  //?不知道含义

    QTimer* trUpdate;   //用于触发界面刷新(统计数据)
};

#endif //UI_OPERATION_H_
