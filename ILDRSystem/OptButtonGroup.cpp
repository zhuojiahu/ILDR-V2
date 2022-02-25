#include "OptButtonGroup.h"

#include <QAbstractButton>

OptButtonGroup::OptButtonGroup(QObject *parent)
    : QButtonGroup(parent)
{

}

OptButtonGroup::~OptButtonGroup()
{

}

void OptButtonGroup::selectAll()
{
    auto btnList = buttons();
    foreach(auto btn, btnList)
    {
        if(btn->isCheckable() && btn->isEnabled())
        {
            btn->setChecked(true);
        }
    }
}

void OptButtonGroup::selectNo()
{
    auto btnList = buttons();
    foreach(auto btn, btnList)
    {
        if(btn->isCheckable() && btn->isEnabled())
        {
            btn->setChecked(false);
        }
    }
}

void OptButtonGroup::invertSelection()
{
    auto btnList = buttons();
    foreach(auto btn, btnList)
    {
        if(btn->isCheckable() && btn->isEnabled())
        {
            btn->setChecked(!btn->isChecked());
        }
    }
}

void OptButtonGroup::setManage(bool st)
{
    auto btnList = buttons();
    foreach(auto btn, btnList)
    {
        if(st)
        {//管理模式 清空选择,隐藏非使能控件
            if(btn->isCheckable() && btn->isEnabled())
            {
                btn->setChecked(false);
            }
            else if(!btn->isEnabled())
            {
                btn->setVisible(false);
            }
        }
        else
        {
            btn->setVisible(true);
        }
    }
}
