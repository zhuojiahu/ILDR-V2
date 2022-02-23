#include "myimageshowitem.h"
#include <QLabel>
#include <QLayout>
#include <QDateTime>
// #include <QFile>
#include <QFileDialog>

#include "clogfile.h"
#include "widget_image.h"
#include "ILDRSystem.h"

extern SysMainUI *pMainFrm;

MyImageShowItem::MyImageShowItem(QWidget *parent)
	: QWidget(parent)
{
    pParent = (ImageWidget*)parent;

	bIsUpdateImage = true;
	bIsMaxShow = false;
	bIsCheck = true;

	colorRect = QColor(0,0,0);
	btnPrevious = new PushButton(this);
	btnFollowing = new PushButton(this);
	btnStartRefresh = new PushButton(this);
	btnPrevious->setVisible(false);
	btnFollowing->setVisible(false);
	btnStartRefresh->setVisible(false);

	connect(btnPrevious, SIGNAL(clicked()), this, SLOT(slots_showPrevious()));
	connect(btnFollowing, SIGNAL(clicked()), this, SLOT(slots_showFollowing()));
	connect(btnStartRefresh, SIGNAL(clicked()), this, SLOT(slots_showStartRefresh()));

	createActions();

	m_vcolorTable.clear();
	for (int i = 0; i < 256; i++)  
	{  
		m_vcolorTable.append(qRgb(i, i, i)); 
	} 
	timerErrorInfo = new QTimer(this);
	timerErrorInfo->setInterval(1000);
	connect(timerErrorInfo, SIGNAL(timeout()), this, SLOT(slots_clearErrorInfo()));  

	timerWarningInfo = new QTimer(this);
	timerWarningInfo->setInterval(1000);
	connect(timerWarningInfo, SIGNAL(timeout()), this, SLOT(slots_clearWarningInfo()));   

}

MyImageShowItem::~MyImageShowItem()
{

	contextMenu->clear(); //清除菜单
	delete contextMenu;
}
void MyImageShowItem::createActions()
{
	contextMenu = new QMenu();
	saveAction = new QAction(tr("Save image"),this);
	connect(saveAction,SIGNAL(triggered()),this,SLOT(slots_saveImage()));
	stopCheck =  new QAction(tr("Stop check"),this);
	connect(stopCheck,SIGNAL(triggered()),this,SLOT(slots_stopCheck()));
	stopAllStressCheck =  new QAction(tr("Stop All Stress check"),this);
	connect(stopAllStressCheck,SIGNAL(triggered()),this,SLOT(slots_stopAllStressCheck()));
	startCheck =  new QAction(tr("Start check"),this);
	connect(startCheck,SIGNAL(triggered()),this,SLOT(slots_startCheck()));
	startAllStressCheck =  new QAction(tr("Start All Stress check"),this);
	connect(startAllStressCheck,SIGNAL(triggered()),this,SLOT(slots_startAllStressCheck()));
//  	startShow = new QAction(tr("Start show"),this);
//  	connect(startShow,SIGNAL(triggered()),this,SLOT(slots_startShow()));
	startFreshAll = new QAction(tr("Start Refresh All Camera"),this);
	connect(startFreshAll,SIGNAL(triggered()),this,SLOT(slots_startShowAll()));

	showCheck = new QAction(tr("Set algorithm"),this);
	connect(showCheck,SIGNAL(triggered()),this,SLOT(slots_showCheck()));
}

void MyImageShowItem::inital(int nCamNo)
{
	iCameraNo = nCamNo;
	strCamera = "null";
	strImageSN = "null";
	strTime = "null";
	strResult = "null";
	update();

	btnPrevious->setPicName(QString(":/pushButton/previous")) ;
	btnFollowing->setPicName(QString(":/pushButton/following")) ;
	btnStartRefresh->setPicName(QString(":/pushButton/stopShow")) ;

	QHBoxLayout *layoutButton = new QHBoxLayout;
	layoutButton->addWidget(btnPrevious,0,Qt::AlignBottom);
	layoutButton->addWidget(btnFollowing,0,Qt::AlignBottom);
	layoutButton->addWidget(btnStartRefresh,0,Qt::AlignBottom);
	
	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->addLayout(layoutButton,0,0,1,1);
	mainLayout->setContentsMargins(1,1,1,1);
	setLayout(mainLayout);
}

void MyImageShowItem::enterEvent(QEvent *)
{
//  	QSize size(geometry().width(),geometry().height());
//  	setFixedSize(size);
	btnPrevious->setVisible(true);
	btnFollowing->setVisible(true);
	btnStartRefresh->setVisible(true);

}
void MyImageShowItem::leaveEvent(QEvent *)
{
	btnPrevious->setVisible(false);
	btnFollowing->setVisible(false);
	btnStartRefresh->setVisible(false);
//  	setMinimumSize(0,0);
//  	setMaximumSize(16777215,16777215);
}

