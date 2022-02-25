#include "widget_test.h"

#include <QMessageBox>
#include <QStringList>

#include "clogfile.h"
#include "ILDRSystem.h"
#include "EquipRuntime.h"
#include "OptButtonGroup.h"
//剔废参数列表项目最大列数
const int maxColCnt = 5;

extern SysMainUI *pMainFrm;

WidgetTest::WidgetTest(QWidget *parent)
	: QWidget(parent)
{
    ui.setupUi(this);

	ifshowImage = 0;
	iSaveMode = -1;

	int widgetWidth = pMainFrm->statked_widget->geometry().width();
	initWidgetName();
	ui.btnChoseCamera->setVisible(false);
	ui.btnChoseErrorType->setVisible(false);

	//增加图片刷新选择
	ui.comboBox->insertItem(0,tr("Refresh All")); 
	ui.comboBox->insertItem(1,tr("Only Bad Images Are Refreshed"));
	ui.comboBox->insertItem(2,tr("All Not Refresh"));

    ui.checkBox_CameraOffLine->setVisible(true);
	ui.label_MissNumber->setVisible(false);
	ui.spinBox_OffLineNumber->setVisible(false);
	ui.checkBox_CameraContinueReject->setVisible(false);
	ui.label_rejectNumber->setVisible(false);
	ui.spinBox_RejectNo->setVisible(false);

    ui.pushButton_set->setVisible(false);
    ui.pushButton_set_2->setVisible(false);
    if(pMainFrm->m_sSystemInfo.iIOCardCount < 2)
    {
        ui.gbxCard2->setVisible(false);
    }
    if(pMainFrm->m_sSystemInfo.iIOCardCount < 1)
    {
        ui.gbxCard1->setEnabled(false);
    }

    for (int i = 0;i<pMainFrm->m_sSystemInfo.iCamCount;i++)
    {
        connect(pMainFrm->pdetthread[i], SIGNAL(signals_updateAlert(int)), this, SLOT(slots_updateAlert(int)));
    }
    
    init();
    timerUpdateIOCardCounter = new QTimer(this);
    timerUpdateIOCardCounter->setInterval(100);//每毫秒刷新一次计数
    CameraOffAlarm = new QTimer(this);
    CameraOffAlarm->setInterval(10000);
    connect(CameraOffAlarm, SIGNAL(timeout()), this, SLOT(slots_CameraOffAlarm()));  

    if (pMainFrm->m_sSystemInfo.bCameraOffLineSurveillance)
    {
        CameraOffAlarm->start();
    }
}
WidgetTest::~WidgetTest()
{
	delete widget_ErrorType;
	delete widget_Camera;
}
void WidgetTest::slots_intoWidget()
{	
	ui.spinBox_LoginHoldTime->setValue(pMainFrm->m_sSystemInfo.nLoginHoldTime);

	connect(timerUpdateIOCardCounter, SIGNAL(timeout()), this, SLOT(slots_updateIOcardCounter()));
	timerUpdateIOCardCounter->stop();
	Sleep(10);
	timerUpdateIOCardCounter->start();

	ui.checkBox_CameraOffLine->setChecked(pMainFrm->m_sSystemInfo.bCameraOffLineSurveillance);
	ui.checkBox_CameraContinueReject->setChecked(pMainFrm->m_sSystemInfo.bCameraContinueRejectSurveillance);
	ui.spinBox_OffLineNumber->setValue(pMainFrm->m_sSystemInfo.iCamOfflineNo);
	ui.spinBox_RejectNo->setValue(pMainFrm->m_sSystemInfo.iCamContinueRejectNumber);

	if (pMainFrm->m_sSystemInfo.m_iSaveNormalErrorImageByTime)
	{
		ui.checkBox_saveFailureNormalImage->setChecked(true);
	}
	else{
		ui.checkBox_saveFailureNormalImage->setChecked(false);
	}
	if (pMainFrm->m_sSystemInfo.m_iSaveStressErrorImageByTime)
	{
		ui.checkBox_saveFailureStressImage->setChecked(true);
	}
	else{
		ui.checkBox_saveFailureStressImage->setChecked(false);
	}

	switch (pMainFrm->m_sRunningInfo.m_eSaveImageType)
	{
	case NotSave:
		ui.comboBox_SaveMode->setCurrentIndex(0);
		iLastSaveMode = 0;
		iSaveMode = 0;
		break;
	case AllImageInCount:
		ui.comboBox_SaveMode->setCurrentIndex(1);
		iLastSaveMode = 1;
		iSaveMode = 1;
		break;
	case FailureImageInCount:
		ui.comboBox_SaveMode->setCurrentIndex(2);
		iLastSaveMode = 2;
		iSaveMode = 2;
		break;
	}
	
	slots_SaveModeChanged(iSaveMode);

	iSaveImgCount = 0;
	for(int i = 0; i<pMainFrm->m_sSystemInfo.iCamCount; i++)
	{
		if (iSaveImgCount < pMainFrm->m_sRunningInfo.m_iSaveImgCount[i])
		{		
			iSaveImgCount = pMainFrm->m_sRunningInfo.m_iSaveImgCount[i];
		}
	}
	ui.spinBox_Number->setValue(iSaveImgCount);
	initInformation();//更新接口卡配置
	ui.comboBox->setCurrentIndex(ifshowImage);
    slots_reloadKickParams();
}
bool WidgetTest::leaveWidget()
{
    //移除标记项
    auto btns = markedIDGroup->buttons();
    foreach(auto btn, btns)
    {
        ui.glayMardIDs->removeWidget(btn);
        markedIDGroup->removeButton(btn);
    }
    btns = markedCaviGroup->buttons();
    foreach(auto btn, btns)
    {
        ui.glayMarkedCavity->removeWidget(btn);
        markedCaviGroup->removeButton(btn);
    }
	return true;
}

