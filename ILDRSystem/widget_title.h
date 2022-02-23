#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QSignalMapper>
#include <QVBoxLayout>
#include "pushButton.h"
#include "toolButton.h"
#include "common.h"

class WidgetTitle : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetTitle(QWidget *parent = 0);
	void addToolName();

signals:

	void showSkin();
    void showMin();
	void showMainMenu();
	void closeWidget();
	void turnPage(int current_page);

public slots:
    void turnPage(QString current_page);
    void slots_setLockState(bool bLock);

private:

    QPoint press_point;//鼠标按下去的点

	QLabel *version_title; //标题
	QLabel *labelVersion; //版本信息
    PushButton *min_button; //最小化
    PushButton *close_button; //关闭
public:
	QList<ToolButton *> button_list;
};

#endif // TITLEWIDGET_H
