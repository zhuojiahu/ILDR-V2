#ifndef WIDGET_ERRORTYPE_H
#define WIDGET_ERRORTYPE_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QSignalMapper>
#include "ConfigInfo.h"

class Widget_ErrorType : public QWidget
{
	Q_OBJECT

public:
	Widget_ErrorType(QWidget *parent = 0);
	~Widget_ErrorType();

	void recoverChanged(){bIsChanged = false;}
	void setChanged(){bIsChanged = true;}
	bool isChanged(){return bIsChanged;}
	void initail();
// 	void setSimpleCount(int iSimpleCount);

// 	QLabel *label_sampleCount;
// 	QLineEdit *lineEdit_sampleCount;
	s_SampleInfo sSampleInfo;
	bool bIsChanged;
	bool bIsChosed[ERRORTYPE_MAX_COUNT];

	QList<QCheckBox*> listCheckBox;
	QSignalMapper *signalmapper;
public slots:
	void changeCheckState(int);
// 	void changeSimpleCount();

private:

	
};

#endif // WIDGET_ERRORTYPE_H
