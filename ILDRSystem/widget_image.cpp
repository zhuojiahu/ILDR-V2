#include "widget_image.h"

#include "clogfile.h"
#include "Widget_CarveSetting.h"
#include "ILDRSystem.h"

extern SysMainUI *pMainFrm;

ImageWidget::ImageWidget(const QList<int>& idLst, QWidget *parent)
	: QWidget(parent)
    , listCamID(idLst)
{
	ui.setupUi(this);
    pParent = (WidgetCarveSetting*)parent;

	iSpacing = 10;
	iShownMode = -1;
	iImagePage = 0;
	bIsCarveWidgetShow = false;
	bIsStopAllStessCheck = false;
	for (int i = 0; i < listCamID.size(); i++)
	{
		bIsShow.append(true);
		bIsShowErrorImage.append(false);
		iImagePosition.append( -1);
        ImageError.append(NULL) ;
        sAlgImageLocInfo.append(s_AlgImageLocInfo());
        sAlgImageLocInfo.last().sXldPoint.nColsAry = new int[BOTTLEXLD_POINTNUM];
        sAlgImageLocInfo.last().sXldPoint.nRowsAry = new int[BOTTLEXLD_POINTNUM];
        camIDIndex.insert(listCamID[i], i);
	}
	initDialog();
    checkCamera();
    if(idLst.isEmpty())
    {
        setVisible(false);
        setEnabled(false);
        return;
    }
}
ImageWidget::~ImageWidget()
{
    for (int i = 0; i < listCamID.size(); i++)
    {
        delete[] sAlgImageLocInfo[i].sXldPoint.nColsAry;
        delete[] sAlgImageLocInfo[i].sXldPoint.nRowsAry;
        if (ImageError[i] != NULL)
        {
            delete ImageError[i];
            ImageError[i] = NULL;
        }
    }
}
void ImageWidget::resizeEvent(QResizeEvent *event)
{
	widgetWidth = ui.scrollArea->geometry().width();
	widgetHeight = ui.scrollArea->geometry().height();

	int minItemHeight = (widgetHeight - 3*iSpacing)/2;
	minwidgetContentWidth = 0;
	for (int i = 0; i < listCamID.size(); i++)
	{
        int nCamID = listCamID[i];
		int minItemWidth;
		if (pMainFrm->m_sCarvedCamInfo[nCamID].m_iImageType == 0) //瓶身
		{
			minItemWidth = minItemHeight/4;
		}
		else if (pMainFrm->m_sCarvedCamInfo[nCamID].m_iImageType == 1)
		{
			minItemWidth = minItemHeight/2;
		}
		else if (pMainFrm->m_sCarvedCamInfo[nCamID].m_iImageType == 2)
		{
			minItemWidth = minItemHeight/4;
		}
		MyImageShowItem *imageShowItem = listImageShowItem.at(i);
		imageShowItem->setMinimumSize(minItemWidth,minItemHeight);
		if (0 == nCamID%2 && nCamID < pMainFrm->m_sSystemInfo.iRealCamCount)
		{
			minwidgetContentWidth += (iSpacing + minItemWidth);
		}
		else
		{
			//minwidgetContentStessWidth += (iSpacing + minItemWidth);
		}
	}
	widgetContent->setMinimumSize(minwidgetContentWidth + iSpacing, widgetHeight);
	widgetContentStess->setMinimumSize(minwidgetContentWidth + iSpacing, widgetHeight);
}

