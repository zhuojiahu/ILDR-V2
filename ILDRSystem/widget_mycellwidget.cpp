#include <QLayout>
 #include <QContextMenuEvent>
#include <QPoint>

#include "widget_mycellwidget.h"

WidgetMyCellWidget::WidgetMyCellWidget(QWidget *parent)
	: QWidget(parent)
{
	labelImg = new QLabel;
	QHBoxLayout *mainlayout = new QHBoxLayout(this);
	mainlayout->addStretch();
	mainlayout->addWidget(labelImg);
	mainlayout->addStretch();
	mainlayout->setMargin(0);			// 必须添加, 否则不能正常显示  
	mainlayout->setAlignment(labelImg, Qt::AlignCenter);  
	createActions();
}

WidgetMyCellWidget::~WidgetMyCellWidget()
{

}
void WidgetMyCellWidget::addImg(QPixmap pixmapImg)
{
	labelImg->setPixmap(pixmapImg);
}
void WidgetMyCellWidget::addText(QString stringLabel)
{
	labelImg->setText(stringLabel);

}
void WidgetMyCellWidget::createActions()
{
	pop_menu = new QMenu();
	action_selectImage = new QAction(this);
	action_selectImage->setText(QString(tr("Select Image")));
}
void WidgetMyCellWidget::contextMenuEvent(QContextMenuEvent *event)
{
	pop_menu->clear(); //清除原有菜单
	QPoint point = event->pos(); //得到窗口坐标
	pop_menu->addAction(action_selectImage);

	//菜单出现的位置为当前鼠标的位置
	pop_menu->exec(QCursor::pos());
	event->accept();
}
