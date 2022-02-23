#include "cmygraphicsview.h"
#include "Widget_CarveImage.h"

CMyGraphicsView::CMyGraphicsView(QWidget *parent)
	: QGraphicsView(parent)
{

}

CMyGraphicsView::~CMyGraphicsView()
{

}
//鼠标点击事件
void CMyGraphicsView::mousePressEvent(QMouseEvent *event)
{  
	emit signals_mousePressEvent(event);
}
//鼠标释放事件
void CMyGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	emit signals_mouseReleaseEvent(event);
}
//鼠标移动事件
void CMyGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	emit signals_mouseMoveEvent(event);
}
void CMyGraphicsView::resizeEvent(QResizeEvent *event)
{

	emit signals_resizeEvent(event);
}