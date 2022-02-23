#ifndef MYIMAGEVIEW_H
#define MYIMAGEVIEW_H

#include <QGraphicsView>
#include <QMenu>
class ImageWidget;
class CMyImageView : public QGraphicsView
{
	Q_OBJECT

public:
	CMyImageView(QWidget *parent);
	~CMyImageView();

	int getTriggerItemID();

	QAction *saveAction;
	QAction *showCheck;
	QAction *stopCheck;
	QAction *startCheck;
	QAction *startShow;

	QMenu *contextMenu;
protected:
	void mouseDoubleClickEvent(QMouseEvent *event);
	void resizeEvent(QResizeEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);
private slots:
	void slots_saveImage();
	void slots_showCheck();
	void slots_stopCheck();
	void slots_startCheck();
	void slots_startShow();
private:
	ImageWidget *pDlg;
	float fScale;	
	QPointF pOldPos;
	int nResizeType;//0:无 1:放大显示 2:复原
	int nItemID; 
	void createActions();
};

#endif // MYIMAGEVIEW_H
