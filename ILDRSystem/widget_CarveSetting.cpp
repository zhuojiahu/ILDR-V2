#include "Widget_CarveSetting.h"

#include <QScrollArea>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QVBoxLayout>

#include "uiOperInfo.h"
#include "ILDRSystem.h"
extern SysMainUI *pMainFrm;

myPushButton::myPushButton(QWidget *parent)
	: QPushButton(parent)
{

}
myPushButton::~myPushButton()
{

}

WidgetCarveSetting::WidgetCarveSetting(const QList<int>& idLst,QWidget *parent)
	: QWidget(parent)
    , listCameraID(idLst)
{
    widgetCarveImage = nullptr;
    if(listCameraID.isEmpty()){
        setVisible(false);
        setEnabled(false);
        return;
    }
    //裁剪区域
	pStackedCarve = new QStackedWidget(this);
    auto midwgt = new QWidget;
    //图像显示区域
	image_widget = new ImageWidget(listCameraID, this);
    //错误区域
	errorList_widget = new CErrorImageList(listCameraID, image_widget);
    //增加当前结果显示,和统计数据显示
    auto operInfo = new UIOperInfo;
    auto vlay = new QVBoxLayout;
    vlay->addWidget(image_widget);
    vlay->addWidget(operInfo);
    midwgt->setLayout(vlay);

	for (int i = 0;i<listCameraID.size();i++)
	{
        int camID = listCameraID[i];
		widgetCarveImage = new Widget_CarveImage(this);
		widgetCarveImage->init(camID);
		listWidgetCarveImage.append(widgetCarveImage);
		pStackedCarve->addWidget(widgetCarveImage);
		connect(widgetCarveImage,SIGNAL(signals_showPic(int)),image_widget,SLOT(slots_imageItemDoubleClick(int )));
		connect(widgetCarveImage,SIGNAL(signals_showPic(int)),image_widget,SLOT(slots_showOnlyCamera(int )));
		connect(pMainFrm->pdetthread[camID],SIGNAL(signals_updateActiveImg(int,int,double,int)),widgetCarveImage,SLOT(slots_updateActiveImg(int,int,double,int)));
	}
	buttonGroupCamera  = new QButtonGroup(this);
	QGridLayout *GridLayout = new QGridLayout;
	for (int i = 0;i<listCameraID.size();i++)
	{
		myPushButton *pushButton_carvePage = new myPushButton(this);
		pushButton_carvePage->setObjectName("toolButtonCamera");
		pushButton_carvePage->setText(QString::number(listCameraID[i]+1));
		pushButton_carvePage->setStyleSheet("background-color:gray");
		buttonGroupCamera->addButton(pushButton_carvePage,i);
		GridLayout->addWidget(pushButton_carvePage,i%2,i/2);
	}
	setWidget = new QWidget(this);
	setWidget->setMaximumWidth(400);
	QVBoxLayout* vLayout = new QVBoxLayout(setWidget);
	vLayout->addWidget(pStackedCarve);
	vLayout->addLayout(GridLayout);
	vLayout->setSpacing(0);
	vLayout->setContentsMargins(0, 0, 0, 0);
	
	listWidget = new QWidget(this);
	QScrollArea *scrollArea = new QScrollArea(listWidget);
	QVBoxLayout* vLayout1 = new QVBoxLayout(listWidget);
	vLayout1->addWidget(scrollArea);
	vLayout1->setSpacing(0);
	vLayout1->setContentsMargins(0,0,0,0  );
	QVBoxLayout* vLayout2 = new QVBoxLayout(scrollArea);
	vLayout2->addWidget(errorList_widget);
	vLayout2->setSpacing(0);
	vLayout2->setContentsMargins(5,5,5,5);	
	
	splitter = new QSplitter;
	splitter->addWidget(midwgt);
	splitter->addWidget(listWidget);
	main_layout = new QHBoxLayout();
	main_layout->addWidget(setWidget);
	main_layout->addWidget(splitter);
	main_layout->setSpacing(0);
	main_layout->setContentsMargins(0, 0, 0, 0);
	setLayout(main_layout); 
	connect(buttonGroupCamera,SIGNAL(buttonClicked(int)),this,SLOT(slots_turnCameraPage(int)));
	iButtonMode = 0;

	QDesktopWidget* desktopWidget = QApplication::desktop();
	QRect screenRect = desktopWidget->screenGeometry();
	int width = screenRect.width();

 	listSplitter.append((width-300));
 	listSplitter.append(300);
  	splitter->setSizes(listSplitter);

	setWidget->setVisible(false);

	connect(image_widget,SIGNAL(signals_showCarve()),this,SLOT(slots_showCarve()));
	connect(image_widget,SIGNAL(signals_hideCarve()),this,SLOT(slots_hideCarve()));

    connect(errorList_widget,SIGNAL(signals_showErrorImage(QImage*, int, int, double, int, int, QList<QRect>, int )),image_widget, SLOT(slots_showErrorImage(QImage*, int, int, double, int, int, QList<QRect>, int)));
	connect(errorList_widget,SIGNAL(signals_showMaxImage( int )),image_widget, SLOT(showMaxImage(int)));

    slots_turnCameraPage(0);
}

