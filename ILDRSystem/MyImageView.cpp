#include "MyImageView.h"
#include "widget_image.h"
#include <QMouseEvent>
#include <QFileDialog>
#include "glasswaredetectsystem.h"

extern GlasswareDetectSystem *pMainFrm;

CMyImageView::CMyImageView(QWidget *parent)
	: QGraphicsView(parent)
{
	nResizeType = 0;
	nItemID = -1;
	createActions();
}

CMyImageView::~CMyImageView() 
{
	contextMenu->clear(); //清除菜单
	delete contextMenu;
}
void CMyImageView::createActions() 
{
	contextMenu = new QMenu;
	saveAction = new QAction(tr("Save image"),this);
	connect(saveAction,SIGNAL(triggered()),this,SLOT(slots_saveImage()));
	stopCheck =  new QAction(tr("Stop check"),this);
	connect(stopCheck,SIGNAL(triggered()),this,SLOT(slots_stopCheck()));
	startCheck =  new QAction(tr("Start check"),this);
	connect(startCheck,SIGNAL(triggered()),this,SLOT(slots_startCheck()));
	startShow = new QAction(tr("Start show"),this);
	connect(startShow,SIGNAL(triggered()),this,SLOT(slots_startShow()));

	showCheck = new QAction(tr("Set algorithm"),this);
}
void CMyImageView::mouseDoubleClickEvent(QMouseEvent *event)
{
	pDlg = pMainFrm->carve_setting->image_widget;
	
	bool bValid = false;
	//判断按在哪个item上
	for (int i = 0;i<pMainFrm->m_sSystemInfo.iCamCount;++i)
	{
		if (pDlg->pBmpItem[i]->isUnderMouse() && pDlg->pBmpItem[i]->isVisible())//
		{			
			bValid = true;
			nItemID = i;
			break;
		}
	}
	if (!bValid)
	{
		return;
	}

	//缩放窗口
	if (nResizeType != 1)
	{

		nResizeType = 1;
		for (int i = 0;i<pMainFrm->m_sSystemInfo.iCamCount;++i)
		{
			if (i == nItemID)
			{
				continue;
			}
			pDlg->pCamGroup[i]->setVisible(false);			
		}	
		pDlg->pBmpItem[nItemID]->setPixmap(pDlg->pixmapShow[nItemID]);	
		pDlg->adaptView(nItemID);

	}
	else
	{
		pDlg->pBmpItem[nItemID]->setScale(1);

		nResizeType = 2;
//		pDlg->ui.imageView->setVisible(true);
		for (int i = 0;i<pMainFrm->m_sSystemInfo.iCamCount;++i)
		{
			pDlg->pCamGroup[i]->setVisible(true);
		}
		pDlg->adaptView();
	}
}

void CMyImageView::resizeEvent(QResizeEvent *event)
{
}

void CMyImageView::contextMenuEvent(QContextMenuEvent *event)
{
	pDlg = pMainFrm->carve_setting->image_widget;

	int i;
	bool bValid = false;
	//判断按在哪个item上
	for (i=0;i<pMainFrm->m_sSystemInfo.iCamCount;++i)
	{
		if (pDlg->pBmpItem[i]->isUnderMouse() && pDlg->pBmpItem[i]->isVisible())
		{			
			bValid = true;
			nItemID = i;
			break;
		}
	}
	if (!bValid)
	{
		return;
	}

	contextMenu->clear(); //清除原有菜单
	QPoint point = event->pos(); //得到窗口坐标
	contextMenu->addAction(saveAction);
	if (1 == pMainFrm->m_sRunningInfo.m_bIsCheck[nItemID])
	{
		contextMenu->addAction(stopCheck);
	}
	else
	{
		contextMenu->addAction(startCheck);
	}
	if (!pDlg->bIsShow[nItemID])
	{
		contextMenu->addAction(startShow);
	}
	contextMenu->addAction(showCheck);

	//菜单出现的位置为当前鼠标的位置
	contextMenu->exec(QCursor::pos());
	event->accept();

}

void CMyImageView::slots_saveImage()
{

	QTime time = QTime::currentTime();
	QDate date = QDate::currentDate();
	QString strImgPath = tr("SaveImage/");
	strImgPath = strImgPath+tr("Camera%1/").arg(nItemID+1);
	QString strFilePath = pMainFrm->m_sConfigInfo.m_strAppPath + strImgPath;
	
	//QString fileName2 = QFileDialog::getSaveFileName(this, tr("Save Image"),".\\SaveImage\\saveImg.bmp",tr("Images (*.bmp *.png *.jpg)"));
	QString fileName = strFilePath + QString("%1-%2-%3-%4%5%6.bmp").arg(date.year()).arg(date.month()).arg(date.day()).arg(time.hour()).arg(time.minute()).arg(time.second());
	QDir *dir = new QDir;
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
		pDlg->pBmpItem[nItemID]->pixmap().toImage().convertToFormat(QImage::Format_Indexed8,pMainFrm->m_vcolorTable).save(fileName);
	}
	else
		return;
}
void CMyImageView::slots_showCheck()
{
 	pMainFrm->ShowCheckSet(nItemID);
}
void CMyImageView::slots_stopCheck()
{
	pMainFrm->m_sRunningInfo.m_bIsCheck[nItemID] = 0;
	if (0 == pMainFrm->m_sCarvedCamInfo[nItemID].m_iIOCardSN)
	{
		pMainFrm->m_cCombine.SetCombineCamera(nItemID,false);
	}
	else if (1 == pMainFrm->m_sCarvedCamInfo[nItemID].m_iIOCardSN)
	{
		pMainFrm->m_cCombine1.SetCombineCamera(nItemID,false);
	}
}
void CMyImageView::slots_startCheck()
{
	pMainFrm->m_sRunningInfo.m_bIsCheck[nItemID] = 1;
	if (0 == pMainFrm->m_sCarvedCamInfo[nItemID].m_iIOCardSN)
	{
		pMainFrm->m_cCombine.SetCombineCamera(nItemID,true);
	}
	else if (1 == pMainFrm->m_sCarvedCamInfo[nItemID].m_iIOCardSN)
	{
		pMainFrm->m_cCombine1.SetCombineCamera(nItemID,true);
	}
}
void CMyImageView::slots_startShow()
{
	pDlg = pMainFrm->carve_setting->image_widget;
	pDlg->bIsShow[nItemID] = true;

}

int CMyImageView::getTriggerItemID()
{
	return nItemID;
}