void WidgetTest::init()
{
	iChoseCamera = 1;
	iChoseErrorType = 1;
	iSaveMode = 0;
	iLastSaveMode = 0;
	iSaveImgCount = 0;
	iLastSaveImgCount = 0;
    for(int i = 0; i < IOCard_MAX_COUNT;i++)
    {
        m_nDelay1[i] = 0;
        m_nDelay2[i] = 0;
        m_nDelay3[i] = 0;
        m_nDelay4[i] = 0;
        m_nDelay5[i] = 0;
        m_nDelay6[i] = 0;

        m_nKickDelay[i] = 0;
        m_nKickWidth[i] = 0;
        m_nSampleDelay[i] = 0;
        m_nSampleWidth[i] = 0;

        iIOCardOffSet[i] = 0;
    }
	
	ui.read->setVisible(true);
	ui.label_Number->setEnabled(false);
	ui.spinBox_Number->setEnabled(false);
	ui.comboBox_SaveMode->setCurrentIndex(0);

	connect(ui.comboBox_SaveMode, SIGNAL(currentIndexChanged(int)), this, SLOT(slots_SaveModeChanged(int)));
	buttonGroup  = new QButtonGroup(this);
	buttonGroup->addButton(ui.radioButton_bad,0);
	buttonGroup->addButton(ui.radioButton_good,1);
	buttonGroup->addButton(ui.radioButton_normal,2);
	connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(slots_KickModeChanged(int)));
	ui.radioButton_normal->setChecked(true);
    buttonGroup2  = new QButtonGroup(this);
    buttonGroup2->addButton(ui.radioButton_bad_2,0);
    buttonGroup2->addButton(ui.radioButton_good_2,1);
    buttonGroup2->addButton(ui.radioButton_normal_2,2);
    connect(buttonGroup2, SIGNAL(buttonClicked(int)), this, SLOT(slots_KickModeChanged(int)));
    ui.radioButton_normal_2->setChecked(true);

	widget_Camera = new Widget_Camera();
	ui.layoutChoseCamera->addWidget(widget_Camera);
	widget_ErrorType = new Widget_ErrorType();
	ui.layoutChoseErrorType->addWidget(widget_ErrorType);

	ui.btnOK->setVisible(false);
	ui.btnCancel->setVisible(false);

    markedIDGroup = new OptButtonGroup(this);
    markedIDGroup->setExclusive(false);
    markedCaviGroup = new OptButtonGroup(this);
    markedCaviGroup->setExclusive(false);

	initEquipAlarmTablewidget();
	connect(ui.btnSaveLoginHoldTime, SIGNAL(clicked()), this, SLOT(slots_SaveLoginHoldTime()));
	connect(ui.btnChoseCamera, SIGNAL(clicked()), this, SLOT(slots_ChoseCamera()));
	connect(ui.btnChoseErrorType, SIGNAL(clicked()), this, SLOT(slots_ChoseErrorType()));
	connect(ui.btnOK_Save, SIGNAL(clicked()), this, SLOT(slots_OKSave()));
	connect(ui.btnOK_CameraSurveillance, SIGNAL(clicked()), this, SLOT(slots_OKCameraSurveillance()));
	connect(ui.pushButton_2,SIGNAL(clicked()), this, SLOT(slots_ifCheckShowImage()));
    connect(ui.read, SIGNAL(clicked()), this, SLOT(slots_readDelay()));
	connect(ui.read_2, SIGNAL(clicked()), this, SLOT(slots_readDelay()));
    connect(ui.settocard, SIGNAL(clicked()), this, SLOT(slots_setToCard()));
	connect(ui.settocard_2, SIGNAL(clicked()), this, SLOT(slots_setToCard()));
    connect(ui.settofile, SIGNAL(clicked()), this, SLOT(slots_setToFile()));
	connect(ui.settofile_2, SIGNAL(clicked()), this, SLOT(slots_setToFile()));

    connect(ui.advance1, SIGNAL(clicked()), this, SLOT(slots_advance1()));
	connect(ui.advance1_2, SIGNAL(clicked()), this, SLOT(slots_advance1()));
	connect(ui.pushButton_choseAllCamera, SIGNAL(clicked()), this, SLOT(slots_choseAllCamera()));
	connect(ui.pushButton_choseNoneCamera, SIGNAL(clicked()), this, SLOT(slots_choseNoneCamera()));

	connect(ui.pushButton_choseAllErrorType, SIGNAL(clicked()), this, SLOT(slots_choseAllErrorType()));
	connect(ui.pushButton_choseNoneErrorType, SIGNAL(clicked()), this, SLOT(slots_choseNoneErrorType()));
	connect(ui.checkBox_EquipAlarm,SIGNAL(clicked(bool)),this,SLOT(slots_EquipAlarmCheckBox(bool)));
	connect(ui.ShowAlarmSet_btn,SIGNAL(clicked(bool)),this,SLOT(slots_EquipAlarmInfoShowbtn(bool)));
	connect(ui.btnOk_EquipAlarm,SIGNAL(clicked()),this,SLOT(slots_EquipAlarmSave()));
	connect(ui.btn_ClearAlarm,SIGNAL(clicked()),this,SLOT(slots_EquipAlarmClear()));
	connect(EquipRuntime::Instance(),SIGNAL(SendAlarms(int,bool)),this,SLOT(slots_SetEquipAlarmSatus(int,bool)));
    connect(ui.btnMarkIDInvSel, SIGNAL(clicked()), this, SLOT(slots_ChangedSelection()));
    connect(ui.btnMarkIDSelAll, SIGNAL(clicked()), this, SLOT(slots_ChangedSelection()));
    connect(ui.btnMarkIDSelNO, SIGNAL(clicked()), this, SLOT(slots_ChangedSelection()));
    connect(ui.btnMarkCaviInvSel, SIGNAL(clicked()), this, SLOT(slots_ChangedSelection()));
    connect(ui.btnMarkCaviSelAll, SIGNAL(clicked()), this, SLOT(slots_ChangedSelection()));
    connect(ui.btnMarkCaviSelNO, SIGNAL(clicked()), this, SLOT(slots_ChangedSelection()));
	initInformation();
}

void WidgetTest::initEquipAlarmTablewidget()
{
	ui.tableWidget_EquipAlarm->setColumnWidth(0,22);
	ui.tableWidget_EquipAlarm->setColumnWidth(1,150);
	ui.tableWidget_EquipAlarm->setColumnWidth(2,400);
	for (int i=0;i<ui.tableWidget_EquipAlarm->rowCount();i++)
	{
		QTableWidgetItem *check=new QTableWidgetItem;
		check->setCheckState (Qt::Unchecked);
		check->setFlags(check->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsSelectable));
		ui.tableWidget_EquipAlarm->setItem(i,0,check); //插入复选框
		QTableWidgetItem *m_item1=new QTableWidgetItem;
		m_item1->setTextAlignment(Qt::AlignCenter); 
		ui.tableWidget_EquipAlarm->setItem(i,1,m_item1); 
		QTableWidgetItem *m_item2=new QTableWidgetItem;
		ui.tableWidget_EquipAlarm->setItem(i,2,m_item2); 

		CameraStatusLabel *pLabel = new CameraStatusLabel(this);
		pLabel->setAlignment(Qt::AlignCenter);
		pLabel->setText(QString::number(i+1));
		QFont font ( "Microsoft YaHei", 7, 50); 
		pLabel->setFont(font);
		m_EquipAlarmStatusList<<pLabel;
		if (i < 10 )
			ui.gridLayout_EquipAlarm->addWidget(pLabel,0,i);
		else
			ui.gridLayout_EquipAlarm->addWidget(pLabel,1,i-10);
		
	}
	ui.tableWidget_EquipAlarm->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
	ui.tableWidget_EquipAlarm->horizontalHeader()->setStretchLastSection(true);

	ui.checkBox_EquipAlarm->setChecked(pMainFrm->m_sRuntimeInfo.isEnable);
	for (int i=0;i<pMainFrm->m_sRuntimeInfo.total;i++)
	{
		if (pMainFrm->m_sRuntimeInfo.AlarmsEnable.at(i))
			ui.tableWidget_EquipAlarm->item(i,0)->setCheckState(Qt::Checked);
		else
			ui.tableWidget_EquipAlarm->item(i,0)->setCheckState(Qt::Unchecked);
		ui.tableWidget_EquipAlarm->item(i,1)->setText(QString::number((pMainFrm->m_sRuntimeInfo.AlarmsDays.at(i))));
		ui.tableWidget_EquipAlarm->item(i,2)->setText(pMainFrm->m_sRuntimeInfo.AlarmsInfo.at(i));
	}
	slots_EquipAlarmInfoShowbtn(false);
	slots_EquipAlarmCheckBox(pMainFrm->m_sRuntimeInfo.isEnable);

	if (pMainFrm->m_sRuntimeInfo.isEnable)
	{
		EquipRuntime::Instance()->start();
	}
	else
	{
		EquipRuntime::Instance()->stop();
	}
}

