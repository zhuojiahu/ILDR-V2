#ifndef CMYQUEUE_H
#define CMYQUEUE_H

#include <QMutex>
#include <QList>
#include "stateTool.h"

class CMyQueue
{
public:
	CMyQueue(void);
	~CMyQueue(void);
	void releaseMemory();
	void releaseSourceMemory();
	int InitID;
	bool InitQueue(int nWidth,int nHeight,int nBitCount,int nBlockCount,bool bAllocMemeroy=true);
	bool InitCarveQueue(int,int,int nWidth,int nHeight,int nBitCount,int nBlockCount,bool bAllocMemeroy=true);
	QMutex mGrabLocker;
	QMutex mDetectLocker;
	QList<CGrabElement *> listGrab;			//采集链表
	QList<CDetectElement> listDetect;	    //检测链表

private:
	BYTE * pBaseBuffer;  
	QVector<QRgb> vColorTable;				//生成灰度颜色表
};

#endif // CMYQUEUE_H
