#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QSignalMapper>
#include <QVBoxLayout>
#include <QStackedWidget>
#include "pushButton.h"
#include "toolButton.h"
#include "common.h"

// #include "widget_system_setting.h"
#include "widget_cameraSet.h"
//#include "config_setting.h"
#include "widget_IOCardSet.h"
#include "Widget_CarveSetting.h"
#include "widget_Management.h"

//#include "ui_Setting_widget.h"

class SettingWidget : public QWidget
{
	Q_OBJECT

public:

	SettingWidget(QWidget *parent = 0);
	~SettingWidget();

	void AddToolName();
	void ShowCheckSet(int nCamIdx = 0,int signalNumber = 0);

private:
//	Ui::SettingWidget ui;
public:
	QStackedWidget *statked_setting;
//	SystemSetting *system_setting;				//系统设置
//	WidgetCameraSetting *widgetCameraSetting;	//相机设置
//	ConfigSetting *config_setting;				//配置设置
	IOCardSetting *IOCard_setting;				//IO卡设置
	WidgetCarveSetting *carve_setting;			//剪切设置
	WidgetManagement *widgetManagement;			//品种设置
	//QWidget *checkWidget;  
	//QWidget *checkWidget1;  
	//QWidget *checkWidget2;
	QList<ToolButton *> setButton_list;

signals:
//	void turnSetPage(int current_page);
	void signals_getShowInfo();
public slots:
	void turnSetPage(QString current_page);

};

#endif // SETTINGWIDGET_H