void WidgetTest::initInformation()
{
    QSettings iniCarveSet(pMainFrm->m_sConfigInfo.m_strGrabInfoPath,QSettings::IniFormat);
    iIOCardOffSet[0] = iniCarveSet.value ("/system/iIOCardOffSet", 200).toInt();
    iIOCardOffSet[1] = iniCarveSet.value ("/system/iIOCardOffSet1", 200).toInt();

	if (pMainFrm->m_sSystemInfo.m_bIsIOCardOK)
	{
        for(int i = 0 ; i < pMainFrm->m_sSystemInfo.iIOCardCount; i++)
        {
            auto pCard = pMainFrm->m_vIOCard[i];
            m_nDelay1[i] = pCard->readParam(32);
            m_nDelay2[i] = pCard->readParam(61);
            m_nDelay3[i] = pCard->readParam(57);
            m_nDelay4[i] = pCard->readParam(59);
            m_nDelay5[i] = pCard->readParam(157);
            m_nDelay6[i] = pCard->readParam(158);
            m_nKickDelay[i] = pCard->readParam(49);
            m_nKickWidth[i] = pCard->readParam(46);
            m_nSampleDelay[i] = pCard->readParam(42);
        }
	}
	updateIOCardParam();

}
void WidgetTest::initWidgetName()
{
	ui.widget_LoginHoldTime->setWidgetName(tr("General Parameters"));
	ui.widget_LoginHoldTime->widgetName->setMaximumHeight(25);
	ui.namelayout_LoginHoldTime->addWidget(ui.widget_LoginHoldTime->widgetName);//,Qt::AlignTop);

	ui.widget_saveImageSet->setWidgetName(tr("Save Mode"));
	ui.widget_saveImageSet->widgetName->setMaximumHeight(25);
	ui.namelayout_saveImage->addWidget(ui.widget_saveImageSet->widgetName);//,Qt::AlignTop);

	ui.widget_CameraSurveillance->setWidgetName(tr("Camera Surveillance"));
	ui.widget_CameraSurveillance->widgetName->setMaximumHeight(25);
	ui.namelayout_CameraSurveillance->addWidget(ui.widget_CameraSurveillance->widgetName);

	ui.widget_IOCardSet->setWidgetName(tr("IOCard Delay"));
	ui.widget_IOCardSet->widgetName->setMaximumHeight(25);
	ui.widget_IOCounter->addWidget(ui.widget_IOCardSet->widgetName);

	//modif 2020-11-05 Joge
	ui.widget_EquipAlarm->setWidgetName(tr("Equipment Maintenance Alarm Set"));
}

void WidgetTest::slots_SaveLoginHoldTime()
{
	pMainFrm->m_sSystemInfo.nLoginHoldTime = ui.spinBox_LoginHoldTime->value();

	QSettings iniStatisSet(pMainFrm->m_sConfigInfo.m_strConfigPath,QSettings::IniFormat);
	iniStatisSet.setIniCodec(QTextCodec::codecForName("GBK"));
	iniStatisSet.setValue("/system/nLoginHoldTime",pMainFrm->m_sSystemInfo.nLoginHoldTime);	

}

void WidgetTest::slots_ChoseCamera()
{
	if (0 == iChoseCamera)
	{
		ui.groupBox_ChoseCamera->setVisible(true);
		iChoseCamera = 1;
	}
	else
	{
		ui.groupBox_ChoseCamera->setVisible(false);
		iChoseCamera = 0;
	}
}
void WidgetTest::slots_ChoseErrorType()
{
	if (0 == iChoseErrorType)
	{
		ui.groupBox_ChoseErrorType->setVisible(true);
		iChoseErrorType = 1;
	}
	else
	{
		ui.groupBox_ChoseErrorType->setVisible(false);
		iChoseErrorType = 0;
	}
}
void WidgetTest::slots_OKSave()
{	
	QSettings settingSave(pMainFrm->m_sConfigInfo.m_strConfigPath,QSettings::IniFormat);
	settingSave.setIniCodec(QTextCodec::codecForName("GBK"));
	settingSave.beginGroup("system");
	if (ui.checkBox_saveFailureNormalImage->isChecked())
	{
		pMainFrm->m_sSystemInfo.m_iSaveNormalErrorImageByTime = 1;
		settingSave.setValue("SaveNormalErrorImageByTime",1);
	}
	else{
		pMainFrm->m_sSystemInfo.m_iSaveNormalErrorImageByTime = 0;
		settingSave.setValue("SaveNormalErrorImageByTime",0);
	}
	if (ui.checkBox_saveFailureStressImage->isChecked())
	{
		pMainFrm->m_sSystemInfo.m_iSaveStressErrorImageByTime = 1;
		settingSave.setValue("SaveStressErrorImageByTime",1);
	}
	else{
		pMainFrm->m_sSystemInfo.m_iSaveStressErrorImageByTime = 0;
		settingSave.setValue("SaveStressErrorImageByTime",0);
	}
	settingSave.endGroup();

	if	(iLastSaveMode != iSaveMode||iSaveImgCount != ui.spinBox_Number->value())
	{
		iLastSaveMode = iSaveMode;
		iLastSaveImgCount = iSaveImgCount;

		iSaveImgCount = ui.spinBox_Number->value();
		for (int i = 0;i<pMainFrm->m_sSystemInfo.iCamCount;i++ )
		{
			if (widget_Camera->bIsChosed[i])
			{
				pMainFrm->m_sRunningInfo.m_iSaveImgCount[i] = iSaveImgCount;
			}
			else
			{
				pMainFrm->m_sRunningInfo.m_iSaveImgCount[i] = 0;
			}
		}
		switch (iSaveMode)
		{
		case 0:
			pMainFrm->m_sRunningInfo.m_eSaveImageType = NotSave;
			break;
		case 1:
			pMainFrm->m_sRunningInfo.m_eSaveImageType = AllImageInCount;
			break;
		case 2:
			pMainFrm->m_sRunningInfo.m_eSaveImageType = FailureImageInCount;
			break;
		}
	}
	for (int i = 0; i<CAMERA_MAX_COUNT;i++)
	{
		pMainFrm->m_sSystemInfo.m_bSaveCamera[i] = widget_Camera->bIsChosed[i];
	}
	for (int i = 0; i<ERRORTYPE_MAX_COUNT;i++)
	{
		pMainFrm->m_sSystemInfo.m_bSaveErrorType[i] = widget_ErrorType->bIsChosed[i];
	}
}
void WidgetTest::slots_OKCameraSurveillance()
{
	pMainFrm->m_sSystemInfo.bCameraOffLineSurveillance = ui.checkBox_CameraOffLine->isChecked();
	pMainFrm->m_sSystemInfo.bCameraContinueRejectSurveillance = ui.checkBox_CameraContinueReject->isChecked();
	pMainFrm->m_sSystemInfo.iCamOfflineNo = ui.spinBox_OffLineNumber->value();
	pMainFrm->m_sSystemInfo.iCamContinueRejectNumber = ui.spinBox_RejectNo->value();


	QSettings iniStatisSet(pMainFrm->m_sConfigInfo.m_strConfigPath,QSettings::IniFormat);
	iniStatisSet.setIniCodec(QTextCodec::codecForName("GBK"));

	iniStatisSet.setValue("/system/bCameraOffLineSurveillance",pMainFrm->m_sSystemInfo.bCameraOffLineSurveillance);
	iniStatisSet.setValue("/system/bCameraContinueRejectSurveillance",pMainFrm->m_sSystemInfo.bCameraContinueRejectSurveillance);	
	iniStatisSet.setValue("/system/iCamOfflineNo",pMainFrm->m_sSystemInfo.iCamOfflineNo);
	iniStatisSet.setValue("/system/iCamContinueRejectNumber",pMainFrm->m_sSystemInfo.iCamContinueRejectNumber);

	if(pMainFrm->m_sSystemInfo.bCameraOffLineSurveillance)
	{
		if (CameraOffAlarm->isActive())
		{
			ui.CameraMonitor_Status->setText(tr("Camera Offline monitoring already start"));
			QTimer::singleShot(1000,this,SLOT(SetCameraMonitorStatus()));
			return;
		}
		CameraOffAlarm->start();
		ui.CameraMonitor_Status->setText(tr("Camera Offline monitoring start"));
		QTimer::singleShot(1000,this,SLOT(SetCameraMonitorStatus()));
	}
	else
	{
		if (!CameraOffAlarm->isActive())
		{
			ui.CameraMonitor_Status->setText(tr("Camera Offline monitoring already stop"));
			QTimer::singleShot(1000,this,SLOT(SetCameraMonitorStatus()));
			return;
		}
		CameraOffAlarm->stop();
		ui.CameraMonitor_Status->setText(tr("Camera Offline monitoring stop"));
		QTimer::singleShot(1000,this,SLOT(SetCameraMonitorStatus()));
	}
}

