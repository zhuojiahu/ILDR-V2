#ifndef UI_TYPES_H
#define UI_TYPES_H
#pragma once

//通用宏定义
#define SHADOW_WIDTH            0
#define TITLE_HEIGHT            110

#define DEFAULT_SKIN            ":/skin/1b"
#include <qimage.h>
#include <QWaitCondition>
#include <QMutex>
#include "qt_windows.h"

typedef enum{
    UI_EN,
    UI_ZH
}LANGUAGE;

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


//当前绘制状态
enum e_DrawStatus    
{
    DRAW_Status_NULL = 0,            //无操作
    DRAW_Status_MOVE,                //移动
    DRAW_Status_ZOOM,                //缩放
    DRAW_Status_MODIFY,              //修改
};


//当前鼠标位置所在点
enum e_DrawSelect    
{
    DRAW_Select_NULL = 0,            //无操作点
    DRAW_Select_MOVE,                //移动点
    DRAW_Select_ZOOM,                //缩放点
    DRAW_Select_MODIFY,              //修改点
};
//选取点
typedef struct _SelectPoint
{
    int iX;
    int iY;
    int iOffset;
    _SelectPoint()
    {
        iX = 0;
        iY = 0;
        iOffset = 5;
    }
}s_SelectPoint;

//矩形结构体
typedef struct _RectangleShape
{
    int ileftupX;
    int ileftupY;
    int iWidth;
    int iHeight;
    _RectangleShape()
    {
        ileftupX = 0;
        ileftupY = 0;
        iWidth = 1;
        iHeight = 1;
    }
    _RectangleShape(QRect rt)
    {
        ileftupX = rt.x();
        ileftupY = rt.y();
        iWidth = rt.width();
        iHeight = rt.height();
    }
}s_RectangleShape;

class sCamConfig
{
public:
    bool operator==(const sCamConfig& s)const
    {
        return camID == s.camID &&
            devType == s.devType &&
            camType == s.camType &&
            sMark == s.sMark &&
            initFileN == s.initFileN &&
            cardPort == s.cardPort &&
            cardIdx == s.cardIdx &&
            imgWidth == s.imgWidth &&
            imgHeight == s.imgHeight &&
            imgSrcPath == s.imgSrcPath &&
            trigCnt == s.trigCnt;
    }

    int camID;              //相机ID,从0开始
    int devType;            //设备类型
    int camType;            //相机类型
    QString sMark;          //相机MAC
    QString initFileN;      //相机初始化文件名
    int cardPort;           //触发相机的IO卡端口
    int cardIdx;            //触发相机的IO卡索引 从0开始

    int imgWidth;           //相机图片宽度
    int imgHeight;          //相机图片高度
    int offsetX;            //相机X的偏移量 用于线阵相机
    int roangle;            //相机旋转角度 用于线阵相机
    QString imgSrcPath;     //模拟相机的用于模拟的图片目录(绝对地址)

    int trigCnt;            //同一图像号的触发次数

    int CardCount;          //用于提供选择的IO卡索引
};

#endif // UI_TYPES_H