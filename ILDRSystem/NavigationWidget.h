#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QWidget>

class PushButton;
class QLabel;
class ToolButton;
class QTimer;

class NavigationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NavigationWidget(QWidget *parent = 0);
    virtual ~NavigationWidget();
signals:
	void signals_navigateTrig(int current_page);

public slots:
    //操作
    void slots_turnPage(int index);
    void slots_updateTime();
    //设置状态
    void slots_selectCurrent(int idx);
    void slots_setLockState(bool bLock);
private:
    void initInterface();

private:
    QLabel* lDatetime;      //用于显示日期和时间
    QTimer* tUpdateTime;     //用于更新时间
	QList<ToolButton *> button_list;
};

#endif // TITLEWIDGET_H
