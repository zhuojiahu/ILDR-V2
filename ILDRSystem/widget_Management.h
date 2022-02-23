#ifndef WIDGET_MANAGEMENT_H
#define WIDGET_MANAGEMENT_H

#include <QWidget>
#include <QPushButton>
#include <QFileInfoList>
#include <QLabel>
#include <QSpinBox>
#include "ui_widget_Management.h"
#include "widget_MyCellWidget.h"
class MyCameraPoButton : public QPushButton
{
	Q_OBJECT
public:
	MyCameraPoButton(QWidget *parent = 0);
	~MyCameraPoButton();
	int iNumber;//当前选择行
signals:
	void signals_showCameraPo(int);
public slots:
	void slots_showCameraPo();

};

class WidgetManagement : public QWidget
{
	Q_OBJECT

public:
	WidgetManagement(QWidget *parent = 0);
	~WidgetManagement();

public:	
	int iSelectRow;//当前选择行
	int iCurModelRow;//当前模板行数
	QFileInfoList dirList;
// 	QList <QPushButton *> listButtonCameraPo;
	QGridLayout *layoutCameraPo;
 	QList <QLabel *> listLabelCameraNo;
 	QList <QDoubleSpinBox *> listdSpinBoxCameraHeight;
 	QList <QDoubleSpinBox *> listdSpinBoxCameraAngle;
	void UpdateTable();
	void DeleteCate(QString strDirPath);
	void BackupCate(QString strDirPath,QString strSrcPath);
	bool leaveWidget();

	void initCameraPositionWidget();
	void intoCameraPositionWidget();
	void SeverAdd(QString);
	void SeverDelete(QString);
	void SeverSelect(QString);
signals:
	void signals_clearTable();
public slots:
	void slots_intoWidget();
	void slots_CameraPositionWidgetOK();
	void slots_CameraPositionWidgetCancel();

private slots:
	void slots_cellClicked(int row,int col);
	void slots_new();
	void slots_load(bool bCurModel = false);
	void slots_backup();
	void slots_delete();
	void slots_export();
	void slots_import();
	void slots_action_selectImage();
	void slots_showCameraPo(int);
public:
	WidgetMyCellWidget *cellWidget;
private:
	void SaveModelNeme(QString str);
private:
	Ui::widgetManagement ui;
};

#endif // WIDGET_MANAGEMENT_H
