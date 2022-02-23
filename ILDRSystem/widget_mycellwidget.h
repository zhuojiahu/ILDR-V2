#ifndef WIDGETMYCELLWIDGET_H
#define WIDGETMYCELLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QAction>
#include <QMenu>

class WidgetMyCellWidget : public QWidget
{
	Q_OBJECT

public:
	WidgetMyCellWidget(QWidget *parent);
	~WidgetMyCellWidget();

	void addImg(QPixmap pixmapImg);
	void addText(QString stringLabel);
	void createActions();
	void contextMenuEvent(QContextMenuEvent *event);
private:
	QLabel *labelImg;
	QMenu *pop_menu;
public:
	QAction *action_selectImage;

};

#endif // WIDGETMYCELLWIDGET_H