void ImageWidget::initDialog()
{
	iCamCount = pMainFrm->m_sSystemInfo.iCamCount;
	buttonTurnImage = new QPushButton;
	QPixmap iconTurnImage(":/sysButton/turnImage");
	buttonTurnImage->setIcon(iconTurnImage);
	buttonTurnImage->setFixedSize(iconTurnImage.size());
	connect(buttonTurnImage, SIGNAL(clicked()), this, SLOT(slots_turnImage()));
	buttonShowCarve = new QPushButton;
	QPixmap iconShowCarve(":/sysButton/arrowright");
	buttonShowCarve->setIcon(iconShowCarve);
	buttonShowCarve->setFixedSize(iconTurnImage.size());
	connect(buttonShowCarve, SIGNAL(clicked()), this, SLOT(slots_showCarve()));
	ui.widget->setWidgetName(tr("Image")+tr(":Normal"));
	ui.LayoutName->addWidget(buttonShowCarve);
	ui.LayoutName->addWidget(ui.widget->widgetName);
	ui.LayoutName->addStretch();
	ui.LayoutName->addWidget(buttonTurnImage);

	QWidget *WidgetScrollArea = new QWidget();
	widgetContent = new QWidget(WidgetScrollArea);
	widgetContentStess = new QWidget(WidgetScrollArea);
	ui.scrollArea->setWidget(WidgetScrollArea);

	QHBoxLayout *Contentlayout = new QHBoxLayout(WidgetScrollArea);
	Contentlayout->addWidget(widgetContent);
	Contentlayout->addWidget(widgetContentStess);
	Contentlayout->setSpacing(0);
	Contentlayout->setContentsMargins(0,0,0,0);
	for (int i = 0; i < listCamID.size(); i++)
	{
		MyImageShowItem *imageShowItem = new MyImageShowItem(this);
		imageShowItem->inital(listCamID[i]);
		listImageShowItem.append(imageShowItem);
    }

    //调整各相机的显示位置
    gridLayoutImagePage1 = new QGridLayout(widgetContent);
    gridLayoutImagePage2 = new QGridLayout(widgetContentStess);
    //遍历相机ID,将正常相机和应力相机分开.
    QList<int> normalCamIDs, stressCamIDs;
    for(int i = 0; i < listCamID.size(); i++)
    {
        if(listCamID[i] >= pMainFrm->m_sSystemInfo.iRealCamCount)
        {//已遍历完非条纹光相机
            break;
        }
        if(pMainFrm->m_sCarvedCamInfo[listCamID[i]].m_iStress == 0)
        {//正常相机
            normalCamIDs.append(listCamID[i]);
        }
        else //if(pMainFrm->m_sCarvedCamInfo[listCamID[i]].m_iStress == 1)
        {//应力相机
            stressCamIDs.append(listCamID[i]);
        }
    }
    int nBotCamCnt = 2;//一个瓶子有几个相机
    if(listCamID.size() <= 6)
    {//小于6个相机，则都显示到界面1，排列顺序就按照ID顺序
        page1CamIDs = listCamID.toSet();
        for(int i = 0; i < listCamID.size();i++)
        {
            int CamID = listCamID[i];
            //插入真实相机窗体
            int r = i%nBotCamCnt;
            int c = i/nBotCamCnt;
            gridLayoutImagePage1->addWidget(listImageShowItem[camIDIndex[CamID]], r, c,1,1);
        }
    }
    else
    {
        //第一页显示正常相机的图像,第二页显示应力相机的图像
        {//支持任意条纹光数量相机
            //从上到下,从左至右,先1个瓶子的正常图片,然后对应这两个相机的条纹光图像,另一页也是这样
            //相机ID要求显示正常相机,然后是应力相机.
            int rCnt = normalCamIDs.size();
            int sCnt = stressCamIDs.size();
            int StripeCamCnt = listCamID.size() - rCnt - sCnt;//条纹光个数
            page1CamIDs.unite(normalCamIDs.toSet());
            //插入第一页窗体
            int i = 0;
            int idx = 0;
            while(i < rCnt)
            {
                int CamID = normalCamIDs[i];
                //插入真实相机窗体
                int r = idx%nBotCamCnt;
                int c = idx/nBotCamCnt;
                gridLayoutImagePage1->addWidget(listImageShowItem[camIDIndex[CamID]], r, c,1,1);
                page1CamIDs.insert(CamID);
                i++;
                idx++;
                if(StripeCamCnt > 0 && ((i%nBotCamCnt)==0 || i == rCnt) )
                {//正常一个瓶子结束,开始插入条纹图像
                    int startID = i;
                    if(startID == rCnt && startID % nBotCamCnt != 0)
                    {
                        startID = (startID/nBotCamCnt)*nBotCamCnt;
                    }
                    else
                    {
                        startID = (startID/nBotCamCnt-1)*nBotCamCnt;
                    }
                    //需要插入之前未插入条纹窗体的条纹窗体
                    for(int j = startID;j < i && j < rCnt && j < StripeCamCnt ;j++,idx++)
                    {
                        CamID = normalCamIDs[j] + pMainFrm->m_sSystemInfo.iRealCamCount;
                        r = idx%nBotCamCnt;
                        c = idx/nBotCamCnt;
                        gridLayoutImagePage1->addWidget(listImageShowItem[camIDIndex[CamID] ], r, c,1,1);
                        page1CamIDs.insert(CamID);
                    }
                }
            }
            //插入第二页窗体
            i = 0;
            idx = 0;
            while(i < sCnt )
            {
                int CamID = stressCamIDs[i];
                //插入应力相机窗体
                int r = idx%nBotCamCnt;
                int c = idx/nBotCamCnt;
                gridLayoutImagePage2->addWidget(listImageShowItem[camIDIndex[CamID]], r, c,1,1);
                i++;
                idx++;
                if(StripeCamCnt > 0 && ( (i%nBotCamCnt) ==0 || i == sCnt) )
                {//正常一个瓶子结束,开始插入条纹图像
                    int startID = i;
                    if(startID == sCnt && startID % nBotCamCnt != 0)
                    {
                        startID = (startID/nBotCamCnt)*nBotCamCnt;
                    }
                    else
                    {
                        startID = (startID/nBotCamCnt-1)*nBotCamCnt;
                    }
                    //需要插入之前未插入条纹窗体的条纹窗体
                    for(int j = startID;j < i && j < sCnt && j < StripeCamCnt ;j++, idx++ )
                    {
                        CamID = stressCamIDs[j] + pMainFrm->m_sSystemInfo.iRealCamCount;
                        r = idx%nBotCamCnt;
                        c = idx/nBotCamCnt;
                        gridLayoutImagePage2->addWidget(listImageShowItem[camIDIndex[CamID]], r, c,1,1);
                    }
                }
            }
        }
    }
    if(page1CamIDs.size() == listCamID.size())
    {
        buttonTurnImage->setVisible(false);
    }
	
	widgetContentStess->setVisible(false);
	for (int i = 0;i<listCamID.size();i++)
	{
		MyImageShowItem *imageShowItem = listImageShowItem.at(i);
		connect(imageShowItem,SIGNAL(signals_imageItemDoubleClick(int ) ),this,SLOT(slots_imageItemDoubleClick(int )));
		connect(imageShowItem,SIGNAL(signals_showStartRefresh(int)),this,SLOT(slots_showStartRefresh(int)));
		connect(imageShowItem,SIGNAL(signals_showPrevious(int)),this,SLOT(slots_showPrevious(int)));
		connect(imageShowItem,SIGNAL(signals_showFollowing(int)),this,SLOT(slots_showFollowing(int)));
		connect(imageShowItem,SIGNAL(signals_showCheck(int)),this,SLOT(slots_showCheck(int)));
		connect(imageShowItem,SIGNAL(signals_stopCheck(int)),this,SLOT(slots_stopCheck(int)));
		connect(imageShowItem,SIGNAL(signals_stopAllStressCheck()),this,SLOT(slots_stopAllStressCheck()));
		connect(imageShowItem,SIGNAL(signals_startCheck(int)),this,SLOT(slots_startCheck(int)));
		connect(imageShowItem,SIGNAL(signals_startShow(int)),this,SLOT(slots_startShow(int)));
		connect(imageShowItem,SIGNAL(signals_stopShow(int)),this,SLOT(slots_stopShow(int)));
		connect(imageShowItem,SIGNAL(signals_startShowAll()),this,SLOT(slots_startShowAll()));
		connect(pMainFrm->pdetthread[listCamID[i]],SIGNAL(signals_updateImage(QImage*, QString, QString, QString ,QString, QString, QList<QRect>,int )),imageShowItem,SLOT(slots_updateImage(QImage*, QString , QString ,QString , QString, QString, QList<QRect>,int )));
		connect(pMainFrm->pdetthread[listCamID[i]], SIGNAL(signals_AddErrorTableView(int,int,int)), this, SLOT(slots_addError(int,int,int)));
	}
}
//检查对应相机是否正确初始化
bool ImageWidget::checkCamera()
{
	bool bRet = true;
	for (int i = 0; i < listCamID.size(); i++)
	{
		MyImageShowItem *imageShowItem = listImageShowItem.at(i);
		if (!pMainFrm->m_sRealCamInfo[listCamID[i]].m_bCameraInitSuccess)
		{
			imageShowItem->slots_showErrorInfo(pMainFrm->m_sRealCamInfo[listCamID[i]].m_strErrorInfo);
			bRet = false;
		}
		else
		{
			imageShowItem->slots_clearErrorInfo();
			emit signals_SetCameraStatus(listCamID[i],1);
		}
	}
	return true;
}
void ImageWidget::slots_turnImage()
{
	if (iShownMode != -1) //显示全部图像
	{
		slots_imageItemDoubleClick(iShownMode);
	}
	if (iImagePage == 0)
	{
		widgetContent->setVisible(false);
		widgetContentStess->setVisible(true);
        iImagePage = 1;
        ui.widget->setWidgetName(tr("Image")+tr(":Stress"));
	}
	else
	{
		widgetContentStess->setVisible(false);
		widgetContent->setVisible(true);
        iImagePage = 0;
        ui.widget->setWidgetName(tr("Image")+tr(":Normal"));
	}
}
void ImageWidget::slots_showCarve()
{
	if (!bIsCarveWidgetShow)
	{
		QPixmap iconShowCarve(":/sysButton/arrowLeft");
		buttonShowCarve->setIcon(iconShowCarve);
		emit signals_showCarve();
		bIsCarveWidgetShow = true;
	}
	else
	{
		QPixmap iconHideCarve(":/sysButton/arrowright");
		buttonShowCarve->setIcon(iconHideCarve);
		emit signals_hideCarve();
		bIsCarveWidgetShow = false;
	}
}

