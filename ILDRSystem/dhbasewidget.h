#ifndef DHBASEWIDGET_H
#define DHBASEWIDGET_H

#include <QWidget>
#include <QLabel>

//#include "ui_dhbasewidget.h"

class DHBaseWidget : public QWidget
{
	Q_OBJECT

public:
	DHBaseWidget(QWidget *parent = 0);
	~DHBaseWidget();

	QLabel* widgetName;

	void setWidgetFont(QFont font);
	void setWidgetName(QString str);
	void setWidgetNameColor(QColor coloe);
protected:
	virtual void paintEvent(QPaintEvent *event);

private:
	QColor* widgetColor;
//	Ui::DHBaseWidget ui;
};

#endif // DHBASEWIDGET_H
