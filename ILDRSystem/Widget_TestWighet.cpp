#include "Widget_TestWighet.h"


Widget_TestWighet::Widget_TestWighet(void)
{
	ui = new Ui::TestWidget;
	ui->setupUi(this);
}


Widget_TestWighet::~Widget_TestWighet(void)
{
	 delete ui;
}