void ImageWidget::slots_imageItemDoubleClick(int iCameraNo)
{	
	if (iShownMode != -1) //显示全部图像
	{
		widgetContent->setMinimumWidth(minwidgetContentWidth);
		for (int i = 0; i < listCamID.size(); i++)
		{
			MyImageShowItem *imageShowItem = listImageShowItem.at(i);
			imageShowItem->setVisible(true);
			imageShowItem->setMaxShow(false);
		}
		iShownMode = -1;
	}else //只显示双击图像
	{
		widgetContent->setMinimumWidth(0);
		for (int i = 0; i < listCamID.size(); i++)
		{
			MyImageShowItem *imageShowItem = listImageShowItem.at(i);
			if (iCameraNo == listCamID[i])
			{
				imageShowItem->setVisible(true); 
				imageShowItem->setMaxShow(true);
			}
			else
			{
				if (iCameraNo != -1)
				{
					imageShowItem->setVisible(false);
				}
			}
		}
		iShownMode = iCameraNo;
	}
}
void ImageWidget::showMaxImage(int iCameraNo)
{
	/*if (iCameraNo >= pMainFrm->m_sSystemInfo.iRealCamCount)
	{
		widgetContent->setVisible(false);
		widgetContentStess->setVisible(true);
	}
	else
	{
		widgetContentStess->setVisible(false);
		widgetContent->setVisible(true);
	}*/
	if(!page1CamIDs.contains(iCameraNo))
	{
		widgetContent->setVisible(false);
		widgetContentStess->setVisible(true);
		iImagePage = 1;
	}else{
		widgetContentStess->setVisible(false);
		widgetContent->setVisible(true);
		iImagePage = 0;
	}

	widgetContent->setMinimumWidth(0);
	for (int i = 0; i < listCamID.size(); i++)
	{
		MyImageShowItem *imageShowItem = listImageShowItem.at(i);
		if (iCameraNo == listCamID[i])
		{
			imageShowItem->setVisible(true); 
			imageShowItem->setMaxShow(true);
		}
		else
		{
			if (iCameraNo != -1)
			{
				imageShowItem->setVisible(false);
			}
		}
	}
    iShownMode = iCameraNo;
}
void ImageWidget::slots_showPrevious(int nItemID)
{
    if(!camIDIndex.contains(nItemID))
    {
        return;
    }
    int idx = camIDIndex[nItemID];
	bool bFound = false;
	CGrabElement* pElement;

	pMainFrm->m_ErrorList.m_mutexmErrorList.lock();

	for (int i = iImagePosition[idx]+1; i < pMainFrm->m_ErrorList.listError.length(); i++)
	{
		pElement = pMainFrm->m_ErrorList.listError.at(i);
		if (pElement->nCamSN == nItemID)
		{
			iImagePosition[idx] = i;
			bFound = true;
			pMainFrm->m_SavePicture[nItemID].pThat=pElement->myImage;
			pMainFrm->m_SavePicture[nItemID].m_Picture=pElement->myImage->copy();
			break;
		}
	}
	pMainFrm->m_ErrorList.m_mutexmErrorList.unlock();

	if (bFound)
	{
		slots_stopShow(nItemID);
		QString camera = QString::number(pElement->nCamSN+1);
		QString imageSN = QString::number(pElement->nSignalNo);
		QString time = QString::number(pElement->dCostTime,'f',2);
		QString result = pMainFrm->m_sErrorInfo.m_vstrErrorType.at(pElement->nCheckRet);
		QString mouldID = QString::number(pElement->nMouldID);
		Q_ASSERT(pElement->myImage->byteCount() == pElement->myImage->height() * pElement->myImage->width());
		listImageShowItem.at(idx)->updateImage(pElement->myImage, camera, imageSN, time, result, mouldID, pElement->cErrorRectList);
		sAlgImageLocInfo[idx].sLocOri = pElement->sImgLocInfo.sLocOri;
		sAlgImageLocInfo[idx].sXldPoint.nCount = pElement->sImgLocInfo.sXldPoint.nCount;
		//new一个设置算法用的QImage
		if (ImageError[idx] != NULL)
		{
			delete ImageError[idx];
			ImageError[idx] = NULL;
		}
		ImageError[idx] = new QImage(*pElement->myImage);

	}
	else if (iImagePosition[idx] == -1)
	{
		QString strError = QString(tr("No error Image! "));
		listImageShowItem.at(idx)->slots_showWarningInfo(strError);
		return;
	}
	else
	{
		QString strError = QString(tr("Alreay the final Image! "));
		listImageShowItem.at(idx)->slots_showWarningInfo(strError);
		return;
	}
}
void ImageWidget::slots_showFollowing(int nItemID)
{
    if(!camIDIndex.contains(nItemID))
    {
        return;
    }
    int idx = camIDIndex[nItemID];
	bool bFound = false;
	CGrabElement* pElement;
	if (iImagePosition[idx] == -1)
	{
		iImagePosition[idx] = ERROR_IMAGE_COUNT;
	}
	for (int i = iImagePosition[idx]-1; i >=0; i--)
	{
		if (i < 0)
		{
			continue;
		}
		pElement = pMainFrm->m_ErrorList.listError.at(i);
		if (pElement->nCamSN == nItemID)
		{
			iImagePosition[idx] = i;
			bFound = true;
			pMainFrm->m_SavePicture[nItemID].pThat=pElement->myImage;
			pMainFrm->m_SavePicture[nItemID].m_Picture=pElement->myImage->copy();;
			break;
		}
	}
	if (bFound)
	{
		slots_stopShow(nItemID);

		QString camera = QString::number(pElement->nCamSN+1);
		QString imageSN = QString::number(pElement->nSignalNo);
		QString time = QString::number(pElement->dCostTime,'f',2);
		QString result = pMainFrm->m_sErrorInfo.m_vstrErrorType.at(pElement->nCheckRet);
		QString mouldID = QString::number(pElement->nMouldID);

		Q_ASSERT(pElement->myImage->byteCount() == pElement->myImage->height() * pElement->myImage->width());

		listImageShowItem.at(idx)->updateImage(pElement->myImage, camera, imageSN, time, result, mouldID, pElement->cErrorRectList);
		sAlgImageLocInfo[idx].sLocOri = pElement->sImgLocInfo.sLocOri;
		sAlgImageLocInfo[idx].sXldPoint.nCount = pElement->sImgLocInfo.sXldPoint.nCount;
		//new一个设置算法用的QImage
		if (ImageError[idx] != NULL)
		{
			delete ImageError[idx];
			ImageError[idx] = NULL;
		}
		ImageError[idx] = new QImage(*pElement->myImage);


	}else if (iImagePosition[idx] == ERROR_IMAGE_COUNT)
	{
		QString strError = QString(tr("No error Image! "));
		listImageShowItem.at(idx)->slots_showWarningInfo(strError);
		iImagePosition[idx] = -1;
		return;
	}
	else
	{
		QString strError = QString(tr("Alreay the first Image! "));
		listImageShowItem.at(idx)->slots_showWarningInfo(strError);
		return;
	}
}
void ImageWidget::slots_showStartRefresh(int nItemID)
{
    if(!camIDIndex.contains(nItemID))
    {
        return;
    }
    int idx = camIDIndex[nItemID];
	bIsShow[idx] = true;
	iImagePosition[idx] = -1;
	bIsShowErrorImage[idx] = false;
}
void ImageWidget::slots_showErrorImage(QImage* ImageShown, int iCamera, int iSignalNo,double iCostTime, int nMouldID, int iErrorType, QList<QRect> listRect, int listNo)
{
    if(!camIDIndex.contains(iCamera))
    {
        return;
    }
    int idx = camIDIndex[iCamera];
	slots_stopShow(iCamera);
	QString camera = QString::number(iCamera+1);
	QString imageSN = QString::number(iSignalNo);
	QString time = QString::number(iCostTime,'f',2);
	QString result = pMainFrm->m_sErrorInfo.m_vstrErrorType.at(iErrorType);
	QString mouldID = QString::number(nMouldID);
	iImagePosition[idx] = listNo;
	listImageShowItem.at(idx)->updateImage(ImageShown, camera, imageSN, time, result, mouldID, listRect);
	if (ImageError[idx] != NULL)
	{
		delete ImageError[idx];
		ImageError[idx] = NULL;
	}
	ImageError[idx] = new QImage(*ImageShown);
}
void ImageWidget::slots_showCheck(int nItemID)
{
	pMainFrm->slots_turnPage(ENaviPageAlgSet, nItemID);
}

