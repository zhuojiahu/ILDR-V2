#ifndef WIDGET_CAMERA_H
#define WIDGET_CAMERA_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QSignalMapper>
#include "ConfigInfo.h"


class Widget_Camera : public QWidget
{
	Q_OBJECT

public:
	Widget_Camera(QWidget *parent = 0);
	~Widget_Camera();

	void recoverChanged(){bIsChanged = false;}
	void setChanged(){bIsChanged = true;}
	bool isChanged(){return bIsChanged;}
	void initail();

	bool bIsChanged;
	bool bIsChosed[CAMERA_MAX_COUNT];
	QList<QCheckBox*> listCheckBox;
	QSignalMapper *signalmapper;

private slots:
	void slots_changeCheckState(int);

};

#endif // WIDGET_CAMERA_H