void MyImageShowItem::paintEvent(QPaintEvent *event)
{
	//添加边框
	if (!pMainFrm->m_sSystemInfo.m_iImageStretch && !bIsMaxShow)
	{
		QPainterPath path;
		path.setFillRule(Qt::WindingFill);
		path.addRect(1, 1, this->width()-2*1, this->height()-2*1);

		QPainter painterRect(this);
		painterRect.setRenderHint(QPainter::Antialiasing, true);//消除锯齿
		painterRect.fillPath(path, QBrush(Qt::white));

		QColor color(0, 0, 0);
		painterRect.setPen(color);
		painterRect.drawPath(path);
	}

	if (imageForWidget.isNull())
	{
		return;
	}
	int widgetWidth = geometry().width()-4;
	int widgetHeight = geometry().height()-4;
	int iShowWidth = widgetWidth;
	int iShowHeight = widgetHeight;
	int iShowX = 0;
	int iShowY = 0;
	QWidget::paintEvent(event);
	QPainter painter(this);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::red);
// 	painter.drawPixmap(QRect(0, 0, widgetWidth, widgetHeight),pixmapShown);
	if (bIsMaxShow)
	{
		int imgwidth = imageForWidget.width();
		int imgheight = imageForWidget.height();
		if (1.0*widgetWidth/widgetHeight > 1.0*imgwidth/imgheight)
		{
			iShowWidth = 1.0*imageForWidget.width()/imageForWidget.height()*widgetHeight;
			iShowHeight = widgetHeight;
			iShowX = (widgetWidth-iShowWidth)/2;
			iShowY = 0;
			painter.drawImage(QRect(iShowX, iShowY, iShowWidth, iShowHeight),imageForWidget);
		}
		else
		{
			iShowWidth = widgetWidth;
			iShowHeight = 1.0*imageForWidget.height()/imageForWidget.width()*widgetWidth;
			iShowX = 0;
			iShowY = (widgetHeight-iShowHeight)/2;
			painter.drawImage(QRect(iShowX, iShowY, iShowWidth, iShowHeight),imageForWidget);
		}
	}
	else
	{
		if (pMainFrm->m_sSystemInfo.m_iImageStretch)
		{
			iShowWidth = widgetWidth;
			iShowHeight = widgetHeight;
			iShowX = 0;
			iShowY = 0;
			painter.drawImage(QRect(iShowX, iShowY, iShowWidth, iShowHeight),imageForWidget);
		}
		else
		{
			int imgwidth = imageForWidget.width();
			int imgheight = imageForWidget.height();

			if (1.0*widgetWidth/widgetHeight > 1.0*imgwidth/imgheight)
			{
				iShowWidth = 1.0*imageForWidget.width()/imageForWidget.height()*widgetHeight;
				iShowHeight = widgetHeight;
				iShowX = (widgetWidth-iShowWidth)/2 + 2;
				iShowY = 0 + 2;
				painter.drawImage(QRect(iShowX, iShowY, iShowWidth, iShowHeight),imageForWidget);
			}
			else
			{
				iShowWidth = widgetWidth;
				iShowHeight = 1.0*imageForWidget.height()/imageForWidget.width()*widgetWidth;
				iShowX = 0 + 2;
				iShowY = (widgetHeight-iShowHeight)/2 + 2;
				painter.drawImage(QRect(iShowX, iShowY, iShowWidth, iShowHeight),imageForWidget);
			}
		}
	}
	
	QFont font("宋体",9,QFont::DemiBold,false);
	QPen pen(Qt::blue);
	pen.setWidth(2);
	painter.setFont(font);
	painter.setPen(pen);
	painter.setBrush(Qt::NoBrush);
