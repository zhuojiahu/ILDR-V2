#ifndef WIDGET_CARVEINFO_H
#define WIDGET_CARVEINFO_H

#include <QWidget>
#include <QToolButton>

#include "ui_Widget_CarveInfo.h"
class Widget_CarveImage;

class Widget_CarveInfo : public QWidget
{
	Q_OBJECT

public:
	Widget_CarveInfo(QWidget *parent);
	~Widget_CarveInfo();

private slots:
	//void slots_craveImg();					//????
//	void slots_ShowActiveImage(int);		//??ʾ
signals:
	void signals_craveImg();					//????
public slots:
	void TrunCameraSet();
	void TrunImageSet();
private:
	QToolButton *toolButtonToCamera;
	QToolButton *toolButtonToImage;

public:
	Ui::Widget_CarveInfo ui;

    Widget_CarveImage* pParent;
    
};

#endif // WIDGET_CARVEINFO_H
