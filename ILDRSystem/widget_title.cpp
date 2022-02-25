#include "widget_title.h"
#include <QCheckBox>
#include <QGroupBox>

#include "ILDRSystem.h"
extern SysMainUI *pMainFrm;


WidgetTitle::WidgetTitle(QWidget *parent)
	: QFrame(parent)
{
	setObjectName("WidgetTitle");
	version_title = new QLabel();
	labelVersion = new QLabel();
	min_button = new PushButton();
	close_button = new PushButton();
	QFont ft;
	ft.setPointSize(12);
	version_title->setFont(ft);
	ft.setPointSize(8);
	version_title->setFont(ft);

	min_button->setPicName(QString(":/sysButton/min"));
	close_button->setPicName(QString(":/sysButton/close"));
	close_button->hide();
	connect(min_button, SIGNAL(clicked()), this, SIGNAL(showMin()));
	connect(close_button, SIGNAL(clicked()), this, SIGNAL(closeWidget()));

    QHBoxLayout *title_layout = new QHBoxLayout();
    title_layout->addStretch();
    title_layout->addWidget(version_title,0,Qt::AlignVCenter);
    title_layout->addStretch();
    title_layout->addWidget(min_button, 0, Qt::AlignTop);
    title_layout->addWidget(close_button, 0, Qt::AlignTop);
	title_layout->setSpacing(0);
	title_layout->setContentsMargins(5, 0, 0, 0);
	version_title->setContentsMargins(15, 0, 0, 0);

    QGroupBox* btnGbx = new QGroupBox; 
	QStringList string_list;
	string_list<<":/toolWidget/Operation"
        <<":/toolWidget/algset"
        <<":/toolWidget/Article"
        <<":/toolWidget/Settings"
        <<":/toolWidget/InOut"
        <<":/toolWidget/History"
        <<":/toolWidget/Alarm"
        <<":/toolWidget/Light"
        <<":/toolWidget/clear"
        <<":/toolWidget/start"
        <<":/toolWidget/Lock"
        <<":/toolWidget/exit";
	QHBoxLayout *button_layout = new QHBoxLayout();//水平布局管理器

	QSignalMapper *signal_mapper = new QSignalMapper(this);//工具栏的信号管理
	for(int i=0; i<string_list.size(); i++)
	{ 
		ToolButton *tool_button = new ToolButton(string_list.at(i));
		tool_button->btnStyle = TITELSTYLE;
		button_list.append(tool_button);
		connect(tool_button, SIGNAL(clicked()), signal_mapper, SLOT(map()));
		signal_mapper->setMapping(tool_button, QString::number(i, 10));
		button_layout->addWidget(tool_button, 0, Qt::AlignBottom);
	}
	connect(signal_mapper, SIGNAL(mapped(QString)), this, SLOT(turnPage(QString)));
	
	QLabel *logo_label = new QLabel();
	QPixmap pixmap(":/sys/logo_tiama");
	logo_label->setPixmap(pixmap);
	logo_label->setFixedSize(pixmap.size());

	QVBoxLayout *layoutLogo = new QVBoxLayout();//水平布局管理器
	QSizePolicy sizePolicyLogo(QSizePolicy::Preferred, QSizePolicy::Expanding);
	sizePolicyLogo.setHorizontalStretch(0);
	sizePolicyLogo.setVerticalStretch(0);
	 logo_label->setSizePolicy(sizePolicyLogo);
	QSizePolicy sizePolicyVersion(QSizePolicy::Preferred, QSizePolicy::Minimum);
	sizePolicyVersion.setHorizontalStretch(0);
	sizePolicyVersion.setVerticalStretch(0);
	labelVersion->setSizePolicy(sizePolicyVersion);

	layoutLogo->addWidget(logo_label);
	labelVersion->setAlignment(Qt::AlignRight);

	button_layout->addStretch();
	button_layout->addLayout(layoutLogo);
	button_layout->setSpacing(8);
	button_layout->setContentsMargins(15, 0, 15, 0);
    btnGbx->setLayout(button_layout);
    btnGbx->setObjectName("toolbar");
	QVBoxLayout *main_layout = new QVBoxLayout();
	main_layout->addLayout(title_layout);
	main_layout->addWidget(btnGbx);
	main_layout->setSpacing(0);
	main_layout->setContentsMargins(0, 0, 0, 0);

	this->addToolName();

    setLayout(main_layout); 
    setFixedHeight(TITEL_HEIGHT);
    for(int i = 0; i < button_list.size(); i++)
    {
        button_list.at(i)->setVisible(pMainFrm->buttonVisible[i]);
    }
}

void WidgetTitle::addToolName()
{
	// 	labelVersion->setText(pMainFrm->sVersion);
	version_title->setText(pMainFrm->m_sSystemInfo.m_strWindowTitle);
	min_button->setToolTip(tr("minimize"));
	close_button->setToolTip(tr("close"));
    button_list.at(ENaviPageOperation)->setText(tr("Operation"));
    button_list.at(ENaviPageAlgSet)->setText(tr("Algorithm"));
    button_list.at(ENaviPageArticle)->setText(tr("Article"));
	button_list.at(ENaviPageSettings)->setText(tr("Settings"));
	button_list.at(ENaviPageHistory)->setText(tr("History"));
    button_list.at(ENaviPageInOut)->setText(tr("InOut"));
    button_list.at(ENaviPageAlarm)->setText(tr("Alarm"));
    button_list.at(ENaviPageLight)->setText(tr("Light"));
	button_list.at(ENaviToolResetCount)->setText(tr("Clear"));
    button_list.at(ENaviToolStart)->setText(tr("Start"));
    button_list.at(ENaviToolLock)->setText(tr("Lock"));
    button_list.at(ENaviToolExit)->setText(tr("Exit"));
}

void WidgetTitle::turnPage(QString current_page)
{
	bool ok;  
	int current_index = current_page.toInt(&ok, 10);
	for(int i=0; i<button_list.count(); i++)
	{
		ToolButton *tool_button = button_list.at(i);
		if (ENaviPageEndIndex < current_index)
		{
			emit turnPage(current_index);//发给main_widget
			button_list.at(current_index)->setMousePress(false);
			return;
		}
		if(i == current_index)
		{
			tool_button->setMousePress(true);
		}
		else
		{
			tool_button->setMousePress(false);
		}

	}
	emit turnPage(current_index);//发给main_widget
}

void WidgetTitle::slots_setLockState(bool bLock)
{
    auto btn = button_list.at(ENaviToolLock);
    if(bLock)
    {
        btn->setIcon(QIcon(":/toolWidget/Unlock"));
        btn->setText(tr("Unlock"));
    }
    else
    {
        btn->setIcon(QIcon(":/toolWidget/Lock"));
        btn->setText(tr("Lock"));
    }
}
