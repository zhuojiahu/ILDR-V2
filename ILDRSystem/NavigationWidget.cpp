#include "NavigationWidget.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QSignalMapper>
#include <QDateTime>
#include <QTimer>

#include "pushButton.h"
#include "toolButton.h"
#include "uitypes.h"

NavigationWidget::NavigationWidget(QWidget *parent)
	: QWidget(parent)
{
	initInterface();
}

NavigationWidget::~NavigationWidget()
{
    tUpdateTime->stop();
}

void NavigationWidget::initInterface()
{
    //���õ�����ͼ��
    QStringList string_list;
    string_list.append(":/toolButton/Operation");
    string_list.append(":/toolButton/Settings");
    string_list.append(":/toolButton/History");
    string_list.append(":/toolButton/InOut");
    string_list.append(":/toolButton/Light");
    string_list.append(":/toolButton/Alarm");
    string_list.append(":/toolButton/Lock");
    string_list.append(":/toolButton/Reset");

    QHBoxLayout *hlay = new QHBoxLayout();//ˮƽ���ֹ�����
    //���õ�����������LOGO
    QLabel *logo_label = new QLabel();
    QPixmap pixmap(":/sys/Logo");
    logo_label->setPixmap(pixmap);
    logo_label->setFixedSize(pixmap.size());
    hlay->addWidget(logo_label);
    lDatetime = new QLabel;
    hlay->addWidget(lDatetime);
    hlay->addStretch();
    hlay->setSpacing(8);
    hlay->setContentsMargins(15, 0, 15, 0);
    //�趨��ťӳ��
    QSignalMapper *signal_mapper = new QSignalMapper(this);//���������źŹ���
    for(int i=0; i<string_list.size(); i++)
    { 
        ToolButton *tool_button = new ToolButton(string_list.at(i));
        tool_button->btnStyle = SETTINGSTYLE;
        button_list.append(tool_button);
        connect(tool_button, SIGNAL(clicked()), signal_mapper, SLOT(map()));
        signal_mapper->setMapping(tool_button, i);
        hlay->addWidget(tool_button, 0, Qt::AlignBottom);
    }
    connect(signal_mapper, SIGNAL(mapped(int)), this, SLOT(slots_turnPage(int)));
    //���ð�ť�·��ı�
    button_list.at(ENaviPageOperation)->setText(tr("Operation"));
    button_list.at(ENaviPageSettings)->setText(tr("Settings"));
    button_list.at(ENaviPageHistory)->setText(tr("History"));
    button_list.at(ENaviPageInOut)->setText(tr("InOut"));
    button_list.at(ENaviPageLight)->setText(tr("Light"));
    button_list.at(ENaviPageAlarm)->setText(tr("Alarm"));
    button_list.at(ENaviToolLock)->setText(tr("Lock"));
    button_list.at(ENaviToolResetCount)->setText(tr("Reset"));
    
    setLayout(hlay); 
    setFixedHeight(TITLE_HEIGHT);

    tUpdateTime = new QTimer(this);
    connect(tUpdateTime, SIGNAL(timeout()), this, SLOT(slots_updateTime()));
    tUpdateTime->start(1000);
}

void NavigationWidget::slots_turnPage(int index)
{
    slots_selectCurrent(index);
	emit signals_navigateTrig(index);//����main_widget
}

void NavigationWidget::slots_selectCurrent(int idx)
{
    if(idx < ENaviToolLock)
    {//�޸ĵ�ǰҳ��״̬
        for(int i=0; i<button_list.count()-1; i++)
        {
            ToolButton *tool_button = button_list.at(i);
            if(i != idx)
            {
                tool_button->setMousePress(false);
            }
        }
        button_list.at(idx)->setMousePress(true);
    }
}

void NavigationWidget::slots_setLockState(bool bLock)
{
    auto btn = button_list.at(ENaviToolLock);
    if(bLock)
    {
        btn->setIcon(QIcon(":/toolButton/Unlock"));
        btn->setText(tr("Unlock"));
    }
    else
    {
        btn->setIcon(QIcon(":/toolButton/Lock"));
        btn->setText(tr("Lock"));
    }
}

void NavigationWidget::slots_updateTime()
{
    QDateTime t = QDateTime::currentDateTime();
    lDatetime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd\nhh:mm:ss"));
}