void WidgetTest::slots_CameraOffAlarm()
{
    if(!pMainFrm->m_sSystemInfo.m_bIsIOCardOK)
        return;

	bool isAlarm = false;
    QString losingCamera;
	for(int i=0;i < pMainFrm->m_sSystemInfo.iRealCamCount;i++)
	{
		if(!pMainFrm->m_cCombine[ pMainFrm->m_sRealCamInfo[i].m_iIOCardSN]->b_CombinCamera[i])
        {//如果有相机对应的卡对应的综合不需要综合了，则不需要检测掉线了
            return;
        }

        int imgSN = pMainFrm->ReadImageSignal(i);
        if((imgSN - LastImageNO[i] +256)%256 > 3 && imgSN != 0 )
        {
            LastImageNO[i] = imgSN;
			//Sleep(100);
            if(pMainFrm->m_iGrabCounter[i] == lastGrabCount[i] && pMainFrm->m_iGrabCounter[i] != 0 )
            {
                if(!CameraOFF[i])
                {//如果此相机尚未报警
                    //相机掉线报警
					//CLogFile::write(QString("Img:%1,LastImg:%2,Grab:%3,LastGrab:%4").arg(imgSN).arg(LastImageNO[i]).arg(pMainFrm->m_iGrabCounter[i]).arg(lastGrabCount[i]),CheckLog);
                    pMainFrm->cameraStatus_list.at(i)->SetCameraStatus(2);
                    isAlarm = true;
                    CameraOFF[i] = true;
                    losingCamera += QString("%1,").arg(i);
					CLogFile::write(QString("Camera OffLine:%1").arg(i+1),CheckLog);
                }
            }
            else
            {
                lastGrabCount[i] = pMainFrm->m_iGrabCounter[i];
            }
        }
	}
	if(isAlarm)
    {
        emit signals_ShowWarning(1, QString(tr("Camera %1 Offline ! \nPlease check the camera and restart the software!").arg(losingCamera)));
	}
}

void WidgetTest::SetCameraMonitorStatus()
{
	ui.CameraMonitor_Status->setText("");
}

void WidgetTest::slots_EquipAlarmCheckBox(bool checked)
{
	ui.ShowAlarmSet_btn->setVisible(checked);
	ui.ShowAlarmSet_btn->setChecked(false);
	ui.ShowAlarmSet_btn->setText(tr("Show"));
	if (!checked)
	{
		ui.tableWidget_EquipAlarm->setVisible(checked);
	}
}

void WidgetTest::slots_EquipAlarmInfoShowbtn(bool checked)
{
	ui.tableWidget_EquipAlarm->setVisible(checked);
	if (checked)
		ui.ShowAlarmSet_btn->setText(tr("Hide"));
	else
		ui.ShowAlarmSet_btn->setText(tr("Show"));
}

void WidgetTest::slots_EquipAlarmSave()
{
	QSettings runtimeCfg(pMainFrm->m_sConfigInfo.m_sRuntimePath,QSettings::IniFormat);
	runtimeCfg.setIniCodec(QTextCodec::codecForName("GBK"));
	bool ret = ui.checkBox_EquipAlarm->isChecked();
	pMainFrm->m_sRuntimeInfo.isEnable = ret;
	if (ret)
		runtimeCfg.setValue("EquipAlarm/Enable",1);
	else
		runtimeCfg.setValue("EquipAlarm/Enable",0);
	runtimeCfg.setValue("EquipAlarm/total",20);
	pMainFrm->m_sRuntimeInfo.total =20;
	for (int i=0;i<pMainFrm->m_sRuntimeInfo.total;i++)
	{
		int pchecked = ui.tableWidget_EquipAlarm->item(i,0)->checkState();
		pMainFrm->m_sRuntimeInfo.AlarmsEnable[i] = bool(pchecked);
		pMainFrm->m_sRuntimeInfo.AlarmsDays[i] = ui.tableWidget_EquipAlarm->item(i,1)->text().toInt();
		pMainFrm->m_sRuntimeInfo.AlarmsInfo[i] = ui.tableWidget_EquipAlarm->item(i,2)->text();

		runtimeCfg.setValue(QString("EquipAlarm/Alarm%1_Enable").arg(i+1),ui.tableWidget_EquipAlarm->item(i,0)->checkState());
		runtimeCfg.setValue(QString("EquipAlarm/Alarm%1_Days").arg(i+1),ui.tableWidget_EquipAlarm->item(i,1)->text());
		runtimeCfg.setValue(QString("EquipAlarm/Alarm%1_Info").arg(i+1),ui.tableWidget_EquipAlarm->item(i,2)->text());
	}
	if (pMainFrm->m_sRuntimeInfo.isEnable)
	{
		EquipRuntime::Instance()->start();
	}
	else
	{
		EquipRuntime::Instance()->stop();
	}
}

void WidgetTest::slots_EquipAlarmClear()
{
	EquipRuntime::Instance()->ResetLogFile();
	pMainFrm->sVersion = pMainFrm->getVersion();
}

void WidgetTest::slots_SetEquipAlarmSatus(int index,bool pStatus)
{
	if (pStatus)
	{
		m_EquipAlarmStatusList[index]->SetCameraStatus(1);
	}else{
		m_EquipAlarmStatusList[index]->SetCameraStatus(0);
	}
}

void WidgetTest::slots_setKickModeEnable(int cardSN, bool en)
{
    if(cardSN == 1)
    {
        foreach(auto btn, buttonGroup->buttons())
        {
            btn->setEnabled(en);
        }
    }
    else
    {
        foreach(auto btn, buttonGroup2->buttons())
        {
            btn->setEnabled(en);
        }
    }
}

void WidgetTest::on_cbtnKickMark_clicked(bool st)
{
    ui.gbxMarkIDs->setVisible(st);
    ui.gbxMarkCavity->setVisible(st);
    ui.cbtnManageList->setVisible(st);
}

