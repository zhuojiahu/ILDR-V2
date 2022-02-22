#ifndef UI_TYPES_H
#define UI_TYPES_H
#pragma once

//通用宏定义
#define SHADOW_WIDTH            0
#define TITLE_HEIGHT            110


//页面导航索引定义
const int ENaviPageEndIndex = 6;        //页面结束索引
enum ENavigation   
{
    ENaviPageOperation = 0,     //操作界面
    ENaviPageSettings,          //设置界面
    ENaviPageHistory,           //历史界面
    ENaviPageInOut,             //输入输出界面
    ENaviPageLight,             //空界面
    ENaviPageAlarm,             //报警界面
    ENaviToolLock,              //锁定按钮
    ENaviToolResetCount,        //重置计数按钮
};


#endif // UI_TYPES_H