WidgetCarveSetting::~WidgetCarveSetting()
{
    if(widgetCarveImage != nullptr)
	    delete widgetCarveImage;
}
void WidgetCarveSetting::slots_changeButtonMode()
{
	if (0 == iButtonMode)
	{
		QPixmap pixmap(":/sysButton/hide");
		expandButton->setIcon(pixmap);

		pStackedCarve->setVisible(true);
		setLayout(main_layout);
		iButtonMode = 1;
	}
	else
	{
		QPixmap pixmap(":/sysButton/expand");
		expandButton->setIcon(pixmap);

		pStackedCarve->setVisible(false);
		setLayout(main_layout);
		iButtonMode = 0;
	}
}
void WidgetCarveSetting::slots_turnCameraPage(int index)
{
	Widget_CarveImage *tempCarveiamge = dynamic_cast<Widget_CarveImage *>(pStackedCarve->widget(pStackedCarve->currentIndex()));
	if (tempCarveiamge->bIsTestMode)
	{
		if (QMessageBox::Yes == QMessageBox::question(this,tr("tip"),
			tr("Testing,do you want to stop test?"),QMessageBox::Yes | QMessageBox::No))
		{
			tempCarveiamge->slots_startTest();
		}
		else{
			return;
		}
	}
    int camID = listCameraID[pStackedCarve->currentIndex()];
	buttonGroupCamera->button(pStackedCarve->currentIndex())->setStyleSheet("background-color:gray");
	buttonGroupCamera->button(pStackedCarve->currentIndex())->setText(QString("%1").arg(camID+1));
	
	pStackedCarve->setCurrentIndex(index);
    camID = listCameraID[index];
	buttonGroupCamera->button(index)->setText(QString("*%1").arg(camID+1));
	buttonGroupCamera->button(index)->setStyleSheet("background-color:green");

	//更新剪切信息
 	tempCarveiamge = dynamic_cast<Widget_CarveImage *>(pStackedCarve->widget(index));
	tempCarveiamge->slots_readDelayPara();
	Widget_CarveInfo *tempCarveInfo = tempCarveiamge->pWidgetCarveInfo;
	tempCarveInfo->ui.spinBox_realNo->setValue(pMainFrm->m_sCarvedCamInfo[camID].m_iToRealCamera+1);
	tempCarveInfo->ui.spinBox_Ro->setValue(pMainFrm->m_sCarvedCamInfo[camID].m_iImageAngle);

	//更新相机信息
	if (1 == pMainFrm->m_sCarvedCamInfo[camID].m_iTrigger)
	{
		tempCarveInfo->ui.comboBox_triggerType->setCurrentIndex(1);
	}
	else
	{
		tempCarveInfo->ui.comboBox_triggerType->setCurrentIndex(0);
	}
	iCameraNo = camID;
	//image_widget->showMaxImage(iCameraNo);
	tempCarveInfo->ui.spinBox_exposureTime->setValue(pMainFrm->m_sRealCamInfo[pMainFrm->m_sCarvedCamInfo[camID].m_iToRealCamera].m_iShuter);
	//tempCarveiamge->updateActiveImg(index);
	tempCarveiamge->intoWidget();
}

void WidgetCarveSetting::slots_showCarve()
{
	setWidget->setVisible(true);
}
void WidgetCarveSetting::slots_hideCarve()
{
	setWidget->setVisible(false);
}
