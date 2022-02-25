#ifndef BOTTLE_RESULT_COMBINE_H
#define BOTTLE_RESULT_COMBINE_H

#include "stateTool.h"
#include "ConfigInfo.h"

struct sBotResult {
    BottleResult result;    //已经有的结果 (未完成综合前,不一定是最终结果)
	BottleResult b_Rlts[CAMERA_MAX_COUNT];		//存储结果的数组
	bool b_Checked[CAMERA_MAX_COUNT];	//已经输入的结果的个数

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
 * 此类用于对识别结果综合
 */
class CBottleRltCombine
{
public:
 	QMutex m_MutexCombin;

	int i_CameraCount;
	//检测时候这个相机的结果默认好,为false时相机在工作，只是结果不再接受
    bool b_CombinCamera[CAMERA_MAX_COUNT];//对应相机是否需要综合
	
	sBotResult m_Rlts[256];
    int iErrorType[256];
    int m_iLatestImageNo;
    int m_iLatestErrorNo;
public:

	CBottleRltCombine();
	~CBottleRltCombine();

    //综合设备个数
	void Inital(int iGrabCountInQueen);

    void CombineRlt(int iCombineCount,int iGrabCountInQueen);
	void CombineRlt(int iCombineCount,int iGrabCountInQueen,int iRltCount);
    //设置设备是否综合
	void SetCombineCamera(int iCamera,bool bIfCombine);
    //设置某个相机的结果
	int AddResult(int iSignalNum,int CameraNum,BottleResult tmpResult);
    //检查是否已完成综合,如果是返回综合的结果和对应的相机编号
	bool ConbineResult(int iSignalNum,BottleResult &bCombineRlt)const;
    //返回正确识别的相机列表
    QList<int> successCameraList(const BottleResult& res)const;
    //重置所有图像号的设备状态
	void RemovAllResult();
    //移除图像号的结果,重置图像号的所有设备状态
	void RemoveOneResult(int iSignalNum);

};

#endif//BOTTLE_RESULT_COMBINE_H