//	painter.drawText(0, 20, tr("Camera:")+QString::number(iCameraNo));
	painter.drawText(0, 20, tr("Camera:")+strCamera);
	painter.drawText(0, 40, tr("ImageSN:")+strImageSN);
	painter.drawText(0, 60, tr("CostTime:")+strTime);
	if (listErrorRect.length()>0)
	{
		pen.setColor(Qt::red);
		painter.setPen(pen);
	}
	painter.drawText(0, 80, tr("Result:")+strResult);

	// 瓶底模点，20180528，by wenfan
	/*int nMouldID = strMouldID.toInt();
	if(strCamera=="8")
	{
		if (nMouldID > 0 && nMouldID < 100)
		{
			painter.drawText(0, 100, tr("MouldID: ") + QString::number(nMouldID));
		}else if(-1 == nMouldID || nMouldID ==0 )
		{
			painter.drawText(0, 100, tr("MouldID: unknown"));
		}else{

		}
	}*/

	//淡蓝画笔
	pen.setColor(Qt::magenta);
	pen.setWidth(3);
	painter.setPen(pen);
	painter.setBrush(Qt::NoBrush);

	//输出警报信息
	if (bIsHaveWarning)
	{
		painter.drawText(0,  geometry().height()/2, geometry().width(), geometry().height()/2, Qt::AlignCenter|Qt::TextWordWrap, strWarning);
	}

	//输出状态信息
	if (!bIsCheck)
	{
		painter.drawText(0, 0, geometry().width(), 20, Qt::AlignRight, tr("Check Stoped"));
	}
	if (!bIsUpdateImage)
	{
		painter.drawText(0, 20, geometry().width(), 20, Qt::AlignRight, tr("Refresh Stoped"));
	}
	//红色画笔
	QFont font2("Arial",16,QFont::Bold);
	pen.setColor(Qt::red);
	pen.setWidth(3);
	painter.setFont(font2);
	painter.setPen(pen);
	painter.setBrush(Qt::NoBrush);
	//画矩形框
	double scaleWidth = 1.0*iShowWidth/iImageWidth;
	double scaleHeight = 1.0*iShowHeight/iImageHeight;

	for (int i = 0; i<listErrorRect.length(); i++)
	{
		QRect rect = listErrorRect.at(i);
		painter.drawRect(rect.left() * scaleWidth+iShowX, rect.top() * scaleHeight+iShowY, rect.width() * scaleWidth, rect.height() * scaleHeight);
	}

	//输出错误信息
	if (bIsHaveError)
	{
		painter.drawText(0, 0, geometry().width(), geometry().height(), Qt::AlignCenter|Qt::TextWordWrap, strError);
	}
	//QPen pen(colorRect);
	//pen.setWidth(3);
	//painter.setPen(pen);
	//for (int i = 1; i<=10; i++)
	//{
	//	painter.setPen(QColor(colorRect.red(), colorRect.green(), colorRect.blue(), i*15));
	//	painter.drawRect(-i,-i,widgetWidth+1,widgetWidth+1);
	//}
}
void MyImageShowItem::slots_clearErrorInfo()
{
	bIsHaveError = false;
	timerErrorInfo->stop();
	update();
}
void MyImageShowItem::slots_clearWarningInfo()
{
	bIsHaveWarning = false;
	timerWarningInfo->stop();
	update();
}

void MyImageShowItem::mouseDoubleClickEvent(QMouseEvent *event)
{	
	emit signals_imageItemDoubleClick(iCameraNo);
// 	setMinimumSize(0,0);
// 	setMaximumSize(16777215,16777215);

}
void MyImageShowItem::contextMenuEvent(QContextMenuEvent *event)
{
	contextMenu->clear(); //清除原有菜单
	QPoint point = event->pos(); //得到窗口坐标
	contextMenu->addAction(saveAction);
	if (bIsCheck)
	{
		if (pMainFrm->sPermission.iStartStop)
		{
			contextMenu->addAction(stopCheck);
		}
	}
	else
	{
		contextMenu->addAction(startCheck);
	}
	if (!pParent->bIsStopAllStessCheck)
	{
		if(pMainFrm->m_sRunningInfo.m_iPermission>0)
		{
			contextMenu->addAction(stopAllStressCheck);
		}
	}
	else
	{
		contextMenu->addAction(startAllStressCheck);
	}

	contextMenu->addAction(startFreshAll);
	if (pMainFrm->sPermission.iAlgSet)
	{
		contextMenu->addAction(showCheck);
	}

	//菜单出现的位置为当前鼠标的位置
	contextMenu->exec(QCursor::pos());
	event->accept();
}
void MyImageShowItem::updateImage(QImage* imageShown,QString camera, QString imageSN,QString time, QString result, QString mouldID, QList<QRect> listRect)
{
	try
	{
		imageForWidget = (imageShown)->mirrored();
		//imageForWidget = imageShown;
	}
	catch (...)
	{
		CLogFile::write(tr("----取出显示图像异常----"),AbnormityLog);
		return;
	}

	strCamera = camera;
	strImageSN = imageSN;
	strTime = time;
	strResult = result;
	strMouldID = mouldID;
	listErrorRect = listRect;
	iImageWidth = imageForWidget.width();
	iImageHeight = imageForWidget.height();
	repaint();

}
void MyImageShowItem::slots_updateImage(QImage* imageShown,QString camera, QString imageSN,QString time, QString result, QString mouldID, QList<QRect> listRect,int ImageCount)
{
	//总共有24个对象，每个对象的标识是icamerano，需要根据camera和ImageCount来确定是哪个icamerano刷新
	if(QString::number(iCameraNo) != camera)
	{
		return;
	}
	if (imageShown == NULL)
	{
		return;
	}
	if(bIsUpdateImage)
	{
		updateImage(imageShown,QString::number(iCameraNo+1),imageSN,time, result, mouldID, listRect);
	}
}
void MyImageShowItem::slots_update()
{
	update();
}

