#ifndef WIDGET_CAMERAPOSITION_H
#define WIDGET_CAMERAPOSITION_H

#include <QWidget>
#include "ui_Widget_CameraPosition.h"

class Widget_CameraPosition : public QWidget
{
	Q_OBJECT

public:
	Widget_CameraPosition(QWidget *parent = 0);
	~Widget_CameraPosition();

public:
	Ui::Widget_CameraPosition ui;
};

#endif // WIDGET_CAMERAPOSITION_H
