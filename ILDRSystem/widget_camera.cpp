#include "widget_camera.h"

#include "ILDRSystem.h"
extern SysMainUI *pMainFrm;

Widget_Camera::Widget_Camera(QWidget *parent)
	: QWidget(parent)
{
	initail();
	for (int i = 0; i<CAMERA_MAX_COUNT;i++)
	{
		bIsChosed[i] = true;
	}

}

Widget_Camera::~Widget_Camera()
{

}

void Widget_Camera::initail()
{
	QGridLayout *gridlayout = new QGridLayout();
	signalmapper = new QSignalMapper(this);//工具栏的信号管理
	for (int i = 0;i<pMainFrm->m_sSystemInfo.iCamCount;i++)
	{
		QCheckBox *checkBox = new QCheckBox(this);
		checkBox->setText(tr("Camera")+QString::number(i+1));
		checkBox->setChecked(true);

		listCheckBox.append(checkBox);
 		connect(checkBox, SIGNAL(stateChanged(int)), signalmapper, SLOT(map()));
// 		connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(changeCheckState(int)));
		signalmapper->setMapping(checkBox, i);
		gridlayout->addWidget(checkBox,i/4,i%4);
	}
	
	connect(signalmapper, SIGNAL(mapped(int)), this, SLOT(slots_changeCheckState(int)));

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addLayout(gridlayout);
	mainLayout->setSpacing(6);
	mainLayout->setContentsMargins(9,0,9,9);

	setLayout(mainLayout);
}
void Widget_Camera::slots_changeCheckState(int index)
{
	bIsChanged = true;
	if (listCheckBox.at(index)->isChecked())
	{
		bIsChosed[index] = true;
	}
	else
	{
		bIsChosed[index] = false;
	}
}