void WidgetTest::on_btnSaveKickParam_clicked()
{
    if(!ui.cbtnManageList->isChecked())
    {//正常保存参数
        pMainFrm->m_sSystemInfo.bKickReadFailed = ui.cbtnKickFailed->isChecked();
        pMainFrm->m_sSystemInfo.bKickMarked = ui.cbtnKickMark->isChecked();
        pMainFrm->m_sSystemInfo.markedID.clear();
        auto btns = markedIDGroup->buttons();
        foreach(auto btn, btns)
        {
            if(btn->isChecked())
            {
                pMainFrm->m_sSystemInfo.markedID.insert(btn->text());
            }
        }
        btns = markedCaviGroup->buttons();
        foreach(auto btn, btns)
        {
            if(btn->isChecked())
            {
                pMainFrm->m_sSystemInfo.markedCavity.insert(btn->text());
            }
        }
        //保存到文件
        QSettings iniset(pMainFrm->m_sConfigInfo.m_strConfigPath,QSettings::IniFormat);
        iniset.setIniCodec(QTextCodec::codecForName("GBK"));
        iniset.setValue("system/KickReadFailed", pMainFrm->m_sSystemInfo.bKickReadFailed);
        iniset.setValue("system/KickMarked", pMainFrm->m_sSystemInfo.bKickMarked);

        iniset.setValue("system/MarkedIDList", QStringList(pMainFrm->m_sSystemInfo.markedID.toList()).join(","));
        iniset.setValue("system/MarkedCavityList", QStringList(pMainFrm->m_sSystemInfo.markedCavity.toList()).join(","));
    }
    else
    {//保存列表参数
        pMainFrm->m_sSystemInfo.idlist.clear();
        pMainFrm->m_sSystemInfo.cavityList.clear();
        auto btns = markedIDGroup->buttons();
        foreach(auto btn, btns)
        {
            auto id = btn->text();
            if(!btn->isChecked())
            {
                pMainFrm->m_sSystemInfo.idlist.append(id);
            }
            else if(btn->isEnabled())
            {
                pMainFrm->m_sSystemInfo.markedID.remove(id);
                ui.glayMardIDs->removeWidget(btn);
                markedIDGroup->remove(btn);
                delete btn;
            }
        }
        pMainFrm->m_sSystemInfo.idlist.sort();
        btns = markedCaviGroup->buttons();
        foreach(auto btn, btns)
        {
            auto cavity = btn->text();
            if(!btn->isChecked())
            {
                pMainFrm->m_sSystemInfo.cavityList.append(cavity);
            }
            else if(btn->isEnabled())
            {
                pMainFrm->m_sSystemInfo.markedCavity.remove(cavity);
                ui.glayMarkedCavity->removeWidget(btn);
                markedCaviGroup->remove(btn);
                delete btn;
            }
        }
        pMainFrm->m_sSystemInfo.cavityList.sort();
        //保存到文件
        QSettings iniset(pMainFrm->m_sConfigInfo.m_strConfigPath,QSettings::IniFormat);
        iniset.setIniCodec(QTextCodec::codecForName("GBK"));
        iniset.setValue("system/KickReadFailed", pMainFrm->m_sSystemInfo.bKickReadFailed);
        iniset.setValue("system/KickMarked", pMainFrm->m_sSystemInfo.bKickMarked);

        iniset.setValue("system/BottleIDList", pMainFrm->m_sSystemInfo.idlist.join(","));
        iniset.setValue("system/MarkedIDList", QStringList(pMainFrm->m_sSystemInfo.markedID.toList()).join(","));
        iniset.setValue("system/BottleFromCavity", pMainFrm->m_sSystemInfo.cavityList.join(","));
        iniset.setValue("system/MarkedCavityList", QStringList(pMainFrm->m_sSystemInfo.markedCavity.toList()).join(","));
    }
}

void WidgetTest::on_cbtnManageList_clicked(bool st)
{
    ui.ledtID->setVisible(st);
    ui.ledtCavity->setVisible(st);
    ui.btnIDAdd->setVisible(st);
    ui.btnCaviAdd->setVisible(st);
    markedCaviGroup->setManage(st);
    markedIDGroup->setManage(st);
    if(!st)
    {
        slots_reloadKickParams();
    }
}

void WidgetTest::on_btnIDAdd_clicked()
{
    auto id = ui.ledtID->text();
    if(id.isEmpty() || pMainFrm->m_sSystemInfo.idlist.contains(id)|| pMainFrm->m_sSystemInfo.markedID.contains(id))
    {
        return;
    }
    int index = markedIDGroup->buttons().size();
    auto cbtn = new QCheckBox(id);
    ui.glayMardIDs->addWidget(cbtn, index/maxColCnt, index%maxColCnt);
    markedIDGroup->addButton(cbtn);
    pMainFrm->m_sSystemInfo.idlist.append(id);
}

void WidgetTest::on_btnCaviAdd_clicked()
{
    auto id = ui.ledtCavity->text();
    if(id.isEmpty() || pMainFrm->m_sSystemInfo.cavityList.contains(id) || pMainFrm->m_sSystemInfo.markedCavity.contains(id))
    {
        return;
    }
    int index = markedCaviGroup->buttons().size();
    auto cbtn = new QCheckBox(id);
    ui.glayMarkedCavity->addWidget(cbtn, index/maxColCnt, index%maxColCnt);
    markedCaviGroup->addButton(cbtn);
    pMainFrm->m_sSystemInfo.cavityList.append(id);
}

void WidgetTest::slots_ChangedSelection()
{
    if(sender() == ui.btnMarkIDInvSel)
    {
        markedIDGroup->invertSelection();
    }
    else if(sender() == ui.btnMarkIDSelAll)
    {
        markedIDGroup->selectAll();
    }
    else if(sender() == ui.btnMarkIDSelNO)
    {
        markedIDGroup->selectNo();
    }
    else if(sender() == ui.btnMarkCaviInvSel)
    {
        markedCaviGroup->invertSelection();
    }
    else if(sender() == ui.btnMarkCaviSelAll)
    {
        markedCaviGroup->selectAll();
    }
    else if(sender() == ui.btnMarkCaviSelNO)
    {
        markedCaviGroup->selectNo();
    }
}

void WidgetTest::slots_reloadKickParams()
{
    //根据配置加载剔废参数
    ui.cbtnKickFailed->setChecked(pMainFrm->m_sSystemInfo.bKickReadFailed);
    ui.cbtnKickMark->setChecked(pMainFrm->m_sSystemInfo.bKickMarked);
    on_cbtnKickMark_clicked(pMainFrm->m_sSystemInfo.bKickMarked);
    slots_clearKickList();

    ui.cbtnManageList->setChecked(false);
    ui.ledtID->setVisible(false);
    ui.ledtCavity->setVisible(false);;
    ui.btnIDAdd->setVisible(false);
    ui.btnCaviAdd->setVisible(false);
    int index = 0;
    for(int i = 0; i < pMainFrm->m_sSystemInfo.idlist.size();i++)
    {
        auto id = pMainFrm->m_sSystemInfo.idlist[i];
        QCheckBox* cbtn = new QCheckBox(id);
        auto bck = pMainFrm->m_sSystemInfo.markedID.contains(id);
        cbtn->setChecked(bck);
        ui.glayMardIDs->addWidget(cbtn, index/maxColCnt, index%maxColCnt);
        index++;
        markedIDGroup->addButton(cbtn);

    }
    foreach(auto id, pMainFrm->m_sSystemInfo.markedID)
    {
        if(!pMainFrm->m_sSystemInfo.idlist.contains(id))
        {
            QCheckBox* cbtn = new QCheckBox(id);
            cbtn->setChecked(true);
            cbtn->setEnabled(false);
            ui.glayMardIDs->addWidget(cbtn, index/maxColCnt, index%maxColCnt);
            index++;
            markedIDGroup->addButton(cbtn);
        }
    }
    index = 0;
    for(int i = 0; i < pMainFrm->m_sSystemInfo.cavityList.size();i++)
    {
        auto caviNum = pMainFrm->m_sSystemInfo.cavityList[i];
        QCheckBox* cbtn = new QCheckBox(caviNum);
        auto bck =pMainFrm->m_sSystemInfo.markedCavity.contains(caviNum);
        cbtn->setChecked( bck);
        ui.glayMarkedCavity->addWidget(cbtn, index/maxColCnt, index%maxColCnt);
        index++;
        markedCaviGroup->addButton(cbtn);
    }
    foreach(auto caviNum, pMainFrm->m_sSystemInfo.markedCavity)
    {
        if(!pMainFrm->m_sSystemInfo.cavityList.contains(caviNum))
        {
            QCheckBox* cbtn = new QCheckBox(caviNum);
            cbtn->setChecked(true);
            cbtn->setEnabled(false);
            ui.glayMarkedCavity->addWidget(cbtn, index/maxColCnt, index%maxColCnt);
            index++;
            markedCaviGroup->addButton(cbtn);
        }
    }
}

