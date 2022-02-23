#include "widget_errortype.h"

#include "ILDRSystem.h"
extern SysMainUI *pMainFrm;

Widget_ErrorType::Widget_ErrorType(QWidget *parent)
	: QWidget(parent)
{
	initail();
	for (int i = 0; i<ERRORTYPE_MAX_COUNT;i++)
	{
		if (0 == i)
		{
			bIsChosed[i] = false;
		}
		else
		{
			bIsChosed[i] = true;
		}
	}
}

Widget_ErrorType::~Widget_ErrorType()
{

}

void Widget_ErrorType::initail()
{
	QGridLayout *gridlayout = new QGridLayout();
	signalmapper = new QSignalMapper(this);//工具栏的信号管理
	for (int i = 0;i<=pMainFrm->m_sErrorInfo.m_iErrorTypeCount;i++)
	{
		QCheckBox *checkBox = new QCheckBox(this);
		checkBox->setText(pMainFrm->m_sErrorInfo.m_vstrErrorType.at(i));
		if (0 == i)
		{
			checkBox->setChecked(false);
		}
		else
		{
			checkBox->setChecked(true);
		}

		listCheckBox.append(checkBox);
		connect(checkBox, SIGNAL(stateChanged(int)), signalmapper, SLOT(map()));
		signalmapper->setMapping(checkBox, i);
		gridlayout->addWidget(checkBox,i/4,i%4);
	}


	connect(signalmapper, SIGNAL(mapped(int)), this, SLOT(changeCheckState(int)));
	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addLayout(gridlayout);
	mainLayout->setSpacing(6);
	mainLayout->setContentsMargins(9,0,9,9);

	setLayout(mainLayout);
}
void Widget_ErrorType::changeCheckState(int index)
{
	bIsChanged = true;
	if (listCheckBox.at(index)->isChecked())
	{
		sSampleInfo.m_bSampleType[index] = true;
		sSampleInfo.m_iSampleTypeCount++;
		bIsChosed[index] = true;
	}
	else
	{
		sSampleInfo.m_bSampleType[index] = false;
		sSampleInfo.m_iSampleTypeCount--;
		bIsChosed[index] = false;

	}

}
