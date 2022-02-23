#include "MyQueue.h"
#include "ILDRSystem.h"


CMyQueue::CMyQueue(void)
{
	pBaseBuffer = NULL;
	listGrab.clear();
	vColorTable.clear();
	for (int i = 0; i < 256; i++)  
	{  
		vColorTable.append(qRgb(i, i, i)); 
	} 
	InitID = 0;
}


CMyQueue::~CMyQueue(void)
{
	releaseMemory();
}

//功能：初始化队列
bool CMyQueue::InitQueue(int nWidth,int nHeight,int nBitCount,int nBlockCount,bool bAllocMemeroy/* =true */)
{
	InitID++;
	releaseMemory();
	if (bAllocMemeroy)
	{
		int nBlockSize = nWidth*nHeight*nBitCount/8;
		for (int i=0;i<nBlockCount;i++)
		{		
			CGrabElement *pElement = new CGrabElement;
			pElement->sImgLocInfo.sXldPoint.nColsAry = new int[BOTTLEXLD_POINTNUM];
			pElement->sImgLocInfo.sXldPoint.nRowsAry = new int[BOTTLEXLD_POINTNUM];
			pElement->myImage = new QImage(nWidth,nHeight,nBitCount);// 用于实时显示
			pElement->myImage->setColorTable(vColorTable);
			pElement->initID = InitID;
			listGrab.push_back(pElement);
		}
	}
	return true;
}
bool CMyQueue::InitCarveQueue(int CarveWidth,int CarveHeight,int nWidth,int nHeight,int nBitCount,int nBlockCount,bool bAllocMemeroy/* =true */)
{
	InitID++;
	releaseSourceMemory();
	if (bAllocMemeroy)
	{
		int nBlockSize = nWidth*nHeight*nBitCount/8;
		for (int i=0;i<nBlockCount;i++)
		{		
			CGrabElement *pElement = new CGrabElement;
			pElement->sImgLocInfo.sXldPoint.nColsAry = new int[BOTTLEXLD_POINTNUM];
			pElement->sImgLocInfo.sXldPoint.nRowsAry = new int[BOTTLEXLD_POINTNUM];
			pElement->SourceImage = new QImage(nWidth,nHeight,nBitCount);// 用于实时显示
			pElement->SourceImage->setColorTable(vColorTable);
			pElement->myImage = new QImage(CarveWidth,CarveHeight,nBitCount);// 用于实时显示
			pElement->myImage->setColorTable(vColorTable);
			pElement->initID = InitID;
			listGrab.push_back(pElement);
		}
	}
	return true;
}
void CMyQueue::releaseSourceMemory()
{
	CGrabElement *pElement = NULL;
	int i = listGrab.length();
	while (!listGrab.isEmpty())
	{
		pElement = listGrab.first();
		listGrab.removeFirst();

		if (pElement->sImgLocInfo.sXldPoint.nColsAry != NULL)
		{
			delete []pElement->sImgLocInfo.sXldPoint.nColsAry;
			pElement->sImgLocInfo.sXldPoint.nColsAry = NULL;
		}
		if (pElement->sImgLocInfo.sXldPoint.nRowsAry != NULL)
		{
			delete []pElement->sImgLocInfo.sXldPoint.nRowsAry;
			pElement->sImgLocInfo.sXldPoint.nRowsAry = NULL;
		}
		if (!pElement->myImage->isNull())
		{
			delete pElement->myImage;
			pElement->myImage = NULL;
		}
		if (!pElement->SourceImage->isNull())
		{
			delete pElement->SourceImage;
			pElement->SourceImage = NULL;
		}
		if (pElement != NULL)
		{
			delete pElement;
			pElement = NULL;
		}
	}
	if (pBaseBuffer)
	{
		delete []pBaseBuffer;
	}

}
void CMyQueue::releaseMemory()
{
	CGrabElement *pElement = NULL;
	int i = listGrab.length();
	while (!listGrab.isEmpty())
	{
		pElement = listGrab.first();
		listGrab.removeFirst();

		if (pElement->sImgLocInfo.sXldPoint.nColsAry != NULL)
		{
			delete []pElement->sImgLocInfo.sXldPoint.nColsAry;
			pElement->sImgLocInfo.sXldPoint.nColsAry = NULL;
		}
		if (pElement->sImgLocInfo.sXldPoint.nRowsAry != NULL)
		{
			delete []pElement->sImgLocInfo.sXldPoint.nRowsAry;
			pElement->sImgLocInfo.sXldPoint.nRowsAry = NULL;
		}
		if (!pElement->myImage->isNull())
		{
			delete pElement->myImage;
			pElement->myImage = NULL;
		}
		if (pElement != NULL)
		{
			delete pElement;
			pElement = NULL;
		}
	}
	if (pBaseBuffer)
	{
		delete []pBaseBuffer;
	}

}

