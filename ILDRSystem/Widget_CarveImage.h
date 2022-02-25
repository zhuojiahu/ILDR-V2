#ifndef WIDGET_CARVEIMAGE_H
#define WIDGET_CARVEIMAGE_H

#include <QWidget>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSpinBox>
#include <QMutex>
#include <QTimer>
#include <qwt_plot_curve.h>

#include "Widget_CarveInfo.h"
#include "cmygraphicsview.h"
#include "rotemptybottle.h"
#include "DHBaseWidget.h"

class UIOperation;

class Widget_CarveImage : public QWidget
{
	Q_OBJECT

public:
	Widget_CarveImage(QWidget *parent = 0);
	~Widget_CarveImage();
	enum e_Status	    //当前状态
	{
		NORMAL = 0,				//正常
		DRAWING,					//形状绘制
		DRAWED,
		CARVED,
		OTHER,
	};
 	enum e_DrawStatus	
 	{
 		DRAW_Status_NULL = 0,			//无操作
 		DRAW_Status_MOVE,				//移动
		DRAW_Status_MODIFYLEFTTOP,				//修改
		DRAW_Status_MODIFYRIGHTTOP,				//修改
		DRAW_Status_MODIFYLEFTBUTTOM,				//修改
		DRAW_Status_MODIFYRIGHTBUTTOM,				//修改
  		//DRAW_Status_ZOOM,				//缩放
	};

public:
	DHBaseWidget *pBaseWidget;
	QVBoxLayout *verticalLayout;
	Widget_CarveInfo *pWidgetCarveInfo;
	bool bIsTestMode;

signals:
	void signals_craveImage();
	void signals_showPic(int);
public slots:
	void slots_mousePressEvent(QMouseEvent *event);
	void slots_mouseReleaseEvent(QMouseEvent *event);
	void slots_mouseMoveEvent(QMouseEvent *event);
	void slots_resizeEvent(QResizeEvent *event);
	void slots_updateActiveImg(int nCamNo,int nImgNo =0,double dCostTime = 0,int nResult = 0);
	void slots_readDelayPara();
	void slots_startTest();
	void slots_spinBoxValueChanged();

public slots:
	int slots_carve();
	void slots_CopyROI();
	void slots_cancel();
	void slots_save();
	void slots_up();
	void slots_down();
	void slots_left();
	void slots_right();
	void slots_StressModeChanged(int index);
	void slots_grey();
	void slots_setToCamera();
	void slots_timerTest();
	void slots_UpdateGrayInfo();
	void Slot_EnableSetBtn();
public:
	void init(int iCamNo);
	void intoWidget();
	void modifyRect();
	void showRect();
	void showMouse(QMouseEvent *event);
	void reSetDrawClieked(QMouseEvent *event);
	void moveRect(QMouseEvent *event);
	bool RoAngle(uchar* pRes,uchar* pTar,int iResWidth,int iResHeight,int iAngle);
	double AverageGray(uchar* pRes,int iResWidth,int iResHeight,int iTarX,int iTarY,int iTarWidth,int iTarHeight);
	double EdgeSpan(uchar* pRes,int iResWidth,int iResHeight,int iTarX,int iTarY,int iTarWidth,int iTarHeight);
	bool StressEnhance(uchar* pRes,uchar* pTar,int iResWidth,int iResHeight,int iRatio=1);

protected:
	void ReturnToOriginal();
	void ShowCarvedImage();
	void ShowOriginalImage();
	void CarveImage(uchar* pRes,uchar* pTar,int iResWidth,int iResHeight,int iTarX,int iTarY,int iTarWidth,int iTarHeight);
	void ReleaseWidget();
	void modifyRect(QMouseEvent *event,	e_DrawStatus m_eDrawStatus);
	QPoint topPoint;
	QPoint buttomPoint;

private:
    UIOperation* m_prent;

	QPoint zeroPoint;
	QPoint oldTopPoint;
	QPoint oldButtomPoint;
	QPoint topPointReal;
	QPoint buttomPointReal;
	QPoint startPoint;
	QPoint stopPoint;
	QPoint maxPoint;
	QPoint m_HVPoint;

	QPoint tempPoint;
	QPoint temPoint;
	
	

	int iImageWidth;
	int iImageHeight;
	int iImageX;
	int iImageY;

	int iLastImageWidth;
	int iLastImageHeight;
	int iLastImageX;
	int iLastImageY;

	int iTriggerDelay;

	e_Status m_eStatus;			//状态
	e_DrawStatus m_eDrawStatus;	//绘制状态
	double fCamScale;
	double fLastCamScale;


	CMyGraphicsView *pView;
	QGraphicsScene *pCamScene;
	QMutex mQimageLock;
	QImage *pImageShown;
	QPixmap pixmapShow ;

	QTimer *timerTest;
	QTimer *timerUpdateGrayInfo;

	QGraphicsItemGroup *pCamGroup;
	QGraphicsPixmapItem *pBmpItem;
	QGraphicsTextItem *pCamSNItem;
	QGraphicsTextItem *pImgSNItem;
	QGraphicsTextItem *pTimeItem;
	QGraphicsTextItem *pResultItem;
	//	QGraphicsRectItem *pRectItem;
	QGraphicsRectItem *pCarveRectItem;
	QGraphicsLineItem *pVerticalLineItem;
	QGraphicsLineItem *pHorizonLineItem1;
	QGraphicsLineItem *pHorizonLineItem2;
	QGraphicsLineItem *pHorizonLineItem3;
	QGraphicsLineItem *pHorizonLineItem4;
	QGraphicsLineItem *pHorizonLineItem5;

	QGraphicsLineItem *m_pHorizonV;
	QGraphicsLineItem *m_pHorizonH;
	QGraphicsLineItem *m_pHorizonH1;
	QGraphicsLineItem *m_pHorizonH2;
	QGraphicsLineItem *m_pHorizonV1;
	QGraphicsLineItem *m_pHorizonV2;

	QLabel	*label_x;
	QLabel	*label_y;
	QLabel	*label_w;
	QLabel	*label_h;
	QSpinBox *spinBox_X;
	QSpinBox *spinBox_Y;
	QSpinBox *spinBox_W;
	QSpinBox *spinBox_H;

	QwtPlotCurve *pCurveGray;
	QwtPlotCurve *pCurveSarpness;

	QwtPlotCurve *pCurveReferenceLineA;
	QwtPlotCurve *pCurveReferenceLineB;
	QwtPlotCurve *pCurveReferenceLineC;
public:
	int iIsTestGrey;
	int iCameraNo;
	int iRealCameraNo;

    friend class Widget_CarveInfo;
};

#endif // WIDGET_CARVEIMAGE_H