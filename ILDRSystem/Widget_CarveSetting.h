#ifndef CARVE_SETTING_H
#define CARVE_SETTING_H

#include <QWidget>
#include <QStackedWidget>
#include <QToolButton>

#include "Widget_CarveImage.h"
#include "widget_image.h"
#include "cerrorimagelist.h"
#include "DHBaseWidget.h"
#include "ConfigInfo.h"

class myPushButton : public QPushButton
{
	Q_OBJECT

public:
	myPushButton(QWidget *parent = 0);
	~myPushButton();

};

class UIOperation : public QWidget
{
	Q_OBJECT

public:
	UIOperation(const QList<int>& idLst,QWidget *parent = 0);
	~UIOperation();
public slots:
	void slots_changeButtonMode();
	void slots_turnCameraPage(int index);
	void slots_showCarve();
	void slots_hideCarve();

signals:
    void signals_updateResult(BottleResult res);
    void signals_updateCount(int total, int reject, int read, int intime, int engraved);
    void signals_clear();
public:
    QList<int> listCameraID;                        //本界面包含的相机ID列表

	QHBoxLayout *main_layout;
	QSplitter *splitter;

	ImageWidget *image_widget;//对于图像显示界面中的图像界面模块

	QWidget *setWidget;//图像显示界面左侧的所有界面
	QWidget *listWidget;
	Widget_CarveImage *widgetCarveImage;//对应图像显示界面左上角的切割图片
	QStackedWidget *pStackedCarve;
	QButtonGroup *buttonGroupCamera;
	QToolButton *expandButton;
	CErrorImageList *errorList_widget;
	int iButtonMode;
	int iCameraNo;
	QList<Widget_CarveImage*> listWidgetCarveImage;

private:
// 	bool isShownCarveSet;
// 	bool isShownErrorList;
	QList<int> listSizeCarveSet;
	QList<int> listSplitter;

};

#endif // CARVE_SETTING_H
