#ifndef WIDGET_USERMANEGMENT_H
#define WIDGET_USERMANEGMENT_H

#include <QWidget>
#include "ui_Widget_UserManegment.h"

class Widget_UserManegment : public QWidget
{
	Q_OBJECT

public:
	Widget_UserManegment(QWidget *parent = 0);
	~Widget_UserManegment();

private:
	Ui::Widget_UserManegment ui;
};

#endif // WIDGET_USERMANEGMENT_H