void ImageWidget::showErrorCheck(int nItemID)
{
	if (listCamID.isEmpty() || !camIDIndex.contains(nItemID))
	{
		return;
	}
    int idx = camIDIndex[nItemID];
	try
	{//算法界面重新初始化,并以当前相机的错误图片和算法打开算法界面
		s_Status  sReturnStatus;
		QImage* tempIamge;
		if(pMainFrm->m_SavePicture[nItemID].pThat==NULL)
		{
			return;
		}
		tempIamge=pMainFrm->m_SavePicture[nItemID].pThat;
		sReturnStatus = pMainFrm->m_cBottleModel.CloseModelDlg();
		if (sReturnStatus.nErrorID != RETURN_OK)
		{
			CLogFile::write(tr("Abnormal in close model "),AbnormityLog);
			return;
		}

		s_AlgModelPara  sAlgModelPara;	
		sAlgModelPara.sImgPara.nChannel = 1;
		sAlgModelPara.sImgPara.nHeight = tempIamge->height();
		sAlgModelPara.sImgPara.nWidth = tempIamge->width();
		sAlgModelPara.sImgPara.pcData = (char*)tempIamge->bits();
		sAlgModelPara.sImgLocInfo = sAlgImageLocInfo[idx];

		if (sAlgModelPara.sImgPara.nHeight != pMainFrm->m_sCarvedCamInfo[nItemID].m_iImageHeight)
		{
			CLogFile::write(tr("Image height:%1 not fit camera height:%2").arg(sAlgModelPara.sImgPara.nHeight).arg(pMainFrm->m_sCarvedCamInfo[nItemID].m_iImageHeight),AbnormityLog);
			return;
		}
		if (sAlgModelPara.sImgPara.nWidth != pMainFrm->m_sCarvedCamInfo[nItemID].m_iImageWidth)
		{
			CLogFile::write(tr("Image Width:%1 not fit camera Width:%2").arg(sAlgModelPara.sImgPara.nWidth).arg(pMainFrm->m_sCarvedCamInfo[nItemID].m_iImageWidth),AbnormityLog);
			return;
		}		
        //?应该不会改,为什么需要重置
		for (int i=0;i<pMainFrm->m_sSystemInfo.iCamCount;i++)
		{
			pMainFrm->CherkerAry.pCheckerlist[i].nID = i;
			pMainFrm->CherkerAry.pCheckerlist[i].pChecker = &pMainFrm->m_cBottleCheck[i];
		}	

		int widthd = pMainFrm->widget_alg->geometry().width();
		int heightd	= pMainFrm->widget_alg->geometry().height();
		if (widthd < 150 || heightd < 150)
		{
			pMainFrm->statked_widget->setCurrentWidget(pMainFrm->widget_alg);
			pMainFrm->statked_widget->setCurrentWidget(pParent);
			widthd = pMainFrm->widget_alg->geometry().width();
			heightd	= pMainFrm->widget_alg->geometry().height();

			if (widthd < 150 || heightd < 150)
			{
				CLogFile::write(tr("widget_alg size is too small width:%1 height:%2").arg(widthd).arg(heightd),AbnormityLog);
				return;
			}
		}	
		sReturnStatus = pMainFrm->m_cBottleModel.SetModelDlg(sAlgModelPara,
			&pMainFrm->m_cBottleCheck[nItemID],pMainFrm->CherkerAry,pMainFrm->widget_alg);
		if (sReturnStatus.nErrorID != RETURN_OK)
		{
			CLogFile::write(tr("Abnormal in set Model"),AbnormityLog);
			return;
		}
		pMainFrm->statked_widget->setCurrentWidget(pMainFrm->widget_alg);
		pMainFrm->m_eCurrentMainPage = ENaviPageAlgSet;
		pMainFrm->m_eLastMainPage = ENaviPageAlgSet;
		pMainFrm->iLastPage = 5;

	}
	catch (...)
	{
		CLogFile::write(tr("Abnormal in set Model "),AbnormityLog);

	}

	CLogFile::write(tr("Into Alg page(From failure image)")+tr("CameraNo:%1").arg(nItemID),OperationLog,0);

}
void ImageWidget::slots_stopCheck(int nItemID)
{
	//CLogFile::write(tr("Stop camera%1 Check").arg(nItemID+1),OperationLog);
	pMainFrm->m_sRunningInfo.m_bIsCheck[nItemID] = 0;
	CameraStatusLabel *cameraStatus = pMainFrm->cameraStatus_list.at(nItemID);
	pMainFrm->m_cCombine[pMainFrm->m_sCarvedCamInfo[nItemID].m_iIOCardSN]->SetCombineCamera(nItemID,false);
	emit signals_SetCameraStatus(nItemID,2);
}
void ImageWidget::slots_stopAllStressCheck()
{
	if (!bIsStopAllStessCheck)
	{
		bIsStopAllStessCheck = true;
		for (int i=0; i<listCamID.size(); i++)
		{
			if (1 == pMainFrm->m_sCarvedCamInfo[listCamID[i]].m_iStress)
			{
				slots_stopCheck(listCamID[i]);
				listImageShowItem.at(i)->bIsCheck = false;
				emit signals_SetCameraStatus(listCamID[i],2);
			}
		}
	}
	else
	{
		bIsStopAllStessCheck = false;
		for (int i=0; i<listCamID.size(); i++)
		{
			if (1 == pMainFrm->m_sCarvedCamInfo[listCamID[i]].m_iStress)
			{
				slots_startCheck(listCamID[i]);
				listImageShowItem.at(i)->bIsCheck = true;
				emit signals_SetCameraStatus(listCamID[i],0);
			}
		}
	}
}
void ImageWidget::slots_startCheck(int nItemID)
{
	pMainFrm->m_sRunningInfo.m_bIsCheck[nItemID] = 1;
	pMainFrm->m_cCombine[pMainFrm->m_sCarvedCamInfo[nItemID].m_iIOCardSN]->SetCombineCamera(nItemID,true);
	emit signals_SetCameraStatus(nItemID,0);
}

