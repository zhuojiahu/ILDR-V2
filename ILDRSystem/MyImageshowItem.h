#ifndef MYIMAGESHOWITEM_H
#define MYIMAGESHOWITEM_H

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QTimer>

#include "pushButton.h"

class ImageWidget;

class MyImageShowItem : public QWidget
{
	Q_OBJECT

public:
	MyImageShowItem(QWidget *parent);
	~MyImageShowItem();

	void inital(int);
	//void SetRectColor();
	void setMaxShow(bool bSatus);
	void updateImage(QImage*, QString, QString, QString, QString, QString, QList<QRect>);
	//	void clearErrorInfo();

	bool bIsCheck;

signals:
	void signals_imageItemDoubleClick(int );
	void signals_showPrevious(int);
	void signals_showFollowing(int);
	void signals_showStartRefresh(int);
	void signals_startShowAll();

	void signals_showCheck(int );
	void signals_stopCheck(int );
	void signals_stopAllStressCheck( );
	void signals_startCheck(int );
	void signals_startShow(int );
	void signals_stopShow(int );
public slots:
	void slots_updateImage(QImage*, QString, QString, QString, QString, QString, QList<QRect>,int);
	void slots_showErrorInfo(QString error,int time = 0, bool bError = true);
	void slots_showWarningInfo(QString error,int time = 1, bool bError = true);
	void slots_update();

	void slots_clearErrorInfo();
	void slots_clearWarningInfo();

	void slots_saveImage();
	void slots_showCheck();
	void slots_stopCheck();
	void slots_stopAllStressCheck();
	void slots_startCheck();
	void slots_startAllStressCheck();
	void slots_startShow();
	void slots_stopShow();

	void slots_showPrevious();
	void slots_showFollowing();
	void slots_showStartRefresh();
	void slots_startShowAll();

protected:
	void createActions();
	void paintEvent(QPaintEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	// 	void resizeEvent(QResizeEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);
	virtual void enterEvent(QEvent *);
	virtual void leaveEvent(QEvent *);

private:
    ImageWidget* pParent;

	int iCameraNo;
	int iImageWidth;
	int iImageHeight;
	QString strCamera;
	QString strImageSN;
	QString strTime;
	QString strResult;
	QString strMouldID;
	QPixmap pixmapShown;
	QImage imageForWidget;
	QString strError;
	QString strWarning;
	bool bIsHaveError;
	bool bIsHaveWarning;
	QList<QRect> listErrorRect;

	QTimer *timerErrorInfo;
	QTimer *timerWarningInfo;

	QAction *saveAction;
	QAction *showCheck;
	QAction *stopCheck;
	QAction *stopAllStressCheck;
	QAction *startCheck;
	QAction *startAllStressCheck;
	//	QAction *startShow;
	QAction *startFreshAll;

	QMenu *contextMenu;

	PushButton *btnPrevious;
	PushButton *btnFollowing;
	PushButton *btnStartRefresh;

	QColor colorRect;
	bool bIsUpdateImage;
	// 	bool bIsCameraOK;
	QVector<QRgb> m_vcolorTable;

	bool bIsMaxShow;
};

#endif // MYIMAGESHOWITEM_H
