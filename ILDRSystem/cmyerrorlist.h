#ifndef CMYERRORLIST_H
#define CMYERRORLIST_H

#include <QWidget>
#include "stateTool.h"
class CMyErrorList //: public QWidget
{
//	Q_OBJECT

public:
	CMyErrorList();//(QWidget *parent);
	~CMyErrorList();
	void initErrorList(int nWidth,int nHeight,int nBitCount,int nBlockCount,bool bAllocMemeroy=true);
	void releaseElement();


	QMutex m_mutexmErrorList;					
	QList<CGrabElement *> listError;			//采集链表
private:
	QVector<QRgb> vColorTable;				//生成灰度颜色表
};

#endif // CMYERRORLIST_H