void ImageWidget::slots_startShowAll()
{
	for (int i = 0; i < listCamID.size();i++)
	{
		if (!bIsShow[i])	
		{
			bIsShow[i] = true;
			listImageShowItem.at(i)->slots_startShow();
			MyImageShowItem *imageShowItem = listImageShowItem.at(i);
		}
		bIsShowErrorImage[i] = false;
	}
}

void ImageWidget::slots_startShow(int nItemID)
{
    if(!camIDIndex.contains(nItemID))
    {
        return;
    }
    int idx = camIDIndex[nItemID];
	bIsShow[idx] = true;
	bIsShowErrorImage[idx] = false;
	listImageShowItem.at(idx)->slots_startShow();
	MyImageShowItem *imageShowItem = listImageShowItem.at(idx);
}
void ImageWidget::slots_stopShow(int nItemID)
{
    if(!camIDIndex.contains(nItemID))
    {
        return;
    }
    int idx = camIDIndex[nItemID];
	bIsShowErrorImage[idx] = true;
	bIsShow[idx] = false;
	listImageShowItem.at(idx)->slots_stopShow();
	MyImageShowItem *imageShowItem = listImageShowItem.at(idx);
	if (ImageError[idx] != NULL)
	{
		delete ImageError[idx];
		ImageError[idx] = NULL;
	}
	if (pMainFrm->nQueue[idx].listGrab.size() <= 0)
	{
		return;
	}
	CGrabElement *pElement = pMainFrm->nQueue[nItemID].listGrab.last();
	ImageError[idx] = new QImage(*pElement->myImage);
}
//加入新错误图像时，当前错误图像位置加1。
void ImageWidget::slots_addError(int nItemID,int nSignalNo,int nErrorType)
{
	for (int i=0; i < listCamID.size(); i++)
	{
		if (iImagePosition[i] != -1)
		{
			if (iImagePosition[i] < ERROR_IMAGE_COUNT - 1)
			{
				iImagePosition[i]++;
			}
		}
	}
}

