#include "Widget_CarveImage.h"

#include <QMouseEvent>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <math.h>
#include <QMessageBox>

#include "clogfile.h"
#include "Widget_CarveSetting.h"
#include "widget_test.h"
#include "ILDRSystem.h"
extern SysMainUI *pMainFrm;

#define OPERATEDISTANCE 60
Widget_CarveImage::Widget_CarveImage(QWidget *parent)
	: QWidget(nullptr)
    , m_prent((WidgetCarveSetting*)parent)
{
	fCamScale = 1;
	fLastCamScale = 1;
	iIsTestGrey = 0;
	bIsTestMode = false;
	iTriggerDelay = 0;
	zeroPoint.setX(0);
	zeroPoint.setY(0);

	pWidgetCarveInfo = new Widget_CarveInfo(this);

	pWidgetCarveInfo->ui.widget_CameraInfo->setVisible(true);
	pWidgetCarveInfo->ui.widget_GrayTest->setVisible(false);
	pWidgetCarveInfo->ui.comboBox_triggerType->setCurrentIndex(1);
	pCurveGray = new QwtPlotCurve(tr("Gray"));
	pCurveGray->setPen(QPen(QColor(21,233,50)));
	pCurveGray->setRenderHint(QwtPlotItem::RenderAntialiased,true);
	pCurveGray->attach(pWidgetCarveInfo->ui.qwtPlot_sharpness);

	/*pCurveSarpness = new QwtPlotCurve(tr("Sarpness"));
	pCurveSarpness->setPen(QPen(QColor(233,21,50)));
	pCurveSarpness->setRenderHint(QwtPlotItem::RenderAntialiased,true);
	pCurveSarpness->attach(pWidgetCarveInfo->ui.qwtPlot_sharpness);*/

	pCurveReferenceLineA = new QwtPlotCurve();
	pCurveReferenceLineA->setPen(QPen(QColor(0,0,255)));
	pCurveReferenceLineA->setRenderHint(QwtPlotItem::RenderAntialiased,false);
	pCurveReferenceLineA->attach(pWidgetCarveInfo->ui.qwtPlot_sharpness);
	
	pCurveReferenceLineB = new QwtPlotCurve();
	pCurveReferenceLineB->setPen(QPen(QColor(0,0,255)));
	pCurveReferenceLineB->setRenderHint(QwtPlotItem::RenderAntialiased,false);
	pCurveReferenceLineB->attach(pWidgetCarveInfo->ui.qwtPlot_sharpness);

	pCurveReferenceLineC = new QwtPlotCurve(QString::fromLocal8Bit("参考线"));
	pCurveReferenceLineC->setPen(QPen(QColor(0,0,255)));
	pCurveReferenceLineC->setRenderHint(QwtPlotItem::RenderAntialiased,false);
	pCurveReferenceLineC->attach(pWidgetCarveInfo->ui.qwtPlot_sharpness);

	pWidgetCarveInfo->ui.qwtPlot_sharpness->insertLegend(new QwtLegend(),QwtPlot::BottomLegend); 

	m_eStatus = DRAWED;
	m_eDrawStatus = DRAW_Status_NULL;

	pBaseWidget = new DHBaseWidget(this);
	pBaseWidget->setWidgetName(tr("The original picture"));

	verticalLayout = new QVBoxLayout(pBaseWidget);
	verticalLayout->setSpacing(5);
	verticalLayout->setContentsMargins(5,0,5,5);
	verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

	timerTest = new QTimer(this);
	connect(timerTest, SIGNAL(timeout()), this, SLOT(slots_timerTest()));   
	timerUpdateGrayInfo = new QTimer(this);
	timerUpdateGrayInfo->setInterval(500);
	connect(timerUpdateGrayInfo, SIGNAL(timeout()), this, SLOT(slots_UpdateGrayInfo()));
}

Widget_CarveImage::~Widget_CarveImage()
{
	if (NULL != pImageShown)
	{
		delete pImageShown;
		pImageShown = NULL;
	}
	ReleaseWidget();
}

