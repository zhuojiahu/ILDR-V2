#ifndef COUNTSET_H
#define COUNTSET_H

#include <QWidget>
#include "ui_countset.h"

class CountSet : public QWidget
{
	Q_OBJECT

public:
	CountSet(QWidget *parent = 0);
	~CountSet();

	Ui::CountSet ui;

private:
};

#endif // COUNTSET_H