void WidgetTest::slots_clearKickList()
{
    //清空原控件
    auto btns = markedIDGroup->buttons();
    foreach(auto btn, btns)
    {
        ui.glayMardIDs->removeWidget(btn);
        markedIDGroup->removeButton(btn);
        delete btn;
    }
    btns = markedCaviGroup->buttons();
    foreach(auto btn, btns)
    {
        ui.glayMarkedCavity->removeWidget(btn);
        markedCaviGroup->removeButton(btn);
        delete btn;
    }
}

void WidgetTest::slots_Cancel()
{
	//恢复正常踢废
    pMainFrm->SetCardKickMode(0, 2);
    pMainFrm->SetCardKickMode(1, 2);
	switch (pMainFrm->m_sRunningInfo.m_iKickMode[0])
	{
	case 0:  
		ui.radioButton_bad->setChecked(true);
		break;
	case 1:
		ui.radioButton_good->setChecked(true);
		break;	
	case 2:
		ui.radioButton_normal->setChecked(true);
		break;
    }
    switch (pMainFrm->m_sRunningInfo.m_iKickMode[1])
    {
    case 0:  
        ui.radioButton_bad_2->setChecked(true);
        break;
    case 1:
        ui.radioButton_good_2->setChecked(true);
        break;	
    case 2:
        ui.radioButton_normal_2->setChecked(true);
        break;
    }
	//恢复存图
	if (iSaveMode != iLastSaveMode)
	{
		iSaveMode = iLastSaveMode;
		ui.comboBox_SaveMode->setCurrentIndex(iLastSaveMode);
		iSaveImgCount = iLastSaveImgCount;
		ui.spinBox_Number->setValue(iSaveImgCount);
	}
}

void WidgetTest::slots_KickModeChanged(int iMode)
{
    int cardSN = -1;
    if(sender() == buttonGroup)
    {
        cardSN = 0;
    }
    else if(sender() == buttonGroup2)
    {
        cardSN = 1;
    }
    else
    {
        return;
    }
    auto iLastKickMode = pMainFrm->m_sRunningInfo.m_iKickMode[cardSN];
	if (iMode != iLastKickMode)
	{
		pMainFrm->SetCardKickMode(cardSN, iMode);
        if(cardSN == 0)
        {
            switch (iMode)
            {
            case 0:
                ui.radioButton_bad->setChecked(true);
                break;
            case 1:
                ui.radioButton_good->setChecked(true);
                break;
            default:
                ui.radioButton_normal->setChecked(true);
                break;
            }
        }
        else
        {
            switch (iMode)
            {
            case 0:
                ui.radioButton_bad_2->setChecked(true);
                break;
            case 1:
                ui.radioButton_good_2->setChecked(true);
                break;
            default:
                ui.radioButton_normal_2->setChecked(true);
                break;
            }
        }
	}
}

