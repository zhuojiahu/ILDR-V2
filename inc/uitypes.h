#ifndef UI_TYPES_H
#define UI_TYPES_H
#pragma once

//ͨ�ú궨��
#define SHADOW_WIDTH            0
#define TITLE_HEIGHT            110


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


#endif // UI_TYPES_H