void ImageWidget::slots_showOnlyCamera(int cameraId)
{
    if(!page1CamIDs.contains(cameraId))
    {
        widgetContent->setVisible(false);
        widgetContentStess->setVisible(true);
        iImagePage = 1;
    }else{
        widgetContentStess->setVisible(false);
        widgetContent->setVisible(true);
        iImagePage = 0;
    }
}

void ImageWidget::slots_resetImgUpdate(int iCameraNo, bool st)
{
    if(!camIDIndex.contains(iCameraNo))
    {
        return;
    }
    MyImageShowItem *imageShowItem = listImageShowItem.at(camIDIndex[iCameraNo]);
    if(st)
    {
        connect(pMainFrm->pdetthread[iCameraNo],SIGNAL(signals_updateImage(QImage*, QString , QString ,QString , QString, QString, QList<QRect>,int )),imageShowItem,SLOT(slots_updateImage(QImage*, QString , QString ,QString , QString, QString, QList<QRect>,int )));
    }
    else
    {
        disconnect(pMainFrm->pdetthread[iCameraNo],SIGNAL(signals_updateImage(QImage*, QString , QString ,QString , QString, QString, QList<QRect>,int )),imageShowItem,SLOT(slots_updateImage(QImage*, QString , QString ,QString , QString, QString, QList<QRect>,int )));
    }       
}

bool ImageWidget::slots_checkCameraShow(int iCameraNo)
{
    if(!camIDIndex.contains(iCameraNo))
    {
        return false;
    }
    return bIsShow[camIDIndex[iCameraNo]];
}

bool ImageWidget::slots_checkCameraShowError(int iCameraNo)
{
    if(!camIDIndex.contains(iCameraNo))
    {
        return false;
    }
    return bIsShowErrorImage[camIDIndex[iCameraNo]];
}
