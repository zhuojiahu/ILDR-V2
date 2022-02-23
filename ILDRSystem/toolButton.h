#ifndef TOOlBUTTON_H
#define TOOlBUTTON_H

#include <QToolButton>
#include <QMouseEvent>
#include <QPainter>
#define TITELSTYLE		1
#define SETTINGSTYLE	2


class ToolButton : public QToolButton
{

public:
	explicit ToolButton(QString pic_name, QWidget *parent = 0);
	~ToolButton();
	void setMousePress(bool mouse_press);

protected:

	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
	void mousePressEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);
	void painterInfo(int top_color, int middle_color, int bottom_color);

public:

	bool mouse_over; //鼠标是否移过
	bool mouse_press; //鼠标是否按下
	int btnStyle;//样式，1为标题栏，2为设置栏
	bool bStatus;
};

#endif //TOOlBUTTON_H
