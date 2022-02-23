#pragma once
#include "ui_TestWidget.h"
namespace Ui
{
	class TestWidget;
};
class Widget_TestWighet :
	public QWidget
{
	 Q_OBJECT
private:
	Ui::TestWidget *ui;
public:
	Widget_TestWighet(void);
	~Widget_TestWighet(void);
};

