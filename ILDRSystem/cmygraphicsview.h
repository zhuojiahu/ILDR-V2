#ifndef CMYGRAPHICSVIEW_H
#define CMYGRAPHICSVIEW_H

#include <QGraphicsView>
//#include "Widget_CarveImage.h"
class CMyGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	CMyGraphicsView(QWidget *parent);
	~CMyGraphicsView();
//	Widget_CarveImage *CarveImage;
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void resizeEvent(QResizeEvent *event);
signals:

	void signals_mousePressEvent(QMouseEvent *event);
	void signals_mouseReleaseEvent(QMouseEvent *event);
	void signals_mouseMoveEvent(QMouseEvent *event);
	void signals_resizeEvent(QResizeEvent *event);	
};

#endif // CMYGRAPHICSVIEW_H