void Widget_CarveImage::init(int iCamNo)
{
	iCameraNo = iCamNo;
	int iRealCameraSN =	pMainFrm->m_sCarvedCamInfo[iCameraNo].m_iToRealCamera;

	pImageShown = new QImage(pMainFrm->m_sRealCamInfo[iCamNo].m_iImageWidth,pMainFrm->m_sRealCamInfo[iCamNo].m_iImageHeight,pMainFrm->m_sRealCamInfo[iCamNo].m_iImageBitCount);
	pImageShown->setColorTable(pMainFrm->m_vcolorTable);
	maxPoint.setX(pMainFrm->m_sRealCamInfo[iCamNo].m_iImageWidth);
	maxPoint.setY(pMainFrm->m_sRealCamInfo[iCamNo].m_iImageHeight);

	topPoint = QPoint(pMainFrm->m_sCarvedCamInfo[iCamNo].i_ImageX ,pMainFrm->m_sCarvedCamInfo[iCamNo].i_ImageY);
	buttomPoint = QPoint(pMainFrm->m_sCarvedCamInfo[iCamNo].m_iImageWidth + topPointReal.x(), \
		pMainFrm->m_sCarvedCamInfo[iCamNo].m_iImageHeight + topPointReal.y());

	topPoint.setY(maxPoint.y() - topPoint.y()-pMainFrm->m_sCarvedCamInfo[iCamNo].m_iImageHeight);
	buttomPoint.setY(topPoint.y()+pMainFrm->m_sCarvedCamInfo[iCamNo].m_iImageHeight);

	m_HVPoint=maxPoint;

	pView = new CMyGraphicsView(pBaseWidget);
	pCamScene = new QGraphicsScene(pBaseWidget);
	pView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff) ;
	pView->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff ) ; 
	
	pCamGroup = new QGraphicsItemGroup();
	pBmpItem = new QGraphicsPixmapItem;
	pCamSNItem = new QGraphicsTextItem;
	pImgSNItem = new QGraphicsTextItem;
	pTimeItem = new QGraphicsTextItem;
	pResultItem = new QGraphicsTextItem;
	//pRectItem = new QGraphicsRectItem;
	pCarveRectItem = new QGraphicsRectItem;
	pVerticalLineItem = new QGraphicsLineItem;
	pHorizonLineItem1 = new QGraphicsLineItem;
	pHorizonLineItem2 = new QGraphicsLineItem;
	pHorizonLineItem3 = new QGraphicsLineItem;
	pHorizonLineItem4 = new QGraphicsLineItem;
	pHorizonLineItem5 = new QGraphicsLineItem;
	m_pHorizonV = new QGraphicsLineItem(pBmpItem);
	m_pHorizonH = new QGraphicsLineItem(pBmpItem);
	m_pHorizonV1 = new QGraphicsLineItem(pBmpItem);
	m_pHorizonV2 = new QGraphicsLineItem(pBmpItem);
	m_pHorizonH1 = new QGraphicsLineItem(pBmpItem);
	m_pHorizonH2 = new QGraphicsLineItem(pBmpItem);

	QPen rectPen(QBrush(QColor(255,0,0)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);
	pCarveRectItem->setPen(rectPen);
	pVerticalLineItem->setPen(rectPen);
	pHorizonLineItem1->setPen(rectPen);
	pHorizonLineItem2->setPen(rectPen);
	pHorizonLineItem3->setPen(rectPen);
	pHorizonLineItem4->setPen(rectPen);
	pHorizonLineItem5->setPen(rectPen);

	pCamGroup->addToGroup(pBmpItem);
	pCamGroup->addToGroup(pCamSNItem);
	pCamGroup->addToGroup(pImgSNItem);
	pCamGroup->addToGroup(pTimeItem);
	pCamGroup->addToGroup(pResultItem);
//	pCamGroup->addToGroup(pRectItem);
	pCamGroup->addToGroup(pCarveRectItem);
	pCamGroup->addToGroup(pVerticalLineItem);
	pCamGroup->addToGroup(pHorizonLineItem1);
	pCamGroup->addToGroup(pHorizonLineItem2);
	pCamGroup->addToGroup(pHorizonLineItem3);
	pCamGroup->addToGroup(pHorizonLineItem4);
	pCamGroup->addToGroup(pHorizonLineItem5);

	//相机序号
	pCamSNItem->setPos(5,5);
	pCamSNItem->setDefaultTextColor(QColor(10,100,200));
	pCamSNItem->setFont(QFont("Arial Black",8,QFont::Light));
	//图像号
	pImgSNItem->setPos(5,20);
	pImgSNItem->setDefaultTextColor(QColor(10,100,200));
	pImgSNItem->setFont(QFont("Arial Black",8,QFont::Light));
	//耗时
	pTimeItem->setPos(5,35);
	pTimeItem->setDefaultTextColor(QColor(10,100,200));
	pTimeItem->setFont(QFont("Arial Black",8,QFont::Light));			

	pResultItem->setPos(5,45);
	pResultItem->setDefaultTextColor(QColor(10,100,200));
	pResultItem->setFont(QFont("Arial Black",8,QFont::Light));			

	//添加到场景
	pCamScene->addItem(pCamGroup);
	pView->setScene(pCamScene);
 	pView->setMinimumHeight(200);
	QHBoxLayout *layoutTitle = new QHBoxLayout;
	layoutTitle->addWidget(pBaseWidget->widgetName);

	label_x = new QLabel;
	label_y = new QLabel;
	label_w = new QLabel;
	label_h = new QLabel;
	spinBox_X = new QSpinBox;
	spinBox_Y = new QSpinBox;
	spinBox_W = new QSpinBox;
	spinBox_H = new QSpinBox;
	spinBox_X->setMaximum(99999);
	spinBox_Y->setMaximum(99999);
	spinBox_W->setMaximum(99999);
	spinBox_H->setMaximum(99999);
	label_x->setText("X:");
	label_y->setText("Y:");
	label_w->setText("W:");
	label_h->setText("H:");
	spinBox_X->setSingleStep(4);
	spinBox_Y->setSingleStep(4);
	spinBox_W->setSingleStep(4);
	spinBox_H->setSingleStep(4);
	QHBoxLayout *layoutPosition = new QHBoxLayout;
	layoutPosition->addWidget(label_x);
	layoutPosition->addWidget(spinBox_X);
	layoutPosition->addWidget(label_y);
	layoutPosition->addWidget(spinBox_Y);
	layoutPosition->addWidget(label_w);
	layoutPosition->addWidget(spinBox_W);
	layoutPosition->addWidget(label_h);
	layoutPosition->addWidget(spinBox_H);
	
	verticalLayout->addLayout(layoutTitle);
	verticalLayout->addWidget(pView);
	verticalLayout->addLayout(layoutPosition);

	QVBoxLayout *main_layout = new QVBoxLayout(this);
	main_layout->addWidget(pBaseWidget);
	main_layout->addWidget(pWidgetCarveInfo);
	main_layout->setSpacing(6);
	main_layout->setContentsMargins(5, 5, 5, 5);
	
	setLayout(main_layout); 

	connect(pView,SIGNAL(signals_mouseReleaseEvent(QMouseEvent*)),this,SLOT(slots_mouseReleaseEvent(QMouseEvent*)));
	connect(pView,SIGNAL(signals_mousePressEvent(QMouseEvent*)),this,SLOT(slots_mousePressEvent(QMouseEvent*)));
	connect(pView,SIGNAL(signals_mouseMoveEvent(QMouseEvent*)),this,SLOT(slots_mouseMoveEvent(QMouseEvent*)));
	connect(pView,SIGNAL(signals_resizeEvent(QResizeEvent *)),this,SLOT(slots_resizeEvent(QResizeEvent *)));
	connect(pWidgetCarveInfo->ui.pushButton_cancel,SIGNAL(clicked()),this,SLOT(slots_cancel()));
	connect(pWidgetCarveInfo->ui.pushButton_carve,SIGNAL(clicked()),this,SLOT(slots_carve()));
	connect(pWidgetCarveInfo->ui.pushButton_save,SIGNAL(clicked()),this,SLOT(slots_save()));
	connect(pWidgetCarveInfo->ui.pushButton_copyROI,SIGNAL(clicked()),this,SLOT(slots_CopyROI()));
//	connect(pWidgetCarveInfo->ui.pushButton_restore,SIGNAL(clicked()),this,SLOT(slots_restore()));	
 	connect(pWidgetCarveInfo->ui.pushButton_up,SIGNAL(clicked()),this,SLOT(slots_up()));
	connect(pWidgetCarveInfo->ui.pushButton_down,SIGNAL(clicked()),this,SLOT(slots_down()));
	connect(pWidgetCarveInfo->ui.pushButton_left,SIGNAL(clicked()),this,SLOT(slots_left()));
	connect(pWidgetCarveInfo->ui.pushButton_right,SIGNAL(clicked()),this,SLOT(slots_right()));

	connect(pWidgetCarveInfo->ui.pushButton_grey,SIGNAL(clicked()),this,SLOT(slots_grey()));
	connect(pWidgetCarveInfo->ui.pushButton_startTest,SIGNAL(clicked()),this,SLOT(slots_startTest()));
	connect(pWidgetCarveInfo->ui.pushButton_setToCamera,SIGNAL(clicked()),this,SLOT(slots_setToCamera()));
	connect(pWidgetCarveInfo->ui.pushButton_readDelayPara,SIGNAL(clicked()),this,SLOT(slots_readDelayPara()));

	connect(pWidgetCarveInfo->ui.spinBox_exposureTime,SIGNAL(valueChanged(int)),this,SLOT(Slot_EnableSetBtn()));
	connect( pWidgetCarveInfo->ui.comboBox_triggerType,SIGNAL(currentIndexChanged(int)),this,SLOT(Slot_EnableSetBtn()));
	connect( pWidgetCarveInfo->ui.lineEdit_delay,SIGNAL(textChanged(QString)),this,SLOT(Slot_EnableSetBtn()));

	connect(spinBox_X,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_Y,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_W,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_H,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));

	QButtonGroup * buttonGroup  = new QButtonGroup(this);
	buttonGroup->addButton(pWidgetCarveInfo->ui.radioButton_Normal,0);
	buttonGroup->addButton(pWidgetCarveInfo->ui.radioButton_Stress,1);
	connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(slots_StressModeChanged(int)));

	pWidgetCarveInfo->ui.lineEdit_delay->setValidator(new QIntValidator(0, 50000, this));

	pWidgetCarveInfo->ui.spinBox_realNo->setMaximum(pMainFrm->m_sSystemInfo.iCamCount);
	pWidgetCarveInfo->ui.spinBox_realNo->setMinimum(1);

	pWidgetCarveInfo->ui.spinBox_toNormal->setMaximum(pMainFrm->m_sSystemInfo.iCamCount);
	pWidgetCarveInfo->ui.spinBox_toNormal->setMinimum(1);

	pWidgetCarveInfo->ui.spinBox_Ro->setMinimum(-15);
	pWidgetCarveInfo->ui.spinBox_Ro->setMaximum(15);

	pWidgetCarveInfo->ui.spinBox_realNo->setValue(iCameraNo+1);
	pWidgetCarveInfo->ui.spinBox_toNormal->setValue(pMainFrm->m_sCarvedCamInfo[iCameraNo].m_iToNormalCamera+1);
	pWidgetCarveInfo->ui.spinBox_Ro->setValue(pMainFrm->m_sCarvedCamInfo[iCameraNo].m_iImageAngle);
	
	pWidgetCarveInfo->ui.groupBox_11->setVisible(false);//微调
	pWidgetCarveInfo->ui.groupBox_Position->setVisible(false);//位置

	if(pMainFrm->m_sCarvedCamInfo[iCamNo].m_iStress)
	{
		pWidgetCarveInfo->ui.radioButton_Stress->setChecked(true);
	}else{
		pWidgetCarveInfo->ui.radioButton_Normal->setChecked(true);
	}
}
void Widget_CarveImage::ReleaseWidget()
{
	if (NULL != pCamGroup)
	{
		delete pCamGroup;
		pCamGroup = NULL;
	}
}
void Widget_CarveImage::intoWidget()
{
	disconnect(spinBox_X,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	disconnect(spinBox_Y,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	disconnect(spinBox_W,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	disconnect(spinBox_H,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));

	spinBox_X->setValue((int)topPoint.x());
	spinBox_Y->setValue((int)topPoint.y());
	spinBox_W->setValue((int)(buttomPoint.x()-topPoint.x()));
	spinBox_H->setValue((int)(buttomPoint.y()-topPoint.y()));

	connect(spinBox_X,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_Y,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_W,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_H,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));

	modifyRect();
}
void Widget_CarveImage::slots_resizeEvent(QResizeEvent *event)
{
	topPointReal = QPoint(pMainFrm->m_sCarvedCamInfo[iCameraNo].i_ImageX ,pMainFrm->m_sCarvedCamInfo[iCameraNo].i_ImageY);
	buttomPointReal = QPoint(pMainFrm->m_sCarvedCamInfo[iCameraNo].m_iImageWidth+topPointReal.x(),pMainFrm->m_sCarvedCamInfo[iCameraNo].m_iImageHeight+topPointReal.y());
	topPoint = topPointReal;
	buttomPoint = buttomPointReal;
	int tempRectWidth = buttomPointReal.x() - topPointReal.x();
	int tempRectHeight = buttomPointReal.y()-topPointReal.y();

	topPoint.setY(pMainFrm->m_sRealCamInfo[iCameraNo].m_iImageHeight - topPoint.y()-tempRectHeight);
	buttomPoint.setY(topPoint.y()+tempRectHeight);

	int pViewWidth = pView->geometry().width()-5;
	int pViewHeight = pView->geometry().height()-5;
	int imgWidth = pMainFrm->m_sRealCamInfo[iCameraNo].m_iImageWidth;
	int imgHeight = pMainFrm->m_sRealCamInfo[iCameraNo].m_iImageHeight;
	//int imgWidth = pImageShown->width();
	//int imgHeight = pImageShown->height();
	if (1.0*pViewWidth/imgWidth < 1.0*pViewHeight/imgHeight)
	{	
		fCamScale = 1.0*pViewWidth/imgWidth;	
	}
	else
	{
		fCamScale = 1.0*pViewHeight/imgHeight;
	}
	disconnect(spinBox_X,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	disconnect(spinBox_Y,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	disconnect(spinBox_W,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	disconnect(spinBox_H,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));

 	spinBox_X->setValue((int)topPoint.x());
 	spinBox_Y->setValue((int)topPoint.y());
 	spinBox_W->setValue((int)(buttomPoint.x()-topPoint.x()));
 	spinBox_H->setValue((int)(buttomPoint.y()-topPoint.y()));
	connect(spinBox_X,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_Y,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_W,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_H,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));

 	modifyRect();

	slots_updateActiveImg(iCameraNo);
}

int Widget_CarveImage::slots_carve()
{
	if (m_eStatus == CARVED)
	{
		ReturnToOriginal();
	}
	if (1 == iIsTestGrey)
	{
		QMessageBox::information(this,"TIPS",tr("Please change to carve mode!"));
		return 2;
	}
	CLogFile::write(tr("Carve image! "),OperationLog);
	pCarveRectItem->setVisible(false);

	int m_iRectWidth = (spinBox_W->value())/4*4;
	int m_iRectHeight = (spinBox_H->value())/4*4; 
	int i_ImageX = spinBox_X->value()/4*4;
	int i_ImageY = spinBox_Y->value()/4*4;
	
	iImageWidth = m_iRectWidth;
	iImageHeight = m_iRectHeight; 
	iImageX = i_ImageX;
	iImageY = i_ImageY;
	
	iImageY = pMainFrm->m_sRealCamInfo[iCameraNo].m_iImageHeight-iImageY-spinBox_H->value();

	//mQimageLock.lock();
	pMainFrm->m_sRealCamInfo[iCameraNo].m_mutexmShownImage.lock();
	if (NULL != pImageShown)
	{
		delete pImageShown;
		pImageShown = NULL;
	}

	pImageShown=new QImage(iImageWidth,	iImageHeight,8);// 用于实时显示

	pImageShown->setColorTable(pMainFrm->m_vcolorTable);
	//mQimageLock.unlock();
	pMainFrm->m_sRealCamInfo[iCameraNo].m_mutexmShownImage.unlock();
	m_eStatus =CARVED;
	slots_updateActiveImg(iCameraNo);
	modifyRect();
	return 0;
}
void Widget_CarveImage::ReturnToOriginal()
{
	if (1 == iIsTestGrey)
	{
		QMessageBox::information(this,"TIPS",tr("Please change to carve mode!"));
		return;
	}
	
	pCarveRectItem->setVisible(true);
	m_eStatus = DRAWED;
	pMainFrm->m_sRealCamInfo[iCameraNo].m_mutexmShownImage.lock();
	if (NULL != pImageShown)
	{
		delete pImageShown;
		pImageShown = NULL;
	}
	pImageShown = new QImage(pMainFrm->m_sRealCamInfo[iCameraNo].m_iImageWidth,pMainFrm->m_sRealCamInfo[iCameraNo].m_iImageHeight,pMainFrm->m_sRealCamInfo[iCameraNo].m_iImageBitCount);
	pImageShown->setColorTable(pMainFrm->m_vcolorTable);
	pMainFrm->m_sRealCamInfo[iCameraNo].m_mutexmShownImage.unlock();

	slots_updateActiveImg(iCameraNo);
	modifyRect();
	
	m_pHorizonV->setVisible(true);
	m_pHorizonV1->setVisible(true);
	m_pHorizonV2->setVisible(true);
	m_pHorizonH->setVisible(true);
	m_pHorizonH1->setVisible(true);
	m_pHorizonH2->setVisible(true);
	int heighttemp = maxPoint.y();
	int widthtemp = maxPoint.x();
	QPen rectPens(QBrush(QColor(0,0,255)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);
	m_pHorizonV->setPen(rectPens);
	m_pHorizonV1->setPen(rectPens);
	m_pHorizonV2->setPen(rectPens);
	m_pHorizonH1->setPen(rectPens);
	m_pHorizonH2->setPen(rectPens);

	m_pHorizonH->setPen(rectPens);
	m_pHorizonH->setLine(widthtemp/2,heighttemp,widthtemp/2,0); //竖线
	m_pHorizonH1->setLine((int)(widthtemp*0.25),heighttemp,(int)(widthtemp*0.25),0); //竖线
	m_pHorizonH2->setLine((int)(widthtemp*0.75),heighttemp,(int)(widthtemp*0.75),0); //竖线

	m_pHorizonV->setLine(0,heighttemp/2,widthtemp,heighttemp/2); //横线
	m_pHorizonV1->setLine(0,heighttemp/10,widthtemp,heighttemp/10); //横线
	m_pHorizonV2->setLine(0,heighttemp/10*9,widthtemp,heighttemp/10*9); //横线

}
void Widget_CarveImage::slots_cancel()
{
	ReturnToOriginal();

	if (1 == iIsTestGrey)
	{
		QMessageBox::information(this,"TIPS",tr("Please change to carve mode!"));
		return;
	}
	//注释掉获取当前前切大小的代码
	{
		int iRealCameraSN =	pMainFrm->m_sCarvedCamInfo[iCameraNo].m_iToRealCamera;
		topPointReal = QPoint(pMainFrm->m_sCarvedCamInfo[iCameraNo].i_ImageX ,pMainFrm->m_sCarvedCamInfo[iCameraNo].i_ImageY);
		buttomPointReal = QPoint(pMainFrm->m_sCarvedCamInfo[iCameraNo].m_iImageWidth + topPointReal.x(), \
			pMainFrm->m_sCarvedCamInfo[iCameraNo].m_iImageHeight + topPointReal.y());
		topPoint = topPointReal;
		buttomPoint = buttomPointReal;
		int iImageWidth = pMainFrm->m_sRealCamInfo[iRealCameraSN].m_iImageWidth;
		int iImageHeight = pMainFrm->m_sRealCamInfo[iRealCameraSN].m_iImageHeight;
		int tempRectWidth = pMainFrm->m_sCarvedCamInfo[iCameraNo].m_iImageWidth;
		int tempRectHeight = pMainFrm->m_sCarvedCamInfo[iCameraNo].m_iImageHeight;

		topPoint.setY(pMainFrm->m_sRealCamInfo[iRealCameraSN].m_iImageHeight - topPoint.y()-tempRectHeight);
		buttomPoint.setY(topPoint.y()+tempRectHeight);

		iImageX = topPoint.x();
		iImageY = topPoint.y();
		iImageWidth = tempRectWidth;
		iImageHeight = tempRectHeight;
			
		modifyRect();
		disconnect(spinBox_X,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
		disconnect(spinBox_Y,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
		disconnect(spinBox_W,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
		disconnect(spinBox_H,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));

		spinBox_X->setValue((int)topPoint.x());
		spinBox_Y->setValue((int)topPoint.y());
		spinBox_W->setValue((int)(buttomPoint.x()-topPoint.x()));
		spinBox_H->setValue((int)(buttomPoint.y()-topPoint.y()));
		connect(spinBox_X,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
		connect(spinBox_Y,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
		connect(spinBox_W,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
		connect(spinBox_H,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));

	}

}
void Widget_CarveImage::slots_save()
{
	//灰度测试
	if (1 == iIsTestGrey)
	{
		QMessageBox::information(this,"Error",tr("Please change to carve mode!"));
		return;
	}
	/*if(timerTest->isActive()||pMainFrm->m_sRunningInfo.m_bCheck)
	{
		QMessageBox::information(this,"Error",QString::fromLocal8Bit("请停止检测!"));
		return;
	}*/
	//保存是否为应力图像,此处应与检测线程互斥
	if (m_eStatus != CARVED&&m_eStatus != DRAWED)
	{
		return;
	}

	if (0 != slots_carve())
	{
		QMessageBox::information(this,tr("Error"),tr("Carve image failure！"));
		return;
	}

	QSettings iniCarveSet(pMainFrm->m_sConfigInfo.m_strGrabInfoPath,QSettings::IniFormat);
	QString strSession;
	strSession = QString("/Stress/Device_%1").arg(iCameraNo);
	if(pWidgetCarveInfo->ui.radioButton_Normal->isChecked())
	{
		iniCarveSet.setValue (strSession,0);
	}else{
		iniCarveSet.setValue (strSession,1);
	}
	strSession = QString("/pointx/Grab_%1").arg(iCameraNo);
	iniCarveSet.setValue (strSession,iImageX);
	strSession = QString("/pointy/Grab_%1").arg(iCameraNo);
	iniCarveSet.setValue (strSession,iImageY);
	strSession = QString("/width/Grab_%1").arg(iCameraNo);
	iniCarveSet.setValue (strSession,iImageWidth);
	strSession = QString("/height/Grab_%1").arg(iCameraNo);
	iniCarveSet.setValue (strSession,iImageHeight);
	strSession = QString("/convert/Grab_%1").arg(iCameraNo);
	iniCarveSet.setValue (strSession,pWidgetCarveInfo->ui.spinBox_realNo->value()-1);
	strSession = QString("/angle/Grab_%1").arg(iCameraNo);
	iniCarveSet.setValue (strSession,pWidgetCarveInfo->ui.spinBox_Ro->value());
	strSession = QString("/tonormal/Grab_%1").arg(iCameraNo);
	iniCarveSet.setValue (strSession,pWidgetCarveInfo->ui.spinBox_toNormal->value()-1);
	for(int i = 0; i < pMainFrm->m_sSystemInfo.iIOCardCount;i++)
    {
        QString sID = QString("/system/iIOCardOffSet%1").arg(i);
        if(i == 0)
        {
            sID = QString("/system/iIOCardOffSet");
        }
        iniCarveSet.setValue (sID, pMainFrm->widget_settings->iIOCardOffSet[i]);
    }
	
	m_prent->image_widget->slots_resetImgUpdate(iCameraNo, false);
		
    pMainFrm->InitCamImage(iCameraNo);
    m_prent->image_widget->slots_resetImgUpdate(iCameraNo, true);

	m_pHorizonH->setVisible(false);
	m_pHorizonH1->setVisible(false);
	m_pHorizonH2->setVisible(false);
	m_pHorizonV->setVisible(false);
	m_pHorizonV1->setVisible(false);
	m_pHorizonV2->setVisible(false);

}
void Widget_CarveImage::slots_CopyROI()
{
	/*for(int i=0; i<pMainFrm->m_sSystemInfo.iCamCount;i++)
	{
	Widget_CarveImage* widgetCarveImage = pMainFrm->widget_carveSetting->listWidgetCarveImage.at(i);
	widgetCarveImage->topPoint = topPoint;
	widgetCarveImage->buttomPoint = buttomPoint;
	}*/
}

void Widget_CarveImage::slots_mousePressEvent(QMouseEvent *event)
{
	disconnect(spinBox_X,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	disconnect(spinBox_Y,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	disconnect(spinBox_W,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	disconnect(spinBox_H,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));

	oldTopPoint = topPoint;
	oldButtomPoint = buttomPoint;
	if (pBmpItem->isUnderMouse()&&event->buttons() == Qt::LeftButton)
	{
	 	switch(m_eStatus)
	 	{
	 	case NORMAL:
	 		topPoint = QPoint(event->x(),event->y());//QCursor::pos();
			topPoint.setX((topPoint.x()-zeroPoint.x())/fCamScale);
			topPoint.setY((topPoint.y()-zeroPoint.y())/fCamScale);
	 		m_eStatus = DRAWING;
 			spinBox_X->setValue((int)topPoint.x());
 			spinBox_Y->setValue((int)topPoint.y());
 			spinBox_W->setValue((int)(buttomPoint.x()-topPoint.x()));
 			spinBox_H->setValue((int)(buttomPoint.y()-topPoint.y()));
	 		break;
	 	case DRAWING:
	 		break;
	 	case DRAWED:
			reSetDrawClieked(event);
			break;
		}
	}
}
void Widget_CarveImage::slots_mouseReleaseEvent(QMouseEvent *event)
{
	QPen rectPen(QBrush(QColor(255,0,0)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);
	switch(m_eStatus)
	{
	case NORMAL:
		break;
	case DRAWING:
		if(event->button() == Qt::LeftButton)
		{
	//		DrawShapeRelease(event);
		}
		m_eStatus = DRAWED;

		break;
	case DRAWED:
		switch(m_eDrawStatus)
		{
		case DRAW_Status_NULL:
			showMouse(event);
			break;
		case DRAW_Status_MOVE:
			moveRect(event);
// 			topPoint = tempPoint;
// 			buttomPoint = temPoint;
			break;
		case DRAW_Status_MODIFYLEFTTOP:
			break;
		case DRAW_Status_MODIFYLEFTBUTTOM:
			break;
		case DRAW_Status_MODIFYRIGHTTOP:
			break;
		case DRAW_Status_MODIFYRIGHTBUTTOM:
			break;
		}
		m_eDrawStatus = DRAW_Status_NULL;
			
		modifyRect(event,m_eDrawStatus);
		spinBox_X->setValue((int)topPoint.x());
		spinBox_Y->setValue((int)topPoint.y());
		spinBox_W->setValue((int)(buttomPoint.x()-topPoint.x()));
		spinBox_H->setValue((int)(buttomPoint.y()-topPoint.y()));	
	}
	connect(spinBox_X,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_Y,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_W,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_H,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));

}
void Widget_CarveImage::slots_mouseMoveEvent(QMouseEvent *event)
{
	//if (pBmpItem->isUnderMouse())//&&event->buttons() == Qt::LeftButton)
	//{
	disconnect(spinBox_X,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	disconnect(spinBox_Y,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	disconnect(spinBox_W,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	disconnect(spinBox_H,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));

		switch(m_eStatus)
		{
		case NORMAL:
			break;
		case DRAWING:
			//DrawShapeMove(event);
			buttomPoint = QPoint(event->x(),event->y());
			buttomPoint.setX((buttomPoint.x()-zeroPoint.x())/fCamScale);
			buttomPoint.setY((buttomPoint.y()-zeroPoint.y())/fCamScale);
			if (buttomPoint.x() < topPoint.x()+4)
			{
				buttomPoint.setX(topPoint.x()+4);
			}
			if (buttomPoint.y() < topPoint.y()+4)
			{
				buttomPoint.setY(topPoint.y()+4);
			}
			if (buttomPoint.y() > maxPoint.y())
			{
				buttomPoint.setY(maxPoint.y());
			}
			if (buttomPoint.x() > maxPoint.x())
			{
				buttomPoint.setX(maxPoint.x());
			}
			
			modifyRect();
			spinBox_X->setValue((int)topPoint.x());
			spinBox_Y->setValue((int)topPoint.y());
			spinBox_W->setValue((int)(buttomPoint.x()-topPoint.x()));
			spinBox_H->setValue((int)(buttomPoint.y()-topPoint.y()));

//			showRect();	

			break;
		case DRAWED:
			switch(m_eDrawStatus)
			{
			case DRAW_Status_NULL:
				showMouse(event);
				break;
			case DRAW_Status_MOVE:
				moveRect(event);
				break;
			case DRAW_Status_MODIFYLEFTTOP:
//				break;
			case DRAW_Status_MODIFYLEFTBUTTOM:
//				break;
			case DRAW_Status_MODIFYRIGHTTOP:
//				break;
			case DRAW_Status_MODIFYRIGHTBUTTOM:
				modifyRect(event,m_eDrawStatus);
				spinBox_X->setValue((int)topPoint.x());
				spinBox_Y->setValue((int)topPoint.y());
				spinBox_W->setValue((int)(buttomPoint.x()-topPoint.x()));
				spinBox_H->setValue((int)(buttomPoint.y()-topPoint.y()));

				break;
			}
			break;
		}
	//}
	spinBox_X->setValue((int)topPoint.x());
	spinBox_Y->setValue((int)topPoint.y());
	spinBox_W->setValue((int)(buttomPoint.x()-topPoint.x()));
	spinBox_H->setValue((int)(buttomPoint.y()-topPoint.y()));
	connect(spinBox_X,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_Y,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_W,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_H,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));


}
void Widget_CarveImage::showMouse(QMouseEvent *event)
{
	QPoint tempPoint = QPoint(event->x(),event->y());

	startPoint.setX((tempPoint.x()-zeroPoint.x())/fCamScale);
	startPoint.setY((tempPoint.y()-zeroPoint.y())/fCamScale);
	int nRectWidth = buttomPoint.x() - topPoint.x();
	int nRectHeight = buttomPoint.y() - topPoint.y();
	if(startPoint.x()>topPoint.x() - OPERATEDISTANCE && startPoint.y()>topPoint.y() - OPERATEDISTANCE && 
		startPoint.x()<topPoint.x() + nRectWidth/3 && startPoint.y()<topPoint.y()+ nRectHeight/3)
	{
		this->setCursor(Qt::CrossCursor);
	}else if(startPoint.x()>topPoint.x()-OPERATEDISTANCE && startPoint.y()>buttomPoint.y()-nRectHeight/3 &&
		startPoint.x()<topPoint.x() + nRectWidth/3 && startPoint.y()<buttomPoint.y() + OPERATEDISTANCE)
	{
		this->setCursor(Qt::CrossCursor);
	}else if(startPoint.x()>buttomPoint.x()-nRectWidth/3 && startPoint.y()>topPoint.y()-OPERATEDISTANCE &&
		startPoint.x()<buttomPoint.x()+OPERATEDISTANCE && startPoint.y()<topPoint.y()+nRectHeight/3)
	{
		this->setCursor(Qt::CrossCursor);
	}else if(startPoint.x()>buttomPoint.x()-nRectWidth/3 && startPoint.y()>buttomPoint.y()-nRectHeight/3 &&
		startPoint.x()<buttomPoint.x()+OPERATEDISTANCE && startPoint.y()<buttomPoint.y()+OPERATEDISTANCE)
	{
		this->setCursor(Qt::CrossCursor);
	}
	else if (startPoint.x()>topPoint.x() && startPoint.y()>topPoint.y() && 
		startPoint.x()<buttomPoint.x() && startPoint.y()<buttomPoint.y())
	{
		this->setCursor(Qt::OpenHandCursor);
	}
	else
	{
		this->setCursor(Qt::ArrowCursor);
	}

}
//修改剪切框点击事件
void Widget_CarveImage::reSetDrawClieked(QMouseEvent *event)
{
	tempPoint = QPoint(event->x(),event->y());
	int nRectWidth = buttomPoint.x() - topPoint.x();
	int nRectHeight = buttomPoint.y() - topPoint.y();
	startPoint.setX((tempPoint.x()-zeroPoint.x())/fCamScale);
	startPoint.setY((tempPoint.y()-zeroPoint.y())/fCamScale);

	if(startPoint.x()>topPoint.x() - OPERATEDISTANCE && startPoint.y()>topPoint.y() - OPERATEDISTANCE && 
		startPoint.x()<topPoint.x() + nRectWidth/3 && startPoint.y()<topPoint.y()+ nRectHeight/3)
	{
		m_eDrawStatus = DRAW_Status_MODIFYLEFTTOP;
		this->setCursor(Qt::CrossCursor);
	}else if(startPoint.x()>topPoint.x()-OPERATEDISTANCE && startPoint.y()>buttomPoint.y()-nRectHeight/3 &&
		startPoint.x()<topPoint.x() + nRectWidth/3 && startPoint.y()<buttomPoint.y() + OPERATEDISTANCE)
	{
		m_eDrawStatus = DRAW_Status_MODIFYLEFTBUTTOM;
		this->setCursor(Qt::CrossCursor);
	}else if(startPoint.x()>buttomPoint.x()-nRectWidth/3 && startPoint.y()>topPoint.y()-OPERATEDISTANCE &&
		startPoint.x()<buttomPoint.x()+OPERATEDISTANCE && startPoint.y()<topPoint.y()+nRectHeight/3)
	{
		m_eDrawStatus = DRAW_Status_MODIFYRIGHTTOP;
		this->setCursor(Qt::CrossCursor);
	}else if(startPoint.x()>buttomPoint.x()-nRectWidth/3 && startPoint.y()>buttomPoint.y()-nRectHeight/3 &&
		startPoint.x()<buttomPoint.x()+OPERATEDISTANCE && startPoint.y()<buttomPoint.y()+OPERATEDISTANCE)
	{
		m_eDrawStatus = DRAW_Status_MODIFYRIGHTBUTTOM;
		this->setCursor(Qt::CrossCursor);
	}else if (startPoint.x()>topPoint.x() && startPoint.y()>topPoint.y() && 
		startPoint.x()<buttomPoint.x() && startPoint.y()<buttomPoint.y())
	{
		m_eDrawStatus = DRAW_Status_MOVE;
		this->setCursor(Qt::ClosedHandCursor);
	}else 	
	{
		topPoint = QPoint(event->x(),event->y());//QCursor::pos();
 		topPoint.setX((topPoint.x()-zeroPoint.x())/fCamScale);
 		topPoint.setY((topPoint.y()-zeroPoint.y())/fCamScale);
		//topPoint.setX((topPoint.x())/fCamScale);
		//topPoint.setY((topPoint.y())/fCamScale);
		disconnect(spinBox_X,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
		disconnect(spinBox_Y,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
		disconnect(spinBox_W,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
		disconnect(spinBox_H,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));

		m_eStatus = DRAWING;
		spinBox_X->setValue((int)topPoint.x());
		spinBox_Y->setValue((int)topPoint.y());
		spinBox_W->setValue((int)(buttomPoint.x()-topPoint.x()));
		spinBox_H->setValue((int)(buttomPoint.y()-topPoint.y()));
		connect(spinBox_X,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
		connect(spinBox_Y,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
		connect(spinBox_W,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
		connect(spinBox_H,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));

	}
}
//移动剪切框事件
void Widget_CarveImage::moveRect(QMouseEvent *event)
{
	topPoint = QPoint(event->x(),event->y());

	stopPoint.setX((topPoint.x()-zeroPoint.x())/fCamScale);
	stopPoint.setY((topPoint.y()-zeroPoint.y())/fCamScale);

	topPoint.setX(oldTopPoint.x()+(stopPoint.x()-startPoint.x()));
	topPoint.setY(oldTopPoint.y()+(stopPoint.y()-startPoint.y()));
	buttomPoint.setX(oldButtomPoint.x()+(stopPoint.x()-startPoint.x()));
	buttomPoint.setY(oldButtomPoint.y()+(stopPoint.y()-startPoint.y()));

	if (topPoint.x() < 0)
	{
		topPoint.setX(0);
		buttomPoint.setX(oldButtomPoint.x()-oldTopPoint.x()); 
	}
	if (topPoint.y() < 0)
	{
		topPoint.setY(0);
		buttomPoint.setY(oldButtomPoint.y()-oldTopPoint.y()); 
	}
	if (buttomPoint.x() > maxPoint.x())
	{
		buttomPoint.setX(maxPoint.x());
		topPoint.setX(maxPoint.x()-oldButtomPoint.x()+oldTopPoint.x()); 
	}
	if (buttomPoint.y() > maxPoint.y())
	{
		buttomPoint.setY(maxPoint.y());
		topPoint.setY(maxPoint.y()-oldButtomPoint.y()+oldTopPoint.y()); 
	}
	modifyRect();
	disconnect(spinBox_X,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	disconnect(spinBox_Y,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	disconnect(spinBox_W,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	disconnect(spinBox_H,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));

	spinBox_X->setValue((int)topPoint.x());
	spinBox_Y->setValue((int)topPoint.y());
	spinBox_W->setValue((int)(buttomPoint.x()-topPoint.x()));
	spinBox_H->setValue((int)(buttomPoint.y()-topPoint.y()));
	connect(spinBox_X,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_Y,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_W,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_H,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
}
//修改剪切框移动事件
void Widget_CarveImage::modifyRect(QMouseEvent *event,e_DrawStatus m_eDrawStatus)
{
	QPoint tempPoint = QPoint(event->x(),event->y());

	stopPoint.setX((tempPoint.x()-zeroPoint.x())/fCamScale);
	stopPoint.setY((tempPoint.y()-zeroPoint.y())/fCamScale);
	switch(m_eDrawStatus)
	{
	case DRAW_Status_MODIFYLEFTTOP:
		topPoint = stopPoint;
		if (buttomPoint.x() < topPoint.x())
		{
			topPoint.setX(buttomPoint.x()-4);
		}
		if (buttomPoint.y() < topPoint.y())
		{
			topPoint.setY(buttomPoint.y()-4);
		}

		break;
	case DRAW_Status_MODIFYLEFTBUTTOM:
		topPoint.setX(stopPoint.x());
		buttomPoint.setY(stopPoint.y());
		if (buttomPoint.x() < topPoint.x())
		{
			topPoint.setX(buttomPoint.x()-4);
		}
		if (buttomPoint.y() < topPoint.y())
		{
			buttomPoint.setY(topPoint.y()+4);
		}

		break;
	case DRAW_Status_MODIFYRIGHTTOP:
		buttomPoint.setX(stopPoint.x());
		topPoint.setY(stopPoint.y());
		if (buttomPoint.x() < topPoint.x())
		{
			buttomPoint.setX(topPoint.x()+4);
		}
		if (buttomPoint.y() < topPoint.y())
		{
			topPoint.setY(buttomPoint.y()-4);
		}

		break;
	case DRAW_Status_MODIFYRIGHTBUTTOM:
		buttomPoint = stopPoint;
		if (buttomPoint.x() < topPoint.x())
		{
			buttomPoint.setX(topPoint.x()+4);
		}
		if (buttomPoint.y() < topPoint.y())
		{
			buttomPoint.setY(topPoint.y()+4);
		}

		break;
	}

	if (topPoint.x() < 0)
	{
		topPoint.setX(0);
	}
	if (topPoint.y() < 0)
	{
		topPoint.setY(0);
	}


	if (buttomPoint.x() > maxPoint.x())
	{
		buttomPoint.setX(maxPoint.x());
	}
	if (buttomPoint.y() > maxPoint.y())
	{
		buttomPoint.setY(maxPoint.y());
	}

	modifyRect();
	disconnect(spinBox_X,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	disconnect(spinBox_Y,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	disconnect(spinBox_W,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	disconnect(spinBox_H,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));

	spinBox_X->setValue((int)topPoint.x());
	spinBox_Y->setValue((int)topPoint.y());
	spinBox_W->setValue((int)(buttomPoint.x()-topPoint.x()));
	spinBox_H->setValue((int)(buttomPoint.y()-topPoint.y()));
	connect(spinBox_X,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_Y,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_W,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));
	connect(spinBox_H,SIGNAL(valueChanged(int)),this,SLOT(slots_spinBoxValueChanged()));

	//showRect();	

}

void Widget_CarveImage::slots_spinBoxValueChanged()
{
// 	spinBox_X->setValue((spinBox_X->value()/4)*4);
// 	spinBox_Y->setValue((spinBox_Y->value()/4)*4);
// 	spinBox_W->setValue((spinBox_W->value()/4)*4);
// 	spinBox_H->setValue((spinBox_H->value()/4)*4);

	topPoint.setX(spinBox_X->value()/4*4);
	topPoint.setY(spinBox_Y->value()/4*4);

	buttomPoint.setX(spinBox_X->value()/4*4+spinBox_W->value()/4*4);
	buttomPoint.setY(spinBox_Y->value()/4*4+spinBox_H->value()/4*4);
	modifyRect();

}

void Widget_CarveImage::slots_up()
{
	topPoint.setY(topPoint.y()-4);
	buttomPoint.setY(buttomPoint.y()-4);
	modifyRect();
}
void Widget_CarveImage::slots_down()
{
	topPoint.setY(topPoint.y()+4);
	buttomPoint.setY(buttomPoint.y()+4);
	modifyRect();
}
void Widget_CarveImage::slots_left()
{
	topPoint.setX(topPoint.x()-4);
	buttomPoint.setX(buttomPoint.x()-4);
	modifyRect();
}
void Widget_CarveImage::slots_right()
{
	topPoint.setX(topPoint.x()+4);
	buttomPoint.setX(buttomPoint.x()+4);
	modifyRect();
}
//旋转图像
bool Widget_CarveImage::RoAngle(uchar* pRes,uchar* pTar,int iResWidth,int iResHeight,int iAngle)
{
	int iTarWidth;
	int iTarHeight;
	if(pRes == NULL || iResWidth == 0 || iResHeight == 0)
	{
		return FALSE;
	}
	if (iAngle == 90)
	{
		iTarWidth = iResHeight;
		iTarHeight = iResWidth;
		for (int i=0;i<iResHeight;i++)
		{
			for (int j=0;j<iResWidth;j++) 
			{
				*(pTar+j*iTarWidth+(iTarWidth-i-1)) = *(pRes+i*iResWidth+j);
			}
		}
	}
	if (iAngle == 270)
	{	
		iTarWidth = iResHeight;
		iTarHeight = iResWidth;
		for (int i=0;i<iResHeight;i++)
		{
			for (int j=0;j<iResWidth;j++) 
			{
				*(pTar+(iTarHeight-j-1)*iTarWidth+i) = *(pRes+i*iResWidth+j);
			}
		}
	}
	if (iAngle == 180)
	{
		iTarWidth = iResWidth;
		iTarHeight = iResHeight;
		for (int i=0;i<iResHeight;i++)
		{
			for (int j=0;j<iResWidth;j++) 
			{
				*(pTar+(iTarHeight-i-1)*iTarWidth+(iTarWidth-j-1))=*(pRes+i*iResWidth+j);
			}
		}
	}
	return TRUE;

}
void Widget_CarveImage::CarveImage(uchar* pRes,uchar* pTar,int iResWidth,int iResHeight,int iTarX,int iTarY,int iTarWidth,int iTarHeight)
{
	try
	{
		uchar* pTemp = pTar;
		uchar* pTempRes = pRes+iResWidth*(iTarY)+iTarX;
		for(int i = 0; i < iTarHeight; i++)
		{
			memcpy(pTemp,pTempRes,iTarWidth);		
			pTemp += iTarWidth;
			pTempRes += iResWidth;
		}
	}
	catch(...)
	{
		CLogFile::write(tr("Error in image Carve!"),AbnormityLog);
	}
}
double Widget_CarveImage::AverageGray(uchar* pRes,int iResWidth,int iResHeight,int iTarX,int iTarY,int iTarWidth,int iTarHeight)
{
	try
	{
		iTarY = iResHeight-iTarY-iTarHeight;
		double dSum = 0;
		double dAverageGray = 0;
		uchar* pTempRes = pRes+iResWidth*(iTarY)+iTarX;
		for(int i = 0; i < iTarHeight; i++)
		{
			for (int j = 0;j < iTarWidth;j++)
			{
				dSum += *(pTempRes+j);
			}
//			pTemp += iTarWidth;
			pTempRes += iResWidth;
		}
		dAverageGray = dSum/(iTarWidth*iTarHeight);

		return dAverageGray;
	}
	catch(...)
	{
		CLogFile::write(tr("Error in image Carve!"),AbnormityLog);
		return 0;
	}
}

//*功能：计算边缘跨度
double Widget_CarveImage::EdgeSpan(uchar* pRes,int iResWidth,int iResHeight,int iTarX,int iTarY,int iTarWidth,int iTarHeight)
{
	iTarY = iResHeight-iTarY-iTarHeight;

	//int *GrayValue = new int[iTarWidth];
	//int *testWidth = new int[iTarWidth];
	QVector <double> GrayValue;
	QVector <double> SharpValue;
	QVector <double> testWidth;
	QVector <double> ReferenceLineA;
	QVector <double> ReferenceLineB;
	QVector <double> ReferenceLineC;
	double dMaxSlope = 0;
	double dSharp = 0;
	if ((iTarWidth)<10)
	{
		return -1;//矩形太窄
	}

	/*int middleLine = iTarY + iTarHeight/2;
	uchar* pTempRes = pRes+iResWidth*(middleLine)+iTarX;

	for (int i = 0;i<iTarWidth;i++)
	{
	GrayValue.push_back(*(pTempRes + i));

	if (i<iTarWidth-1)
	{
	dSharp = *(pTempRes + i+1)-*(pTempRes + i);
	SharpValue.push_back(fabs(dSharp));
	}
	testWidth.push_back(i);
	if (dMaxSlope < fabs(dSharp))
	{
	dMaxSlope = fabs(dSharp);
	}
	}*/
	int middleLine = iTarY ;//+ iTarHeight/2;
	uchar* pTempRes = pRes+iResWidth*(middleLine)+iTarX;

	
	for(int j=0;j<iTarHeight;j++)
	{
		int sumGray = 0;
		double sumSharp = 0;
		for (int i = 0;i<iTarWidth;i++)
		{
			sumGray += *(pTempRes + i);
		}
		for (int i = 0;i<iTarWidth-1;i++)
		{
			sumSharp += *(pTempRes + i+1)-*(pTempRes + i);
		}
		pTempRes += iResWidth;
		GrayValue.push_front((int)sumGray/iTarWidth);
		SharpValue.push_back(fabs(sumSharp/iTarWidth));
		ReferenceLineA.push_back(140);
		ReferenceLineB.push_back(170);
		ReferenceLineC.push_back(200);
		testWidth.push_back(j);

		if (dMaxSlope < fabs(sumSharp))
		{
			dMaxSlope = fabs(sumSharp);
		}
	}
	
	pCurveReferenceLineA->setSamples(ReferenceLineA,testWidth);
	pCurveReferenceLineB->setSamples(ReferenceLineB,testWidth);
	pCurveReferenceLineC->setSamples(ReferenceLineC,testWidth);
	pCurveGray->setSamples(GrayValue,testWidth);
	//pCurveSarpness->setSamples(SharpValue,testWidth);
	pWidgetCarveInfo->ui.qwtPlot_sharpness->setAxisScale(QwtPlot::yLeft,0,iTarHeight);
	pWidgetCarveInfo->ui.qwtPlot_sharpness->setAxisScale(QwtPlot::xBottom,0,256);
	pWidgetCarveInfo->ui.qwtPlot_sharpness->replot();
	return dMaxSlope;
}
void Widget_CarveImage::slots_StressModeChanged(int index)
{
	if (0 == index)
	{
		pWidgetCarveInfo->ui.label_toNormal->setEnabled(false);
		pWidgetCarveInfo->ui.spinBox_toNormal->setEnabled(false);
	}
	else if (1 == index)
	{
		pWidgetCarveInfo->ui.label_toNormal->setEnabled(true);
		pWidgetCarveInfo->ui.spinBox_toNormal->setEnabled(true);
	}
}
void Widget_CarveImage::slots_updateActiveImg(int nCamNo,int nImgNo,double dCostTime,int nResult)
{
	if (nCamNo != iCameraNo)
	{
		return;
	}
	if (isVisible())
	{
		if (m_eStatus ==CARVED)
		{
			pMainFrm->m_sRealCamInfo[iCameraNo].m_mutexmShownImage.lock();
			//mQimageLock.lock();
 			QImage tempImg = pMainFrm->m_sRealCamInfo[iCameraNo].m_pRealImage->copy ( iImageX, iImageY, iImageWidth, iImageHeight );
			memcpy(pImageShown->bits(),tempImg.bits(),iImageWidth*iImageHeight);
			/*if (pWidgetCarveInfo->ui.radioButton_Stress->isChecked())
			{
				if (1 == pMainFrm->m_sCarvedCamInfo[nCamNo].m_iImageType)
				{
					StressEnhance(pImageShown->bits(),pImageShown->bits(),iImageWidth,iImageHeight,pMainFrm->m_sSystemInfo.fPressScale);
				}
				else
				{
					StressEnhance(pImageShown->bits(),pImageShown->bits(),iImageWidth,iImageHeight,pMainFrm->m_sSystemInfo.fBasePressScale);
				}
			}*/
			//mQimageLock.unlock();
			pMainFrm->m_sRealCamInfo[iCameraNo].m_mutexmShownImage.unlock();
			if (0!=pWidgetCarveInfo->ui.spinBox_Ro->value())
			{
				s_AlgCInP sAlgCInp;						//检测输入参数
				s_AlgCheckResult *pAlgCheckResult;		//返回检测结果结构体

				sAlgCInp.sInputParam.nHeight = pImageShown->height();
				sAlgCInp.sInputParam.nWidth = pImageShown->width();
				sAlgCInp.sInputParam.nChannel = (pImageShown->depth()+7)/8;
				sAlgCInp.sInputParam.pcData = (char*)pImageShown->bits();

				sAlgCInp.nParam = pWidgetCarveInfo->ui.spinBox_Ro->value();

				pMainFrm->m_cBottleRotateCarve.Check(sAlgCInp, &pAlgCheckResult);
			}
 			*pImageShown = pImageShown->mirrored();
			pixmapShow = QPixmap::fromImage(*pImageShown);

			int pViewWidth = pView->geometry().width();
			int pViewHeight = pView->geometry().height();
			int imgWidth = pImageShown->width();
			int imgHeight = pImageShown->height();

			if ( 1.0*pViewHeight/imgHeight < 1.0*pViewWidth/imgWidth)
			{
				fCamScale = 1.0*pViewHeight/imgHeight;
			}
			else
			{
				fCamScale = 1.0*pViewWidth/imgWidth;
			}
			pCamScene->setSceneRect(QRect(0,0,pViewWidth,pViewHeight));
			pCamGroup->setPos((pViewWidth-fCamScale*imgWidth)/2,(pViewHeight-fCamScale*imgHeight)/2);

			pBmpItem->setScale(fCamScale);
			pBmpItem->setPixmap(pixmapShow);	
			pCamSNItem->setPlainText(tr("Camera %1").arg(nCamNo+1));	
		}
		else
		{
			pMainFrm->m_sRealCamInfo[iCameraNo].m_mutexmShownImage.lock();
			//mQimageLock.lock();
			int iImgWidth =pMainFrm->m_sRealCamInfo[iCameraNo].m_pRealImage->width();
			int	iImgHeight = pMainFrm->m_sRealCamInfo[iCameraNo].m_pRealImage->height();
			memcpy(pImageShown->bits(),pMainFrm->m_sRealCamInfo[iCameraNo].m_pRealImage->bits(),iImgWidth*iImgHeight);//pixmapShow = QPixmap::fromImage(tempImg);
			/*if (pWidgetCarveInfo->ui.radioButton_Stress->isChecked())
			{
			if (1 == pMainFrm->m_sCarvedCamInfo[nCamNo].m_iImageType)
			{
			StressEnhance(pImageShown->bits(),pImageShown->bits(),iImgWidth,iImgHeight,pMainFrm->m_sSystemInfo.fPressScale);
			}
			else
			{
			StressEnhance(pImageShown->bits(),pImageShown->bits(),iImgWidth,iImgHeight,pMainFrm->m_sSystemInfo.fBasePressScale);
			}
			}*/
			//mQimageLock.unlock();
			pMainFrm->m_sRealCamInfo[iCameraNo].m_mutexmShownImage.unlock();
			if (0!=pWidgetCarveInfo->ui.spinBox_Ro->value())
			{
				s_AlgCInP sAlgCInp;						//检测输入参数
				s_AlgCheckResult *pAlgCheckResult;		//返回检测结果结构体

				sAlgCInp.sInputParam.nChannel = (pImageShown->depth()+7)/8;
				sAlgCInp.sInputParam.nHeight = pImageShown->height();
				sAlgCInp.sInputParam.nWidth = pImageShown->width();
				sAlgCInp.sInputParam.pcData = (char*)pImageShown->bits();

				sAlgCInp.nParam = pWidgetCarveInfo->ui.spinBox_Ro->value();

				pMainFrm->m_cBottleRotateCarve.Check(sAlgCInp, &pAlgCheckResult);
			}
			*pImageShown = pImageShown->mirrored();
			pixmapShow = QPixmap::fromImage(*pImageShown);
		
			int pViewWidth = pView->geometry().width()-5;
			int pViewHeight = pView->geometry().height()-5;
			int imgWidth = pMainFrm->m_sRealCamInfo[iCameraNo].m_pRealImage->width();
			int imgHeight = pMainFrm->m_sRealCamInfo[iCameraNo].m_pRealImage->height();

			if (1.0*pViewWidth/imgWidth < 1.0*pViewHeight/imgHeight)
			{	
				fCamScale = 1.0*pViewWidth/imgWidth;
			}
			else
			{
				fCamScale = 1.0*pViewHeight/imgHeight;
			}

			pCamScene->setSceneRect(QRect(0,0,pViewWidth,pViewHeight));
			zeroPoint.setX((pViewWidth-fCamScale*imgWidth)/2);// - temPoint.x());
			zeroPoint.setY((pViewHeight-fCamScale*imgHeight)/2);// - temPoint.x());

			pCamGroup->setPos((pViewWidth-fCamScale*imgWidth)/2,(pViewHeight-fCamScale*imgHeight)/2);

			pBmpItem->setScale(fCamScale);
			pBmpItem->setPixmap(pixmapShow);	
			pCamSNItem->setPlainText(tr("Camera %1").arg(nCamNo+1));
		}
	}
}
bool Widget_CarveImage::StressEnhance(uchar* pRes,uchar* pTar,int iResWidth,int iResHeight,int iRatio)
{
	int temp;
	if(pRes == NULL || iResWidth == 0 || iResHeight == 0)
	{
		return FALSE;
	}
	for (int i=0;i<iResHeight*iResWidth;i++)
	{
		temp = (*(pRes+i))*(*(pRes+i));
		if (temp>255)
		{
			*(pTar+i) = 255;
		}
		else
		{
			*(pTar+i) = temp;
		}
	}
	return TRUE;
}
void Widget_CarveImage::modifyRect()
{
	if (topPoint.x() < 0)
	{
		topPoint.setX(0);
	}
	if (topPoint.y() < 0)
	{
		topPoint.setY(0);
	}
	if (buttomPoint.x() > maxPoint.x())
	{
		buttomPoint.setX(maxPoint.x());
	}
	if (buttomPoint.y() > maxPoint.y())
	{
		buttomPoint.setY(maxPoint.y());
	}
	showRect();	
}
void Widget_CarveImage::showRect()
{
	pCarveRectItem->setRect(topPoint.x()*fCamScale,topPoint.y()*fCamScale,\
		(buttomPoint.x()-topPoint.x())*fCamScale,(buttomPoint.y()-topPoint.y())*fCamScale);

	QPoint temptop;
	QPoint tempButtom;
	if (m_eStatus != CARVED)
	{
		temptop = topPoint;
		tempButtom = buttomPoint;
	}
	else
	{
		temptop = QPoint(0,0);
		tempButtom = QPoint(buttomPoint.x()-topPoint.x(),buttomPoint.y()-topPoint.y());

	}
	int x1 = (temptop.x()+(tempButtom.x()-temptop.x())/2)*fCamScale;
	int y1 = temptop.y()*fCamScale;
	int x2 = (temptop.x()+(tempButtom.x()-temptop.x())/2)*fCamScale;
	int y2 = tempButtom.y()*fCamScale;
	
	pVerticalLineItem->setLine(x1, y1, x1, y2);
	x1 = temptop.x()*fCamScale;
	y1 = temptop.y()*fCamScale;
	x2 = tempButtom.x()*fCamScale;
	y2 = tempButtom.y()*fCamScale; 
	pHorizonLineItem1->setLine(x1+(x2-x1)/4,	y1+(y2-y1)/6,	x1+(x2-x1)*3/4,	y1+(y2-y1)/6);
	pHorizonLineItem2->setLine(x1+(x2-x1)*3/8,	y1+(y2-y1)*2/6,	x1+(x2-x1)*5/8,	y1+(y2-y1)*2/6);
	pHorizonLineItem3->setLine(x1,				y1+(y2-y1)*3/6,	x2,				y1+(y2-y1)*3/6);
	pHorizonLineItem4->setLine(x1+(x2-x1)*3/8,	y1+(y2-y1)*4/6,	x1+(x2-x1)*5/8,	y1+(y2-y1)*4/6);
	pHorizonLineItem5->setLine(x1+(x2-x1)*3/8,	y1+(y2-y1)*5/6,	x1+(x2-x1)*5/8,	y1+(y2-y1)*5/6);

	int heighttemp = m_HVPoint.y();//spinBox_Y->text().toInt();
	int widthtemp = m_HVPoint.x();//spinBox_W->text().toInt();
	QPen rectPens(QBrush(QColor(0,0,255)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);
	m_pHorizonV->setPen(rectPens);
	m_pHorizonV1->setPen(rectPens);
	m_pHorizonV2->setPen(rectPens);
	m_pHorizonH1->setPen(rectPens);
	m_pHorizonH2->setPen(rectPens);
	m_pHorizonH->setPen(rectPens);

	m_pHorizonH->setLine(widthtemp/2,heighttemp,widthtemp/2,0); //竖线
	m_pHorizonH1->setLine((int)(widthtemp*0.25),heighttemp,(int)(widthtemp*0.25),0); //竖线
	m_pHorizonH2->setLine((int)(widthtemp*0.75),heighttemp,(int)(widthtemp*0.75),0); //竖线

	m_pHorizonV->setLine(0,heighttemp/2,widthtemp,heighttemp/2); //横线
	m_pHorizonV1->setLine(0,heighttemp/10,widthtemp,heighttemp/10); //横线
	m_pHorizonV2->setLine(0,heighttemp/10*9,widthtemp,heighttemp/10*9); //横线
}
//测试平均灰度
void Widget_CarveImage::slots_grey()
{
	int tempIsTestGrey = iIsTestGrey;
	iIsTestGrey = 0;
	slots_cancel();
	if (0 == tempIsTestGrey)
	{
		iIsTestGrey = 1;
		QPen rectPen(QBrush(QColor(0,255,0)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);
		pCarveRectItem->setPen(rectPen);
		pWidgetCarveInfo->ui.pushButton_grey->setText(tr("Go Back"));
		pWidgetCarveInfo->ui.widget_CameraInfo->setVisible(false);
		pWidgetCarveInfo->ui.widget_GrayTest->setVisible(true);
		timerUpdateGrayInfo->start();
	}
	else
	{
		iIsTestGrey = 0;
		QPen rectPen(QBrush(QColor(255,0,0)),2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);
		pCarveRectItem->setPen(rectPen);
		pWidgetCarveInfo->ui.pushButton_grey->setText(tr("test grey"));
		pWidgetCarveInfo->ui.widget_CameraInfo->setVisible(true);
		pWidgetCarveInfo->ui.widget_GrayTest->setVisible(false);
		timerUpdateGrayInfo->stop();
	}
}
//移到carveinfo中
void Widget_CarveImage::slots_setToCamera()
{
	//if(timerTest->isActive()||pMainFrm->m_sRunningInfo.m_bCheck)
	//{
	//	QMessageBox::information(this,"Error",QString::fromLocal8Bit("请停止检测!"));
	//	return;
	//}
	pWidgetCarveInfo->ui.pushButton_setToCamera->setEnabled(false);
	pWidgetCarveInfo->ui.pushButton_setToCamera_2->setEnabled(false);

	int iCameraSN =  pMainFrm->m_sCarvedCamInfo[iCameraNo].m_iToRealCamera;
	int iShuterTime = pWidgetCarveInfo->ui.spinBox_exposureTime->text().toInt();
	int iTriggerType = pWidgetCarveInfo->ui.comboBox_triggerType->currentIndex();

	pMainFrm->mutexDetectElement[iCameraSN].lock();
	pMainFrm->m_sRealCamInfo[iCameraSN].m_iShuter = iShuterTime;
	pMainFrm->m_sRealCamInfo[iCameraSN].m_iTrigger = iTriggerType;
	if(pMainFrm->m_sRealCamInfo[iCameraSN].m_iGrabType == 0)
	{
		((CDHGrabberSG*)pMainFrm->m_sRealCamInfo[iCameraSN].m_pGrabber)->SGSetParamInt(SGGrabSpeed,iShuterTime);
	}
	else if(pMainFrm->m_sRealCamInfo[iCameraSN].m_iGrabType == 8)
	{

		((CDHGrabberMER*)pMainFrm->m_sRealCamInfo[iCameraSN].m_pGrabber)->MERSetParamInt(MERExposure,iShuterTime);
	}else if(pMainFrm->m_sRealCamInfo[iCameraSN].m_iGrabType == 1)
	{
		((CDHGrabberAVT*)pMainFrm->m_sRealCamInfo[iCameraSN].m_pGrabber)->AVTSetParamPro(AVTShutter,iShuterTime);
	}	
	pMainFrm->mutexDetectElement[iCameraSN].unlock();

	QSettings iniCameraSet(pMainFrm->m_sConfigInfo.m_strGrabInfoPath,QSettings::IniFormat);
	QString strShuter,strTrigger;
	strShuter = QString("/Shuter/Grab_%1").arg(iCameraSN);
	strTrigger = QString("/Trigger/Grab_%1").arg(iCameraSN);
	iniCameraSet.setValue(strShuter,QString::number(iShuterTime));
	iniCameraSet.setValue(strTrigger,QString::number(iTriggerType));
	//设置延时参数
	if (pMainFrm->m_sSystemInfo.m_bIsIOCardOK)
	{
		iTriggerDelay = pWidgetCarveInfo->ui.lineEdit_delay->text().toInt();
		QString strValue,strPara;
		strValue = strValue.setNum(iTriggerDelay,10);
		int station = 0;
        int cardSN = pMainFrm->m_sRealCamInfo[iCameraSN].m_iIOCardSN;
        if(iCameraNo >= pMainFrm->m_sSystemInfo.iRealCamCount)
        {
            station = pMainFrm->struGrabCardPara[iCameraSN].iReserve2;
        }else{
            station = pMainFrm->struGrabCardPara[iCameraSN].iReserve1;
        }
        switch (station)
        {
        case 1:
            pMainFrm->m_vIOCard[cardSN]->writeParam(32,iTriggerDelay);
            strPara = strPara.setNum(32,10);
            break;
        case 2:
            pMainFrm->m_vIOCard[cardSN]->writeParam(61,iTriggerDelay);
            strPara = strPara.setNum(61,10);
            break;	
        case 3:
            pMainFrm->m_vIOCard[cardSN]->writeParam(57,iTriggerDelay);
            strPara = strPara.setNum(57,10);
            break;
        case 4:
            pMainFrm->m_vIOCard[cardSN]->writeParam(59,iTriggerDelay);
            strPara = strPara.setNum(59,10);
            break;
        case 5:
            pMainFrm->m_vIOCard[cardSN]->writeParam(157,iTriggerDelay);
            strPara = strPara.setNum(157,10);
            break;
        case 6:
            pMainFrm->m_vIOCard[cardSN]->writeParam(158,iTriggerDelay);
            strPara = strPara.setNum(158,10);
            break;
        }
        StateTool::WritePrivateProfileQString("PIO24B",strPara,strValue,pMainFrm->m_sSystemInfo.m_sConfigIOCardInfo[0].strCardInitFile);
	}
	CLogFile::write(tr("set camera%1 param,TriggerType:%2,ShuterTime:%3,TriggerDelay:%4!").arg(iCameraSN).arg(iTriggerType).arg(iShuterTime).arg(iTriggerDelay),OperationLog);
}
void Widget_CarveImage::slots_startTest()
{
	if (bIsTestMode)
	{
		timerTest->stop();
		bIsTestMode = false;
		pMainFrm->m_sSystemInfo.m_bIsTest = FALSE;
		pWidgetCarveInfo->ui.pushButton_startTest->setText(tr("Start Test"));

        pMainFrm->m_sRunningInfo.m_bCheck = false;
        emit signals_showPic(iCameraNo);
	}
	else
	{
		if (pMainFrm->m_sRunningInfo.m_bCheck)//如果正在检测
		{
			QMessageBox::information(this,tr("Error"),tr("Please stop detect!"));
			return;
		}
		int intercal = pWidgetCarveInfo->ui.spinBox_exposureTime->value()*2;
		if (intercal < 200)
		{
			intercal = 200;
		}
		timerTest->setInterval(intercal);
		timerTest->start();
		bIsTestMode = true;
		pMainFrm->m_sSystemInfo.m_bIsTest = TRUE;
        pMainFrm->m_sRunningInfo.iTestCamera = iCameraNo;
		pWidgetCarveInfo->ui.pushButton_startTest->setText(tr("Stop Test"));

		//增加点击开始测试时，实时更新相机显示图片
        int cardID = pMainFrm->m_sCarvedCamInfo[iCameraNo].m_iIOCardSN;
        pMainFrm->m_cCombine[cardID]->m_MutexCombin.lock();
        pMainFrm->m_cCombine[cardID]->RemovAllResult();
        pMainFrm->m_cCombine[cardID]->RemovAllError();
        pMainFrm->m_cCombine[cardID]->m_MutexCombin.unlock();

		pMainFrm->m_sRunningInfo.m_bCheck = true;
		emit signals_showPic(iCameraNo);
	}
}
void Widget_CarveImage::slots_timerTest()
{
	if (pMainFrm->m_sSystemInfo.m_bIsIOCardOK)
	{
        int CameraNo = pMainFrm->m_sCarvedCamInfo[iCameraNo].m_iToRealCamera;
        int cardID = pMainFrm->m_sCarvedCamInfo[iCameraNo].m_iIOCardSN;
		if(iCameraNo < pMainFrm->m_sSystemInfo.iRealCamCount)
		{
			pMainFrm->m_vIOCard[cardID]->TestOutPut(pMainFrm->struGrabCardPara[CameraNo].iReserve1);
		}else{
			pMainFrm->m_vIOCard[cardID]->TestOutPut(pMainFrm->struGrabCardPara[CameraNo].iReserve2);
		}
	}
}
void Widget_CarveImage::slots_readDelayPara()
{
	if (pMainFrm->m_sSystemInfo.m_bIsIOCardOK)
	{
        int iCameraSN =  pMainFrm->m_sCarvedCamInfo[iCameraNo].m_iToRealCamera;
        int station = pMainFrm->m_sRealCamInfo[iCameraSN].m_iGrabPosition;
        int cardSN = pMainFrm->m_sRealCamInfo[iCameraSN].m_iIOCardSN;
        switch (station)
        {
        case 1:
            iTriggerDelay = pMainFrm->m_vIOCard[cardSN]->readParam(32);
            break;
        case 2:
            iTriggerDelay = pMainFrm->m_vIOCard[cardSN]->readParam(61);
            break;	
        case 3:
            iTriggerDelay = pMainFrm->m_vIOCard[cardSN]->readParam(57);
            break;
        case 4:
            iTriggerDelay = pMainFrm->m_vIOCard[cardSN]->readParam(59);
            break;
        case 5:
            iTriggerDelay = pMainFrm->m_vIOCard[cardSN]->readParam(157);
            break;
        case 6:
            iTriggerDelay = pMainFrm->m_vIOCard[cardSN]->readParam(158);
            break;
        }
	}
	pWidgetCarveInfo->ui.lineEdit_delay->setText(QString::number(iTriggerDelay));
}
void Widget_CarveImage::slots_UpdateGrayInfo()
{
	int iRectWidth = (spinBox_W->value())/8*8;
	int iRectHeight = (spinBox_H->value())/8*8;
	int i_ImageX = spinBox_X->value();
	int i_ImageY = pMainFrm->m_sRealCamInfo[iCameraNo].m_iImageHeight-spinBox_Y->value()-spinBox_H->value();
	double dAverageGray = AverageGray(pImageShown->bits(),pImageShown->width(),pImageShown->height(),i_ImageX,i_ImageY,iRectWidth,iRectHeight);
	pWidgetCarveInfo->ui.lineEdit_gray->setText(QString::number(dAverageGray));
	double iEdgeSpan = EdgeSpan(pImageShown->bits(),pImageShown->width(),pImageShown->height(),i_ImageX,i_ImageY,iRectWidth,iRectHeight);
	pWidgetCarveInfo->ui.lineEdit_sharpness->setText(QString::number(iEdgeSpan));
}



void Widget_CarveImage::Slot_EnableSetBtn()
{
	pWidgetCarveInfo->ui.pushButton_setToCamera->setEnabled(true);
	pWidgetCarveInfo->ui.pushButton_setToCamera_2->setEnabled(true);
}