void WidgetTest::slots_SaveModeChanged(int index)
{
	iSaveMode = index;
	if (1 == iSaveMode)
	{
		ui.label_Number->setEnabled(true);
		ui.spinBox_Number->setEnabled(true);
		if (1 == iChoseErrorType)
		{
			slots_ChoseErrorType();
		}
		if (0 == iChoseCamera)
		{
			slots_ChoseCamera();
		}
	}
	else if (2 == iSaveMode)
	{
		ui.label_Number->setEnabled(true);
		ui.spinBox_Number->setEnabled(true);
		if (0 == iChoseErrorType)
		{
			slots_ChoseErrorType();
		}
		if (0 == iChoseCamera)
		{
			slots_ChoseCamera();
		}
	}
	else
	{
		ui.label_Number->setEnabled(false);
		ui.spinBox_Number->setEnabled(false);
		if (1 == iChoseErrorType)
		{
			slots_ChoseErrorType();
		}
		if (1 == iChoseCamera)
		{
			slots_ChoseCamera();
		}
	}
}
void WidgetTest::slots_readDelay()
{
    int cardNO = 0;
    if(sender() == ui.read)
    {
        cardNO = 0;
    }
    else if(sender() == ui.read_2)
    {
        cardNO = 1;
    }
    else{
        return;
    }
	if (pMainFrm->m_sSystemInfo.m_bIsIOCardOK)
	{
        auto pCard = pMainFrm->m_vIOCard[cardNO];
        int i,j,k;
        i= pCard->readStatus(13);
        j= pCard->readStatus(46);
        k= pCard->readStatus(15);
        m_nKickWidth[cardNO] = pCard->readParam(46);
        m_nKickDelay[cardNO] = (65536 +j-i-iIOCardOffSet[cardNO])%65536;
        m_nSampleDelay[cardNO] = 0;
        if(cardNO == 0)
        {
            //card1
            ui.KickDelay->setText(QString::number(m_nKickDelay[0]));
            ui.lineDelay->setText(QString::number(m_nSampleDelay[0]));
            ui.KickWidth->setText(QString::number(m_nKickWidth[0]));
        }
        else{
            //card2
            ui.KickDelay_2->setText(QString::number(m_nKickDelay[1]));
            ui.lineDelay_2->setText(QString::number(m_nSampleDelay[1]));
            ui.KickWidth_2->setText(QString::number(m_nKickWidth[1]));
        }
	}
}
void WidgetTest::slots_setToCard()
{
    int cardNO = 0;
    if(sender() == ui.settocard)
    {
        cardNO = 0;
    }
    else if(sender() == ui.settocard_2)
    {
        cardNO = 1;
    }
    else{
        return;
    }
    getIOCardParam(cardNO);
    auto pCard = pMainFrm->m_vIOCard[cardNO];
	if (m_nDelay1[cardNO]>0xFFFF || m_nDelay2[cardNO]>0xFFFF || m_nDelay3[cardNO]>0xFFFF || 
        m_nDelay4[cardNO]>0xFFFF || m_nDelay5[cardNO]>0xFFFF || m_nDelay6[cardNO]>0xFFFF || 
        m_nKickWidth[cardNO]>0xFFFF||m_nKickDelay[cardNO]>0xFFFF)
	{
		QString str(tr("The set value is out of range!"));
		QMessageBox::information(this,"Error",str);	
		return; 
	}
	if (pMainFrm->m_sSystemInfo.m_bIsIOCardOK)
    {
        pCard->writeParam(32,m_nDelay1[cardNO]);
        pCard->writeParam(61,m_nDelay2[cardNO]);
        pCard->writeParam(57,m_nDelay3[cardNO]);
        pCard->writeParam(59,m_nDelay4[cardNO]);
        pCard->writeParam(157,m_nDelay5[cardNO]);
        pCard->writeParam(158,m_nDelay6[cardNO]);
		pCard->writeParam(49,m_nKickDelay[cardNO]);
		pCard->writeParam(46,m_nKickWidth[cardNO]);
		pCard->writeParam(42,m_nSampleDelay[cardNO]);
	}
}
void WidgetTest::slots_setToFile()
{
    int cardNO = 0;
    if(sender() == ui.settofile)
    {
        cardNO = 0;
    }
    else if(sender() == ui.settofile_2)
    {
        cardNO = 1;
    }
    else{
        return;
    }
    getIOCardParam(cardNO);
	if (m_nDelay1[cardNO]>0xFFFF||m_nDelay2[cardNO]>0xFFFF||m_nDelay3[cardNO]>0xFFFF||m_nDelay4[cardNO]>0xFFFF||
        m_nDelay5[cardNO]>0xFFFF||m_nDelay6[cardNO]>0xFFFF||m_nKickWidth[cardNO]>0xFFFF||m_nKickDelay[cardNO]>0xFFFF||
        m_nSampleDelay[cardNO]>0xFFFF||m_nSampleWidth[cardNO]>0xFFFF)
	{
		QString str(tr("The set value is out of range!"));
		QMessageBox::information(this,"Error",str);	
		return;
	}
    QString strValue,strPara;
    strValue = strValue.setNum(m_nDelay1[cardNO],10);
    strPara = strPara.setNum(32,10);
    StateTool::WritePrivateProfileQString("PIO24B",strPara,strValue,pMainFrm->m_sSystemInfo.m_sConfigIOCardInfo[cardNO].strCardInitFile);

    strValue = strValue.setNum(m_nDelay2[cardNO],10);
    strPara = strPara.setNum(61,10);
    StateTool::WritePrivateProfileQString("PIO24B",strPara,strValue,pMainFrm->m_sSystemInfo.m_sConfigIOCardInfo[cardNO].strCardInitFile);

    strValue = strValue.setNum(m_nDelay3[cardNO],10);
    strPara = strPara.setNum(57,10);
    StateTool::WritePrivateProfileQString("PIO24B",strPara,strValue,pMainFrm->m_sSystemInfo.m_sConfigIOCardInfo[cardNO].strCardInitFile);

    strValue = strValue.setNum(m_nDelay4[cardNO],10);
    strPara = strPara.setNum(59,10);
    StateTool::WritePrivateProfileQString("PIO24B",strPara,strValue,pMainFrm->m_sSystemInfo.m_sConfigIOCardInfo[cardNO].strCardInitFile);

    strValue = strValue.setNum(m_nDelay5[cardNO],10);
    strPara = strPara.setNum(157,10);//
    StateTool::WritePrivateProfileQString("PIO24B",strPara,strValue,pMainFrm->m_sSystemInfo.m_sConfigIOCardInfo[cardNO].strCardInitFile);

    strValue = strValue.setNum(m_nDelay6[cardNO],10);
    strPara = strPara.setNum(158,10);
    StateTool::WritePrivateProfileQString("PIO24B",strPara,strValue,pMainFrm->m_sSystemInfo.m_sConfigIOCardInfo[cardNO].strCardInitFile);
	strValue = strValue.setNum(m_nKickDelay[cardNO],10);
	strPara = strPara.setNum(49,10);
	StateTool::WritePrivateProfileQString("PIO24B",strPara,strValue,pMainFrm->m_sSystemInfo.m_sConfigIOCardInfo[cardNO].strCardInitFile);

	strValue = strValue.setNum(m_nKickWidth[cardNO],10);
	strPara = strPara.setNum(46,10);

	StateTool::WritePrivateProfileQString("PIO24B",strPara,strValue,pMainFrm->m_sSystemInfo.m_sConfigIOCardInfo[cardNO].strCardInitFile);

	strValue = strValue.setNum(m_nSampleDelay[cardNO],10);
	strPara = strPara.setNum(42,10);
	StateTool::WritePrivateProfileQString("PIO24B",strPara,strValue,pMainFrm->m_sSystemInfo.m_sConfigIOCardInfo[cardNO].strCardInitFile);

	QSettings iniCarveSet(pMainFrm->m_sConfigInfo.m_strGrabInfoPath,QSettings::IniFormat);
    QString strSession = QString("/system/iIOCardOffSet%1").arg(cardNO);
    if(cardNO == 0)
    {
        strSession = QString("/system/iIOCardOffSet");
    }
	iniCarveSet.setValue (strSession, iIOCardOffSet[cardNO]);
}
void WidgetTest::slots_advance1()
{
    int cardNO = 0;
    if(sender() == ui.advance1)
    {
        cardNO = 0;
    }
    else if(sender() == ui.advance1_2)
    {
        cardNO = 1;
    }
    else{
        return;
    }
    auto pCard = pMainFrm->m_vIOCard[cardNO];
    if (pMainFrm->m_sSystemInfo.m_bIsIOCardOK)
	{
		int res = pCard->Show_PIO24B_DebugDialog();
	}
 	/*else
 	{
 		QMessageBox::information(this,"ERROR","IOCard is not available");
 	}*/
}
void WidgetTest::updateIOCardParam()
{
	QString str;
    //Card1
	str = str.setNum(m_nDelay1[0],10);
	ui.Delay1->setText(str);
	str = str.setNum(m_nDelay2[0],10);
	ui.Delay2->setText(str);
	str = str.setNum(m_nDelay3[0],10);
	ui.Delay3->setText(str);
	str = str.setNum(m_nDelay4[0],10);
	ui.Delay4->setText(str);
	str = str.setNum(m_nDelay5[0],10);
	ui.Delay5->setText(str);
	str = str.setNum(m_nDelay6[0],10);
	ui.Delay6->setText(str);

	str = str.setNum(m_nSampleDelay[0],10);
	ui.lineDelay->setText(str);

	str = str.setNum(m_nKickDelay[0],10);
	ui.KickDelay->setText(str);
	str = str.setNum(m_nKickWidth[0],10);
	ui.KickWidth->setText(str);
    ui.Distance1to4->setText(QString::number(iIOCardOffSet[0]));
    //Card2
    str = str.setNum(m_nDelay1[1],10);
    ui.Delay1_2->setText(str);
    str = str.setNum(m_nDelay2[1],10);
    ui.Delay2_2->setText(str);
    str = str.setNum(m_nDelay3[1],10);
    ui.Delay3_2->setText(str);
    str = str.setNum(m_nDelay4[1],10);
    ui.Delay4_2->setText(str);
    str = str.setNum(m_nDelay5[1],10);
    ui.Delay5_2->setText(str);
    str = str.setNum(m_nDelay6[1],10);
    ui.Delay6_2->setText(str);

    str = str.setNum(m_nSampleDelay[1],10);
    ui.lineDelay_2->setText(str);

    str = str.setNum(m_nKickDelay[1],10);
    ui.KickDelay_2->setText(str);
    str = str.setNum(m_nKickWidth[1],10);
    ui.KickWidth_2->setText(str);
    ui.Distance1to4_2->setText(QString::number(iIOCardOffSet[1]));
}
void WidgetTest::getIOCardParam(int cardid)
{
    if(cardid == 0)
    {
        m_nDelay1[cardid] = ui.Delay1->text().toInt();
        m_nDelay2[cardid] = ui.Delay2->text().toInt();
        m_nDelay3[cardid] = ui.Delay3->text().toInt();
        m_nDelay4[cardid] = ui.Delay4->text().toInt();
        m_nDelay5[cardid] = ui.Delay5->text().toInt();
        m_nDelay6[cardid] = ui.Delay6->text().toInt();
        m_nSampleDelay[cardid] = ui.lineDelay->text().toInt();
        m_nKickDelay[cardid] = ui.KickDelay->text().toInt();
        m_nKickWidth[cardid] = ui.KickWidth->text().toInt();
        iIOCardOffSet[cardid] = ui.Distance1to4->text().toInt();
    }
    else//if(cardid == 1)
    {
        m_nDelay1[cardid] = ui.Delay1_2->text().toInt();
        m_nDelay2[cardid] = ui.Delay2_2->text().toInt();
        m_nDelay3[cardid] = ui.Delay3_2->text().toInt();
        m_nDelay4[cardid] = ui.Delay4_2->text().toInt();
        m_nDelay5[cardid] = ui.Delay5_2->text().toInt();
        m_nDelay6[cardid] = ui.Delay6_2->text().toInt();
        m_nSampleDelay[cardid] = ui.lineDelay_2->text().toInt();
        m_nKickDelay[cardid] = ui.KickDelay_2->text().toInt();
        m_nKickWidth[cardid] = ui.KickWidth_2->text().toInt();
        iIOCardOffSet[cardid] = ui.Distance1to4_2->text().toInt();
    }
}

