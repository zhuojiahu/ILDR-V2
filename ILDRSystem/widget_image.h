#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#pragma once 

#include <QWidget>
#include <QSplitter>
#include <QToolButton>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QPainter>
#include <QPen>
#include <QHBoxLayout>
#include <QEvent>
#include <QVBoxLayout>
#include <QSet>
#include "ui_image_widget.h"
#include "stateTool.h"
#include "myimageshowitem.h"

class UIOperation;

class ImageWidget : public QWidget
{
	Q_OBJECT

public:
    ImageWidget(const QList<int>& idLst, QWidget *parent);
	~ImageWidget();
	
	void initDialog();
	bool checkCamera();
	void showErrorCheck(int nItemID);
	Ui::ImageWidget ui;
protected:
	void resizeEvent(QResizeEvent *event);
public:
	bool bIsCarveWidgetShow;
signals:
	void signals_SetCameraStatus(int,int);
	void signals_showCarve();
	void signals_hideCarve();
    public slots:
        void showMaxImage(int);
	void slots_showErrorImage(QImage*, int, int,double, int, int, QList<QRect>, int);
	void slots_imageItemDoubleClick(int );
	void slots_addError(int nCamSN,int nSignalNo,int nErrorType);
	void slots_turnImage();
	void slots_showCarve();

	void slots_showStartRefresh(int);
	void slots_showPrevious(int);
	void slots_showFollowing(int);

	void slots_showCheck(int nItemID);
	void slots_stopCheck(int nItemID);
	void slots_stopAllStressCheck();
	void slots_startCheck(int nItemID);
	//void slots_startAllStressCheck();
	void slots_startShow(int nItemID);
	void slots_stopShow(int nItemID);
	void slots_startShowAll();
	void slots_showOnlyCamera(int cameraId);

    void slots_resetImgUpdate(int iCameraNo, bool st);
    bool slots_checkCameraShow(int iCameraNo);
    bool slots_checkCameraShowError(int iCameraNo);
public:
	QList<QImage *> ImageError;
	QList<Alg::s_AlgImageLocInfo> sAlgImageLocInfo;	

	QList<MyImageShowItem*> listImageShowItem;

	bool bIsStopAllStessCheck;
	QPushButton *buttonTurnImage;
	QPushButton *buttonShowCarve;
private:
    UIOperation* pParent;

	QWidget *widgetContent;
	QWidget *widgetContentStess;
	QGridLayout *gridLayoutImagePage1;
	QGridLayout *gridLayoutImagePage2;
	int minwidgetContentWidth;
	int iSpacing;
	int iImagePage;
 	int iShownMode;			//��ʾģʽ��-1:ȫ����ʾ��1~n����ʾ��Ӧ�������
	int iBmpItemWidth;
	int iBmpItemHeight;
	int widgetWidth;
	int widgetHeight;
	int iCamCount;
	QList<int> iImagePosition;

	QGridLayout *gridLayoutLeft;
	QGridLayout *gridLayoutMiddle;
	QGridLayout *gridLayoutRight;

    QList<int> listCamID;
    QMap<int, int> camIDIndex;
    QSet<int> page1CamIDs;

    QList<bool> bIsShow;//ָʾ��Ӧ����Ƿ�ˢ��ͼ��Ŀǰ������
    QList<bool> bIsShowErrorImage;//ָʾ��Ӧ����Ƿ�ˢ��ͼ��Ŀǰ�л����㷨ʱ���ڼ���Ƿ���ͼ��

    friend class CErrorImageList;
};

#endif //IMAGEWIDGET_H