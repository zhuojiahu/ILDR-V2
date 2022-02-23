#ifndef UI_TYPES_H
#define UI_TYPES_H
#pragma once

//ͨ�ú궨��
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

//ҳ�浼����������
const int ENaviPageEndIndex = 6;        //ҳ���������
enum ENavigation   
{
    ENaviPageOperation = 0,     //��������
    ENaviPageSettings,          //���ý���
    ENaviPageHistory,           //��ʷ����
    ENaviPageInOut,             //�����������
    ENaviPageLight,             //�ս���
    ENaviPageAlarm,             //��������
    ENaviToolLock,              //������ť
    ENaviToolResetCount,        //���ü�����ť
};


//��ǰ����״̬
enum e_DrawStatus    
{
    DRAW_Status_NULL = 0,            //�޲���
    DRAW_Status_MOVE,                //�ƶ�
    DRAW_Status_ZOOM,                //����
    DRAW_Status_MODIFY,              //�޸�
};


//��ǰ���λ�����ڵ�
enum e_DrawSelect    
{
    DRAW_Select_NULL = 0,            //�޲�����
    DRAW_Select_MOVE,                //�ƶ���
    DRAW_Select_ZOOM,                //���ŵ�
    DRAW_Select_MODIFY,              //�޸ĵ�
};
//ѡȡ��
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

//���νṹ��
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

    int camID;              //���ID,��0��ʼ
    int devType;            //�豸����
    int camType;            //�������
    QString sMark;          //���MAC
    QString initFileN;      //�����ʼ���ļ���
    int cardPort;           //���������IO���˿�
    int cardIdx;            //���������IO������ ��0��ʼ

    int imgWidth;           //���ͼƬ���
    int imgHeight;          //���ͼƬ�߶�
    int offsetX;            //���X��ƫ���� �����������
    int roangle;            //�����ת�Ƕ� �����������
    QString imgSrcPath;     //ģ�����������ģ���ͼƬĿ¼(���Ե�ַ)

    int trigCnt;            //ͬһͼ��ŵĴ�������

    int CardCount;          //�����ṩѡ���IO������
};

#endif // UI_TYPES_H