#ifndef CERRORIMAGELIST_H
#define CERRORIMAGELIST_H

#include <QWidget>
#include <QTableView>
#include <QGraphicsView>
#include <QStandardItemModel>
#include <QLabel>
#include <QTimer>
#include <QMutex>
#include <QPushButton>
#include "dhbasewidget.h"
#include "rotemptybottle.h"
#include "mytableview.h"

class ImageWidget;

class CErrorImageList : public DHBaseWidget
{
	Q_OBJECT

public:
	CErrorImageList(const QList<int>& _idLst, QWidget *parent);
	~CErrorImageList();
	QPushButton *buttonCarve;

signals:
	void signals_showErrorImage(QImage*, int, int, double, int, int, QList<QRect>, int);
    void signals_showMaxImage(int);

public slots:
	void slots_RemoveLastRow();
    int slots_ShowSelectImage(QModelIndex modelIndex);
	void slots_ShowCheckModle(QModelIndex modelIndex);
	void slots_appendFirstRow(int iCamNo,int iImageCount,int iErrorType);
	void slots_clearTable();
	void slots_updateInfo();

private:
	void initail();

private:
    ImageWidget* pParent;

    int refCardSN;  //��ʾ��ͳ���������õĿ�ID
    QList<int> idList;

	QMutex m_ModelLock;
	QTimer *timer_updateInfo;
	myTableView *m_ListErrorWidget;
	QStandardItemModel *m_modelError;					//��������ģ��
	QPushButton *btnClear;
	//�㷨ʹ�õļ���б�
	s_InputCheckerAry CherkerAry;

	QImage* imageError;
	QLabel *labelTotal;
	QLabel *labelFailur;
	QLabel *labelFailurRate;
	QLabel *labelModleRate;
};

#endif // CERRORIMAGELIST_H
