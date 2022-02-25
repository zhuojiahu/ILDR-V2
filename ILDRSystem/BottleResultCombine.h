#ifndef BOTTLE_RESULT_COMBINE_H
#define BOTTLE_RESULT_COMBINE_H

#include "stateTool.h"
#include "ConfigInfo.h"

struct sBotResult {
    BottleResult result;    //�Ѿ��еĽ�� (δ����ۺ�ǰ,��һ�������ս��)
	BottleResult b_Rlts[CAMERA_MAX_COUNT];		//�洢���������
	bool b_Checked[CAMERA_MAX_COUNT];	//�Ѿ�����Ľ���ĸ���

 	sBotResult()
 	{	
 		for (int i = 0;i < CAMERA_MAX_COUNT; i++)
 		{
 			b_Rlts[i].reset();
			b_Checked[i] = false;
 		}
 	}
    QList<int> succeedCamList(const BottleResult& res)const
    {
        QList<int> list;
        for (int i = 0 ; i<CAMERA_MAX_COUNT;i++)
        {
            if (b_Checked[i]&& res == b_Rlts[i])
            {
                list.append(i);
            }
        }
        return list;
    }
};
/**
 * �������ڶ�ʶ�����ۺ�
 */
class CBottleRltCombine
{
public:
 	QMutex m_MutexCombin;

	int i_CameraCount;
	//���ʱ���������Ľ��Ĭ�Ϻ�,Ϊfalseʱ����ڹ�����ֻ�ǽ�����ٽ���
    bool b_CombinCamera[CAMERA_MAX_COUNT];//��Ӧ����Ƿ���Ҫ�ۺ�
	
	sBotResult m_Rlts[256];
    int iErrorType[256];
    int m_iLatestImageNo;
    int m_iLatestErrorNo;
public:

	CBottleRltCombine();
	~CBottleRltCombine();

    //�ۺ��豸����
	void Inital(int iGrabCountInQueen);

    void CombineRlt(int iCombineCount,int iGrabCountInQueen);
	void CombineRlt(int iCombineCount,int iGrabCountInQueen,int iRltCount);
    //�����豸�Ƿ��ۺ�
	void SetCombineCamera(int iCamera,bool bIfCombine);
    //����ĳ������Ľ��
	int AddResult(int iSignalNum,int CameraNum,BottleResult tmpResult);
    //����Ƿ�������ۺ�,����Ƿ����ۺϵĽ���Ͷ�Ӧ��������
	bool ConbineResult(int iSignalNum,BottleResult &bCombineRlt)const;
    //������ȷʶ�������б�
    QList<int> successCameraList(const BottleResult& res)const;
    //��������ͼ��ŵ��豸״̬
	void RemovAllResult();
    //�Ƴ�ͼ��ŵĽ��,����ͼ��ŵ������豸״̬
	void RemoveOneResult(int iSignalNum);

};

#endif//BOTTLE_RESULT_COMBINE_H