void WidgetTest::slots_choseAllCamera()
{
	for (int i=0;i<widget_Camera->listCheckBox.length();i++)
	{
		widget_Camera->listCheckBox.at(i)->setChecked(true);
	}
}
void WidgetTest::slots_choseNoneCamera()
{
	for (int i=0;i<widget_Camera->listCheckBox.length();i++)
	{
		widget_Camera->listCheckBox.at(i)->setChecked(false);
	}
}
void WidgetTest::slots_choseAllErrorType()
{
	for (int i=0;i<widget_ErrorType->listCheckBox.length();i++)
	{
		widget_ErrorType->listCheckBox.at(i)->setChecked(true);
	}
}
void WidgetTest::slots_choseNoneErrorType()
{
	for (int i=0;i<widget_ErrorType->listCheckBox.length();i++)
	{
		widget_ErrorType->listCheckBox.at(i)->setChecked(false);
	}
}

void WidgetTest::slots_updateIOcardCounter()
{
	if (pMainFrm->m_sSystemInfo.m_bIsIOCardOK)
	{
        for(int i = 0; i < pMainFrm->m_sSystemInfo.iIOCardCount;i++)
        {
            auto pCard = pMainFrm->m_vIOCard[i];
            int iCounter;
            iCounter = pCard->ReadCounter(0);
            pMainFrm->nIOCard[i*24+0] = iCounter;
            iCounter = pCard->ReadCounter(1);
            pMainFrm->nIOCard[i*24+1] = iCounter;
            iCounter = pCard->ReadCounter(2);
            pMainFrm->nIOCard[i*24+2] = iCounter;
            iCounter = pCard->ReadCounter(3);
            pMainFrm->nIOCard[i*24+3] = iCounter;
            iCounter = pCard->ReadCounter(4);
            pMainFrm->nIOCard[i*24+4] = iCounter;
            iCounter = pCard->ReadCounter(5);
            pMainFrm->nIOCard[i*24+5] = iCounter;
            iCounter = pCard->ReadCounter(6);
            pMainFrm->nIOCard[i*24+6] = iCounter;
            iCounter = pCard->ReadCounter(7);
            pMainFrm->nIOCard[i*24+7] = iCounter;
            iCounter = pCard->ReadCounter(9)%0x100;
            pMainFrm->nIOCard[i*24+8] = iCounter;
            iCounter = pCard->ReadCounter(9)/0x100;
            pMainFrm->nIOCard[i*24+9] = iCounter;
            iCounter = pCard->ReadCounter(10)%0x100;
            pMainFrm->nIOCard[i*24+10] = iCounter;
            iCounter = pCard->ReadCounter(10)/0x100;
            pMainFrm->nIOCard[i*24+11] = iCounter;
            pMainFrm->nIOCard[i*24+12] = pMainFrm->m_sRunningInfo.nTotal[i];
            pMainFrm->nIOCard[i*24+13] = pMainFrm->m_sRunningInfo.nKick[i];
            pMainFrm->nIOCard[i*24+14] = pMainFrm->m_sRunningInfo.nCompensateKick[i];
            pMainFrm->nIOCard[i*24+15] = pMainFrm->m_sRunningInfo.nCompensateCardKick[i];
            if(i == 0)
            {
                iCounter = pCard->ReadCounter(16);
                ui.label_frequency->setText(tr("Frequency:")+QString::number(iCounter));
                ui.label_IN0->setText(tr("IN0:")+QString::number(pMainFrm->nIOCard[i*24+0]));
                ui.label_IN1->setText(tr("IN1:")+QString::number(pMainFrm->nIOCard[i*24+1]));
                ui.label_IN2->setText(tr("IN2:")+QString::number(pMainFrm->nIOCard[i*24+2]));
                ui.label_IN3->setText(tr("IN3:")+QString::number(pMainFrm->nIOCard[i*24+3]));

                ui.label_OUT0->setText(tr("OUT0:")+QString::number(pMainFrm->nIOCard[i*24+4]));
                ui.label_OUT1->setText(tr("OUT1:")+QString::number(pMainFrm->nIOCard[i*24+5]));
                ui.label_OUT2->setText(tr("OUT2:")+QString::number(pMainFrm->nIOCard[i*24+6]));
                ui.label_OUT3->setText(tr("OUT3:")+QString::number(pMainFrm->nIOCard[i*24+7]));
                ui.label_OUT4->setText(tr("OUT4:")+QString::number(pMainFrm->nIOCard[i*24+8]));
                ui.label_OUT5->setText(tr("OUT5:")+QString::number(pMainFrm->nIOCard[i*24+9]));
                ui.label_OUT6->setText(tr("OUT6:")+QString::number(pMainFrm->nIOCard[i*24+10]));
                ui.label_OUT7->setText(tr("OUT7:")+QString::number(pMainFrm->nIOCard[i*24+11]));
                //记录接口中的数据 by zl
                iCounter = pCard->ReadCounter(37);
                ui.label_23->setText(tr("Card Compensate Kick:")+QString::number(iCounter));		//接口卡补踢总数
                //软件综合计数
                ui.label_15->setText(tr("Combine Total:")+QString::number(pMainFrm->m_sRunningInfo.nTotal[i])); //综合过检总数
                ui.label_24->setText(tr("Combine Kick:")+QString::number(pMainFrm->m_sRunningInfo.nKick[i]));  //综合过检踢废
                ui.label_28->setText(tr("Compensate Total:")+QString::number(pMainFrm->m_sRunningInfo.nCompensateKick[i])); //补踢过检总数
                ui.label_29->setText(tr("Compensate Kick:")+QString::number(pMainFrm->m_sRunningInfo.nCompensateCardKick[i]));  //补踢踢废总数
            }
            else
            {
                iCounter = pCard->ReadCounter(16);
                ui.label_frequency_2->setText(tr("Frequency:")+QString::number(iCounter));
                ui.label_IN0_2->setText(tr("IN0:")+QString::number(pMainFrm->nIOCard[i*24+0]));
                ui.label_IN1_2->setText(tr("IN1:")+QString::number(pMainFrm->nIOCard[i*24+1]));
                ui.label_IN2_2->setText(tr("IN2:")+QString::number(pMainFrm->nIOCard[i*24+2]));
                ui.label_IN3_2->setText(tr("IN3:")+QString::number(pMainFrm->nIOCard[i*24+3]));

                ui.label_OUT0_2->setText(tr("OUT0:")+QString::number(pMainFrm->nIOCard[i*24+4]));
                ui.label_OUT1_2->setText(tr("OUT1:")+QString::number(pMainFrm->nIOCard[i*24+5]));
                ui.label_OUT2_2->setText(tr("OUT2:")+QString::number(pMainFrm->nIOCard[i*24+6]));
                ui.label_OUT3_2->setText(tr("OUT3:")+QString::number(pMainFrm->nIOCard[i*24+7]));
                ui.label_OUT4_2->setText(tr("OUT4:")+QString::number(pMainFrm->nIOCard[i*24+8]));
                ui.label_OUT5_2->setText(tr("OUT5:")+QString::number(pMainFrm->nIOCard[i*24+9]));
                ui.label_OUT6_2->setText(tr("OUT6:")+QString::number(pMainFrm->nIOCard[i*24+10]));
                ui.label_OUT7_2->setText(tr("OUT7:")+QString::number(pMainFrm->nIOCard[i*24+11]));
                //记录接口中的数据 by zl
                iCounter = pCard->ReadCounter(37);
                ui.label_26->setText(tr("Card Compensate Kick:")+QString::number(iCounter));		//接口卡补踢总数
            }
        }
	}
}

void WidgetTest::slots_updateAlert(int i)
{
}
void WidgetTest::slots_ifCheckShowImage()
{
	ifshowImage=ui.comboBox->currentIndex();
}