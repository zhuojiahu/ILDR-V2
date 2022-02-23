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

    //用于重置当前界面统计数据
    void slots_resetCurrent();

private:
    Ui::uiOperInfo* ui;

    //当前结果
    bool bSucceed;
    int imgNO;                  //当前图像号
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
};

#endif //UI_OPERATION_H_
