#include "cmyerrorlist.h"

CMyErrorList::CMyErrorList()//(QWidget *parent)
	//: QWidget(parent)
{
	listError.clear();

	vColorTable.clear();
	for (int i = 0; i < 256; i++)  
	{  
		vColorTable.append(qRgb(i, i, i)); 
	} 

}

CMyErrorList::~CMyErrorList()
{
	releaseElement();

}
void CMyErrorList::initErrorList(int nWidth,int nHeight,int nBitCount,int nBlockCount,bool bAllocMemeroy)
{
	releaseElement();

	m_mutexmErrorList.lock();
	qDeleteAll(listError);
	listError.clear();
// 	int nBlockSize = nWidth*nHeight*nBitCount/8;
 	for (int i=0;i<nBlockCount;i++)
 	{	
		CGrabElement *pElement = new CGrabElement;
		pElement->sImgLocInfo.sXldPoint.nColsAry = new int[BOTTLEXLD_POINTNUM];
		pElement->sImgLocInfo.sXldPoint.nRowsAry = new int[BOTTLEXLD_POINTNUM];
		pElement->myImage = NULL;
  		//pElement->myImage = new QImage(nWidth,nHeight,nBitCount/8);
  		//pElement->myImage->setColorTable(vColorTable);
 
 		listError.push_back(pElement);
 		pElement = NULL;
 	}
	m_mutexmErrorList.unlock();

}
void CMyErrorList::releaseElement()
{
	CGrabElement *pElement = NULL;
	while (!listError.isEmpty())
	{
		m_mutexmErrorList.lock();
		pElement = listError.first();
		if (pElement->myImage != NULL)
		{
			delete pElement->myImage;
			pElement->myImage = NULL;
		}
		listError.removeFirst();
		if (pElement != NULL)
		{
			delete pElement;
			pElement = NULL;
		}
		m_mutexmErrorList.unlock();
	}
}