void MyImageShowItem::slots_showErrorInfo(QString error, int time, bool bError)
{
	strError = error;
	bIsHaveError = bError;
	update();
	if (0 != time)
	{
		timerErrorInfo->setInterval(time*1000);
		timerErrorInfo->start();
	}
}
void MyImageShowItem::slots_showWarningInfo(QString error, int time, bool bError)
{
	strWarning = error;
	bIsHaveWarning = bError;
	update();
	if (0 != time)
	{
		timerWarningInfo->setInterval(time*1000);
		timerWarningInfo->start();
	}
}

void MyImageShowItem::slots_saveImage()
{
	QTime time = QTime::currentTime();
	QDate date = QDate::currentDate();
	QString strImgPath = tr("SaveImage/");
	strImgPath = strImgPath+tr("Camera%1/").arg(iCameraNo+1);
	//QString strFilePath = pMainFrm->m_sConfigInfo.m_strAppPath + strImgPath;
	
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"),\
		".\\SaveImage\\" + QString("%1-%2-%3-%4%5%6.bmp").arg(date.year()).arg(date.month()).arg(date.day()).arg(time.hour()).arg(time.minute()).arg(time.second()),\
		tr("Images (*.bmp *.png *.jpg)"));
//	QString fileName = ".\\SaveImage\\" + QString("%1-%2-%3-%4%5%6.bmp").arg(date.year()).arg(date.month()).arg(date.day()).arg(time.hour()).arg(time.minute()).arg(time.second());
	QDir *dir = new QDir;
	QString strFilePath = fileName.left(fileName.findRev("\\")+1);
	if(!dir->exists(strFilePath))
	{
		bool ok = dir->mkpath(strFilePath);
	}
	dir=NULL;
	//delete dir;
	if (!fileName.isEmpty())
	{
/*		QImage *imgSave = new QImage(pDlg->pBmpItem[nItemID]->pixmap().toImage().convertToFormat(QImage::Format_Indexed8));
		imgSave->setColorTable(pMainFrm->m_vcolorTable);
		imgSave->save(fileName);*/
//		pixmapShown.toImage().convertToFormat(QImage::Format_Indexed8, m_vcolorTable).save(fileName);
		imageForWidget.save(fileName);
	}

}
void MyImageShowItem::slots_showCheck()
{
	emit signals_showCheck(iCameraNo);
}
void MyImageShowItem::slots_stopCheck()
{
	bIsCheck = false;
	emit signals_stopCheck(iCameraNo );
	update();
}
void MyImageShowItem::slots_stopAllStressCheck()
{
//	bIsCheck = false;
	emit signals_stopAllStressCheck();
	update();
}
void MyImageShowItem::slots_startAllStressCheck()
{
//	bIsCheck = false;
	emit signals_stopAllStressCheck();
	update();
}
void MyImageShowItem::slots_startCheck()
{
	bIsCheck = true;
	emit signals_startCheck(iCameraNo );
	update();

}
void MyImageShowItem::slots_startShow()
{
	bIsUpdateImage = true;
	update();
	btnStartRefresh->setPicName(QString(":/pushButton/stopShow")) ;

}
void MyImageShowItem::slots_stopShow()
{
	bIsUpdateImage = false;
	update();
	btnStartRefresh->setPicName(QString(":/pushButton/startShow")) ;

}

void MyImageShowItem::slots_showPrevious()
{
	emit signals_showPrevious(iCameraNo);
}
void MyImageShowItem::slots_showFollowing()
{
	emit signals_showFollowing(iCameraNo);
}
void MyImageShowItem::slots_showStartRefresh()
{
	if (bIsUpdateImage == false)
	{
		pParent->slots_startShow(iCameraNo);
		slots_startShow();	
		emit signals_showStartRefresh(iCameraNo);
	}
	else
	{
		pParent->slots_stopShow(iCameraNo);
		slots_stopShow();
	}
}
void MyImageShowItem::slots_startShowAll()
{
	emit signals_startShowAll();
	for (int i=0;i<CAMERA_MAX_COUNT;i++)
	{
		pMainFrm->m_SavePicture[i].pThat=NULL;
	}
}
void MyImageShowItem::setMaxShow(bool bSatus)
{
	bIsMaxShow = bSatus;
}
