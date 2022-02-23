#include "mytableview.h"

myTableView::myTableView(QWidget *parent)
	: QTableView(parent)
{

}

myTableView::~myTableView()
{

}
void myTableView::dragEnterEvent ( QDragEnterEvent * event )
{
	emit signal_dragMove();
}
void myTableView::dragLeaveEvent ( QDragLeaveEvent * event )
{
	emit signal_dragMove();
}
void myTableView::dragMoveEvent ( QDragMoveEvent * event )
{
	emit signal_dragMove();
}
void myTableView::dropEvent ( QDropEvent * event )
{
	emit signal_dragMove();
}


// void myTableView::wheelEvent ( QWheelEvent * e )
// {
// 	emit signal_dragMove();
// }
