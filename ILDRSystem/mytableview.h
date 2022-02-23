#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H

#include <QTableView>

class myTableView : public QTableView
{
	Q_OBJECT

public:
	myTableView(QWidget *parent);
	~myTableView();

	void dragEnterEvent ( QDragEnterEvent * event );
	void dragLeaveEvent ( QDragLeaveEvent * event );
	void dragMoveEvent ( QDragMoveEvent * event );
	void dropEvent ( QDropEvent * event );

// 	void wheelEvent ( QWheelEvent * e );

signals:
	void signal_dragMove();

private:
	
};

#endif // MYTABLEVIEW_H
