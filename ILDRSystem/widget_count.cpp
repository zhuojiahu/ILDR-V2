#include "widget_count.h"
#include <QDebug>
#include <QSignalMapper>
#include <QMessageBox>

#include "database.h"
#include "ILDRSystem.h"

extern SysMainUI *pMainFrm;


widget_count::widget_count(QWidget *parent)
	:QWidget(parent)
{
	ui.setupUi(this);
	init();
}

widget_count::~widget_count()
{

}

int widget_count::GetCurrentReportMode()
{
	return ui.stackedWidget_2->currentIndex();
}

void widget_count::init()
{
	//注册Qt元对象
	qRegisterMetaType<QList<cErrorTypeCountInfo>>("QList<cErrorTypeCountInfo>"); 
	qRegisterMetaType<QList<cErrorCountbyTime>>("QList<cErrorCountbyTime>");
	qRegisterMetaType<cErrorCountbyTime>("cErrorCountbyTime");
	qRegisterMetaType<QList<int>>("QList<int>");

	ui.stackedWidget->setCurrentIndex(0);
	//小标题设置
	ui.widget->setWidgetName(tr("Count Table"));
	ui.widget_countInfo->setWidgetName(tr("Count Info"));
	ui.widget_saveRecord->setWidgetName(tr("Save Set"));
	ui.widget_ShiftSet->setWidgetName(tr("Shift Set"));
	ui.widget_HistotySearch->setWidgetName(tr("History Search"));
	ui.widget_TimePie->setWidgetName(tr("Defect Count - Every hour or half an hour"));
	ui.widget_shiftSearch->setWidgetName(tr("Shift Selete"));
	ui.widget_table->setWidgetName(tr("Search Table Type"));
	ui.widget_ShiftPie->setWidgetName(tr("Detect Count - Shift"));
	ui.widget_3->setWidgetName(tr("MoldNo count"));
	//添加切换页按钮
	buttonTurn = new QPushButton; 
	QPixmap iconTurn(":/sysButton/turnImage");
	buttonTurn->setIcon(iconTurn);
	buttonTurn->setFixedSize(iconTurn.size());
	QHBoxLayout *nameLayout = new QHBoxLayout(ui.widget);
	nameLayout->addWidget(ui.widget->widgetName);
	nameLayout->addStretch();
	nameLayout->addWidget(buttonTurn);
	nameLayout->setSpacing(5);
	nameLayout->setContentsMargins(5,0,5,0);

	//初始化模号统计
	QSignalMapper *signal_mapper = new QSignalMapper(this);
	QGridLayout *m_MoldNoLayout=new QGridLayout;
	m_MoldNoLayout->addRowSpacing(0,20);
	for(int i=0; i<MOLDNO_MAX_COUNT; i++)
	{ 
		QCheckBox *m_MoldNoCheckBox = new QCheckBox;
		m_MoldNoCheckBox->setText(QString("%1").arg(i));
		m_iMoldNoCheckBoxs.append(m_MoldNoCheckBox);
		int row = i / 10 +1;
		int col = i % 10 ;
		m_MoldNoLayout->addWidget(m_MoldNoCheckBox,row,col,1,1);	
		connect(m_MoldNoCheckBox, SIGNAL(clicked()), signal_mapper, SLOT(map()));
		signal_mapper->setMapping(m_MoldNoCheckBox, i);
		if (i>90)
		{
			m_MoldNoCheckBox->setEnabled(false);
		}
	}
	ui.widget_3->setLayout(m_MoldNoLayout);
	connect(signal_mapper, SIGNAL(mapped(int)), this, SLOT(slot_MoldNoCheckBox_clicked(int)));
	ui.tableView_3->setEditTriggers(QAbstractItemView::NoEditTriggers);		//禁止编辑
	ui.tableView_3->setSelectionBehavior(QAbstractItemView::SelectRows);	//整行选中
	ui.tableView_3->setSelectionMode(QAbstractItemView::SingleSelection);	//单个选中目标
	ui.tableView_3->verticalHeader()->setVisible(false);					//隐藏行头
	table3Model=new QStandardItemModel;
	ui.tableView_3->setModel(table3Model);

	ui.tableView_4->setEditTriggers(QAbstractItemView::NoEditTriggers);		//禁止编辑
	ui.tableView_4->setSelectionBehavior(QAbstractItemView::SelectRows);	//整行选中
	ui.tableView_4->setSelectionMode(QAbstractItemView::SingleSelection);	//单个选中目标
	ui.tableView_4->verticalHeader()->setVisible(false);					//隐藏行头
	table4Model=new QStandardItemModel;
	ui.tableView_4->setModel(table4Model);

	ui.tableView_5->setEditTriggers(QAbstractItemView::NoEditTriggers);		//禁止编辑
	ui.tableView_5->setSelectionBehavior(QAbstractItemView::SelectRows);	//整行选中
	ui.tableView_5->setSelectionMode(QAbstractItemView::SingleSelection);	//单个选中目标
	ui.tableView_5->verticalHeader()->setVisible(false);					//隐藏行头
	table5Model=new QStandardItemModel;
	ui.tableView_5->setModel(table5Model);

	//初始化实时显示的缺陷统计表
	ui.tableView_1->setEditTriggers(QAbstractItemView::NoEditTriggers);		//禁止编辑
	ui.tableView_1->setSelectionBehavior(QAbstractItemView::SelectRows);	//整行选中
	ui.tableView_1->setSelectionMode(QAbstractItemView::SingleSelection);	//单个选中目标
	ui.tableView_1->verticalHeader()->setVisible(false);					//隐藏行头
	table1Model=new QStandardItemModel;
	QStringList headerList;
	headerList<<tr("Type");
	headerList<<tr("Count");
	headerList<<tr("Front");
	headerList<<tr("Clamp");
	headerList<<tr("Rear");

	table1Model->setHorizontalHeaderLabels(headerList);
	ui.tableView_1->setModel(table1Model);
	ui.tableView_1->setColumnWidth(0,120);									//设置第一列列宽

	ui.tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);		//禁止编辑
	ui.tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows);	//整行选中
	ui.tableView_2->setSelectionMode(QAbstractItemView::SingleSelection);	//单个选中目标
	ui.tableView_2->verticalHeader()->setVisible(false);					//隐藏行头
	table2Model=new QStandardItemModel;
	headerList.clear();
	headerList<<tr("DateTime");
	headerList<<tr("AllCheck");
	headerList<<tr("AllReject");
	headerList<<tr("Reject Rate");
	headerList<<tr("Front");
	headerList<<tr("Clamp");
	headerList<<tr("Rear");
	
	table2Model->setHorizontalHeaderLabels(headerList);
	ui.tableView_2->setModel(table2Model);
	ui.tableView_2->setColumnWidth(0,120);									//设置第一列列宽

	//绘制饼图
	PieItemcolors<<QColor(153,230,0)
				 <<QColor(174,77,102)
				 <<QColor(128,255,128)
				 <<QColor(128,255,255)
				 <<QColor(0,128,255)
				 <<QColor(255,255,128)
				 <<QColor(0,248,128)
				 <<QColor(255,0,213)
				 <<QColor(35,0,255)
				 <<QColor(255,129,0);
	pieImage1=new PieView();
	PieModel_1 = new QStandardItemModel(10, 2, this);
	PieModel_1->setHeaderData(0, Qt::Horizontal, tr("Error1"));
	PieModel_1->setHeaderData(1, Qt::Horizontal, tr("Error2"));
	PieModel_1->setHeaderData(2, Qt::Horizontal, tr("Error3"));
	PieModel_1->setHeaderData(3, Qt::Horizontal, tr("Error4"));
	PieModel_1->setHeaderData(4, Qt::Horizontal, tr("Error5"));
	PieModel_1->setHeaderData(5, Qt::Horizontal, tr("Error6"));
	PieModel_1->setHeaderData(6, Qt::Horizontal, tr("Error7"));
	PieModel_1->setHeaderData(7, Qt::Horizontal, tr("Error8"));
	PieModel_1->setHeaderData(8, Qt::Horizontal, tr("Error9"));
	PieModel_1->setHeaderData(9, Qt::Horizontal, tr("Error10"));
	pieImage1->setModel(PieModel_1);
	QVBoxLayout *m_pielayout = new QVBoxLayout(ui.widget_pie1);
	m_pielayout->addWidget(pieImage1);
	m_pielayout->setMargin(4);

	pieImage2=new PieView();
	PieModel_2 = new QStandardItemModel(10, 2, this);
	PieModel_2->setHeaderData(0, Qt::Horizontal, tr("Error1"));
	PieModel_2->setHeaderData(1, Qt::Horizontal, tr("Error2"));
	PieModel_2->setHeaderData(2, Qt::Horizontal, tr("Error3"));
	PieModel_2->setHeaderData(3, Qt::Horizontal, tr("Error4"));
	PieModel_2->setHeaderData(4, Qt::Horizontal, tr("Error5"));
	PieModel_2->setHeaderData(5, Qt::Horizontal, tr("Error6"));
	PieModel_2->setHeaderData(6, Qt::Horizontal, tr("Error7"));
	PieModel_2->setHeaderData(7, Qt::Horizontal, tr("Error8"));
	PieModel_2->setHeaderData(8, Qt::Horizontal, tr("Error9"));
	PieModel_2->setHeaderData(9, Qt::Horizontal, tr("Error10"));
	pieImage2->setModel(PieModel_2);
	QVBoxLayout *m_pielayout_2 = new QVBoxLayout(ui.widget_pie2);
	m_pielayout_2->addWidget(pieImage2);
	m_pielayout_2->setMargin(4);

	//初始化直方图
	QLinearGradient gradient(0, 0, 0, 400);
	gradient.setColorAt(0, QColor(90, 90, 90));
	gradient.setColorAt(0.38, QColor(105, 105, 105));
	gradient.setColorAt(1, QColor(70, 70, 70));
	ui.CustomPlot->setBackground(QBrush(gradient));

	//设置countInfo默认值
	slot_saveRecordCancel_clicked();
;	slot_shiftCancel_clicked();

	connect(buttonTurn,SIGNAL(clicked()),this,SLOT(slots_turnPage()));
	connect(ui.pushButton_saveRecordOK,SIGNAL(clicked()),this,SLOT(slot_saveRecordOK_clicked()));
	connect(ui.pushButton_saveRecordCancel,SIGNAL(clicked()),this,SLOT(slot_saveRecordCancel_clicked()));
	connect(ui.pushButton_shiftOK,SIGNAL(clicked()),this,SLOT(slot_shiftOK_clicked()));
	connect(ui.pushButton_shiftCancel,SIGNAL(clicked()),this,SLOT(slot_shiftCancel_clicked()));
	connect(ui.Btn_HistorySearch,SIGNAL(clicked()),this,SLOT(slot_HistorySearch_clicked()));
	connect(ui.tableView_2, SIGNAL(clicked(QModelIndex)), this, SLOT(slots_ShowPie(QModelIndex)));
	connect(ui.pushButton_openRecord,SIGNAL(clicked()),this,SLOT(slot_OpenRecord_clicked()));
	connect(ui.pushButton_deleteRecord,SIGNAL(clicked()),this,SLOT(slot_DeleteRecord_clicked()));
	connect(ui.Btn_OpenExcel,SIGNAL(clicked()),this,SLOT(slot_OpenExcel_clicked()));
	connect(ui.pushButton_Search,SIGNAL(clicked()),this,SLOT(slot_SearchShift_clicked()));
	connect(this,SIGNAL(updateMoldNo(QList<int>,bool)),this,SLOT(slot_ShowMoldNoCount(QList<int>,bool)));
	connect(ui.comboBox_2,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_changeReport(int)));
	connect(ui.tableView_4, SIGNAL(clicked(QModelIndex)), this, SLOT(slot_updateMoldNoTable2(QModelIndex)));

	ExportThread=new ExportExcelThread(this);

	//init
	slot_changeReport(0);

// 	MoldNoTimer = new QTimer(this);
// 	connect(MoldNoTimer,SIGNAL(timeout()),this,SLOT(slot_updateMoldNoCount()));
// 	MoldNoTimer->setInterval(1000);
// 	MoldNoTimer->start();
}

DWORD widget_count::GetProcessIdFromName(const char*processName)    
{
	PROCESSENTRY32 pe;
	DWORD id = 0;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	if( !Process32First(hSnapshot,&pe) )
		return 0;
	char pname[300];
	do
	{
		pe.dwSize = sizeof(PROCESSENTRY32);
		if( Process32Next(hSnapshot,&pe)==FALSE )
			break;
		//把WCHAR*类型转换为const char*类型
		sprintf_s(pname,"%ws",pe.szExeFile,260);
		//比较两个字符串，如果找到了要找的进程
		if(strcmp(pname,processName) == 0)
		{
			id = pe.th32ProcessID;
			break;
		}

	} while(1);
	CloseHandle(hSnapshot);
	return id;
}

QTime widget_count::initTime(QTime pTime)
{
	int phour = pTime.hour();
	int pminute = pTime.minute();
	if(pMainFrm->m_sSystemInfo.iSaveRecordInterval == 60)
	{
		if (pminute >0)
		{
			if (phour != 23)
			{
				phour += 1;
			}
			pminute = 0;
		}
	}
	else
	{
		if (pminute > 0 && pminute < 30)
		{
			pminute = 30;
		}
		else if ( pminute > 30 )
		{
			if (phour != 23)
			{
				phour += 1;
				pminute = 0;
			}	
			else
			{
				pminute = 30;
			}
		}
	}
	QString timeStr = QString("%1:%2").arg(phour).arg(pminute);

	return QTime::fromString(timeStr,"h:m");
}

void widget_count::openInNotePad(QString str)
{
	DWORD pid = GetProcessIdFromName("notepad.exe");
	//获取进程的最大权限
	if(pid!=0)
	{
		HANDLE token = OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);
		//关闭进程
		TerminateProcess(token, 0);
	}
	ShellExecuteA(NULL,"open",(LPCSTR)str.toLocal8Bit(),NULL,NULL,SW_SHOW);
}

void widget_count::slots_turnPage()
{
	if (0 == ui.stackedWidget->currentIndex())
		ui.stackedWidget->setCurrentIndex(1);
	else
		ui.stackedWidget->setCurrentIndex(0);
}

void widget_count::slot_saveRecordOK_clicked()
{
	int iSaveRecordInterval = 30;
	bool isSaveRecord = ui.checkBox_saveRecord->isChecked();
	if (ui.radioButton_30minite->isChecked())
	{
		iSaveRecordInterval = 30;
	}
	else
	{
		iSaveRecordInterval = 60;
	}

	QSettings SystemConfigSet(pMainFrm->m_sConfigInfo.m_strConfigPath,QSettings::IniFormat);
	SystemConfigSet.setIniCodec(QTextCodec::codecForName("GBK"));
	SystemConfigSet.setValue("system/iSaveRecordInterval",iSaveRecordInterval);
	SystemConfigSet.setValue("system/isSaveRecord",isSaveRecord);

	emit updateRecordSet();
}

void widget_count::slot_saveRecordCancel_clicked()
{
	QSettings SystemConfigSet(pMainFrm->m_sConfigInfo.m_strConfigPath,QSettings::IniFormat);
	SystemConfigSet.setIniCodec(QTextCodec::codecForName("GBK"));
	int iSaveRecordInterval	=	SystemConfigSet.value("system/iSaveRecordInterval",30).toInt();
	bool isSaveRecord		=	SystemConfigSet.value("system/isSaveRecord",true).toBool();

	ui.checkBox_saveRecord->setChecked(isSaveRecord);
	if (iSaveRecordInterval == 30)
	{
		ui.radioButton_30minite->setChecked(true);
	}
	else
	{
		ui.radioButton_60minite->setChecked(true);
	}
}

void widget_count::slot_shiftOK_clicked()
{
	QTime shift1Time = ui.timeEdit_shift1->time();
	QTime shift2Time = ui.timeEdit_shift2->time();
	QTime shift3Time = ui.timeEdit_shift3->time();
	bool isAutoClear = ui.checkBox_AutoSetZero->isChecked();

	QVector<QTime> times;
	times<< initTime(shift1Time) << initTime(shift2Time) << initTime(shift3Time) ;
	qSort(times);

	QSettings SystemConfigSet(pMainFrm->m_sConfigInfo.m_strConfigPath,QSettings::IniFormat);
	SystemConfigSet.setIniCodec(QTextCodec::codecForName("GBK"));
	SystemConfigSet.setValue("system/shift1Time",times[0].toString("hhmmss"));
	SystemConfigSet.setValue("system/shift2Time",times[1].toString("hhmmss"));
	SystemConfigSet.setValue("system/shift3Time",times[2].toString("hhmmss"));
	SystemConfigSet.setValue("system/isAutoClear",isAutoClear);

	emit updateShiftSet();

	slot_shiftCancel_clicked();
}

void widget_count::slot_shiftCancel_clicked()
{
	QSettings SystemConfigSet(pMainFrm->m_sConfigInfo.m_strConfigPath,QSettings::IniFormat);
	SystemConfigSet.setIniCodec(QTextCodec::codecForName("GBK"));
	QTime shift1Time = QTime::fromString(SystemConfigSet.value("system/shift1Time","060000").toString(),"hhmmss");
	QTime shift2Time = QTime::fromString(SystemConfigSet.value("system/shift2Time","150000").toString(),"hhmmss");
	QTime shift3Time = QTime::fromString(SystemConfigSet.value("system/shift3Time","230000").toString(),"hhmmss");
	bool isAutoClear = SystemConfigSet.value("system/isAutoClear",true).toBool();

	ui.timeEdit_shift1->setTime(shift1Time);
	ui.timeEdit_shift2->setTime(shift2Time);
	ui.timeEdit_shift3->setTime(shift3Time);
	ui.checkBox_AutoSetZero->setChecked(isAutoClear);
}

void widget_count::slot_HistorySearch_clicked()
{
	QDate dateSelecte = ui.calendarWidget_1->selectedDate();
	QString temp = dateSelecte.toString(Qt::ISODate);
	temp.replace("-", "");

	if(ui.stackedWidget_2->currentIndex() == 0)
	{
		QList<long long> tmpTimes;
		QList<cErrorInfo> tmpInfos;
		pMainFrm->m_Datebase->queryByDay(temp ,tmpTimes,tmpInfos);
		slots_UpdateTable2(tmpTimes,tmpInfos);
	}
	else
	{
		QList<long long> tmpTimes;
		QList<cMoldNoErrorInfo> tmpInfos;
		pMainFrm->m_Datebase->queryMoldNoByDay(temp ,tmpTimes,tmpInfos);
		slots_updateMoldNoInfo(tmpTimes,tmpInfos);
	}
	

}

void widget_count::slot_OpenRecord_clicked()
{
	QDate dateSelecte = ui.calendarWidget->selectedDate();
	QString strFileName;
	if(0==ui.comboBox->currentIndex())
	{
		strFileName = pMainFrm->m_sConfigInfo.m_strAppPath + "CountInfo/timeCount/" + dateSelecte.toString(Qt::ISODate) + ".txt";
	}else{
		strFileName = pMainFrm->m_sConfigInfo.m_strAppPath + "CountInfo/shiftCount/" + dateSelecte.toString(Qt::ISODate) + ".txt";
	}
	if(!QFile::exists(strFileName))
	{
		QMessageBox::information(this,tr("Information"),tr("No record in select date!"));
		return;
	}
	openInNotePad(strFileName);
}

void widget_count::slot_DeleteRecord_clicked()
{
	QDate dateSelecte = ui.calendarWidget->selectedDate();
	QString strDirPath;
	QString strFileName;
	if(0==ui.comboBox->currentIndex())
	{
		strDirPath = pMainFrm->m_sConfigInfo.m_strAppPath+ "CountInfo/timeCount/";
		strFileName = pMainFrm->m_sConfigInfo.m_strAppPath+ "CountInfo/timeCount/" + dateSelecte.toString(Qt::ISODate) + ".txt";
	}else{
		strDirPath = pMainFrm->m_sConfigInfo.m_strAppPath+ "CountInfo/shiftCount/";
		strFileName = pMainFrm->m_sConfigInfo.m_strAppPath + "CountInfo/shiftCount/" + dateSelecte.toString(Qt::ISODate) + ".txt";
	}
	if(!QFile::exists(strFileName))
	{
		QMessageBox::information(this,tr("Information"),tr("No record in select date!"));
		return;
	}
	QDir dir(strDirPath);
	if (!dir.remove(strFileName))
	{
		QMessageBox::information(this,tr("Information"),tr("Deleting [%1] fail!").arg(strFileName));
		return;
	}

}

void widget_count::slot_OpenExcel_clicked()
{
	if (ExportThread->isRunning())
	{
		QMessageBox::information(NULL, tr("tips"), tr("The report is being generated. Please wait patiently!"), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}
	
	ExportThread->m_Date = ui.calendarWidget->selectedDate();
	//ExportThread->InitShiftTime(pMainFrm->m_sSystemInfo.shift1,pMainFrm->m_sSystemInfo.shift2,pMainFrm->m_sSystemInfo.shift3);
	ExportThread->start();
	QMessageBox::information(NULL, QString::fromLocal8Bit("注意"), QString::fromLocal8Bit("保存excel时间较长请耐心等待约10秒左右"), QMessageBox::Yes, QMessageBox::Yes);
}

void widget_count::slot_SearchShift_clicked()
{
	QString startTime,endTime;
	QDate dateSelecte = ui.calendarWidget_2->selectedDate();
	QString temp = dateSelecte.toString(Qt::ISODate);
	temp.replace("-", "");//20201028
	QString shiftStr;
	if (ui.radioButton_Shift1->isChecked())
	{		
		startTime = temp + pMainFrm->m_sSystemInfo.shift1Time.toString("hhmm");
		endTime = temp + pMainFrm->m_sSystemInfo.shift2Time.toString("hhmm");
		shiftStr=tr("shift1");
	}
	else if (ui.radioButton_Shift2->isChecked())
	{
		startTime = temp + pMainFrm->m_sSystemInfo.shift2Time.toString("hhmm");
		endTime = temp + pMainFrm->m_sSystemInfo.shift3Time.toString("hhmm");
		shiftStr=tr("shift2");
	}
	else if(ui.radioButton_Shift3->isChecked())
	{
		startTime = temp + pMainFrm->m_sSystemInfo.shift3Time.toString("hhmm");
		temp = dateSelecte.addDays(1).toString(Qt::ISODate).replace("-","");
		endTime = temp + pMainFrm->m_sSystemInfo.shift1Time.toString("hhmm");
		shiftStr=tr("shift3");
	}
	else
	{
		startTime = temp + pMainFrm->m_sSystemInfo.shift1Time.toString("hhmm");
		temp = dateSelecte.addDays(1).toString(Qt::ISODate).replace("-","");
		endTime = temp + pMainFrm->m_sSystemInfo.shift1Time.toString("hhmm");
		shiftStr=tr("AllShift");
	}
	QList<long long> tmpTimes;
	QList<cErrorInfo> tmpInfos;
	pMainFrm->m_Datebase->queryByShift(startTime,endTime,tmpTimes,tmpInfos);
	if (ui.radioButton_ShiftAll->isChecked())
	{
		slots_ShowShiftIamge(tmpTimes,tmpInfos);
	}
	else
	{
		slots_ShowShiftIamge(startTime,endTime,tmpTimes,tmpInfos);
	}
	
}

void widget_count::slot_MoldNoCheckBox_clicked(int id)
{
	Q_UNUSED(id);
	//MoldNoTimer->stop();
	CurSelectMoldNos.clear();
	for (int i=0;i<MOLDNO_MAX_COUNT;i++)
	{
		if(m_iMoldNoCheckBoxs[i]->isChecked())
		{
			CurSelectMoldNos<<i;
			m_iMoldNoCheckBoxs[i]->setStyleSheet(QString("color:red"));
		}
		else
			m_iMoldNoCheckBoxs[i]->setStyleSheet(QString("color:black"));
	}
	emit updateMoldNo(CurSelectMoldNos,false);
}

void widget_count::slot_ShowMoldNoCount(QList<int> nMoldNos,bool isOnlyUpdate)
{
	//pMainFrm->countLocker.lock();
	QList<int> nErrorTypes;
	for (int j=1;j<pMainFrm->m_sErrorInfo.m_iErrorTypeCount;j++)
	{
		for (int i=0;i<nMoldNos.count();i++)
		{
			int pTmpcount = pMainFrm->m_sMoldNoCount.m_iMoldNoTypeCount[j][nMoldNos[i]] ;
			if(pTmpcount != 0)
			{
				nErrorTypes<<j;
				break;
			}
		}
	}

	if(isOnlyUpdate && nErrorTypes.count() == nCurErrorTypes.count())
	{//只刷新数据，防止tableView下拉条不能拖动的问题。
		for(int i=0;i<table3Model->rowCount() && i<nMoldNos.count() ;i++)
		{	
			int k=0;
			table3Model->item(i,k++)->setText(QString::number(nMoldNos[i]));
			table3Model->item(i,k++)->setText(QString::number(pMainFrm->m_sMoldNoCount.m_iMoldNoCount[nMoldNos[i]]));
			for (int j=0;j<nCurErrorTypes.count();j++)
			{
				int pTmpcount = pMainFrm->m_sMoldNoCount.m_iMoldNoTypeCount[nCurErrorTypes[j]][nMoldNos[i]] ;
				table3Model->item(i,k++)->setText(QString::number(pTmpcount));
			}
		}
	}
	else
	{//有列宽数量的变化
		while(table3Model->rowCount()>0 )
		{
			QList<QStandardItem*> listItem = table3Model->takeRow(0);
			qDeleteAll(listItem);
			listItem.clear();
		}
		table3Model->clear();

		QStringList H_HeaderList;
		H_HeaderList<<tr("MoldNo")<<tr("Count");
		nCurErrorTypes.clear();
		for (int j=1;j<pMainFrm->m_sErrorInfo.m_iErrorTypeCount;j++)
		{
			for (int i=0;i<nMoldNos.count();i++)
			{
				int pTmpcount = pMainFrm->m_sMoldNoCount.m_iMoldNoTypeCount[j][nMoldNos[i]] ;
				if(pTmpcount != 0)
				{
					nCurErrorTypes<<j;
					H_HeaderList<<pMainFrm->m_sErrorInfo.m_vstrErrorType[j];
					break;
				}
			}
		}
		table3Model->setHorizontalHeaderLabels(H_HeaderList);

		for(int i=0;i<nMoldNos.count();i++)
		{
			QList<QStandardItem*> items;
			QStandardItem *item1 = new QStandardItem(QString::number(nMoldNos[i]));
			items<<item1;
			QStandardItem *item2 = new QStandardItem(QString::number(pMainFrm->m_sMoldNoCount.m_iMoldNoCount[nMoldNos[i]] ));
			items<<item2;
			for (int j=0;j<nCurErrorTypes.count();j++)
			{
				int pTmpcount = pMainFrm->m_sMoldNoCount.m_iMoldNoTypeCount[nCurErrorTypes[j]][nMoldNos[i]] ;
				QStandardItem *item3 = new QStandardItem(QString::number(pTmpcount ));
				items<<item3;
			}
			table3Model->insertRow(table3Model->rowCount(),items);
		}
		//MoldNoTimer->start();
	}
	//pMainFrm->countLocker.unlock();
}

void widget_count::slot_changeReport(int index)
{
	ui.stackedWidget_2->setCurrentIndex(index);
	if(index == 0)
	{

	}
	else
	{
		slots_updateMoldNoCount();
	}
}

void widget_count::slots_updateMoldNoCount()
{
	emit updateMoldNo(CurSelectMoldNos);
}

void widget_count::slots_updateMoldNoInfo(QList<long long> pTimes,QList<cMoldNoErrorInfo> pInfos)
{
	if (pInfos.isEmpty() || pTimes.count() != pInfos.count())
	{
		QMessageBox::information(this,tr("Report: query MoldNo Info"),tr("No data record of this time is queried!"));
		return;
	}
	while(table4Model->rowCount()>0 )
	{
		QList<QStandardItem*> listItem = table4Model->takeRow(0);
		qDeleteAll(listItem);
		listItem.clear();
	}
	table4Model->clear();

	QList<int> pMoldNos;
	QStringList H_HeaderList;
	H_HeaderList<<tr("time")<<tr("AllCount")<<tr("FailCount");
	for(int j=0;j<MOLDNO_MAX_COUNT;j++)
	{
		for(int i=0;i<pInfos.count();i++)
		{
			int tmpValue =pInfos[i].GetMoldNoCount(j);
			if(tmpValue != 0)
			{
				pMoldNos<<j;
				H_HeaderList<<tr("MoldNo:")+QString::number(j);
				break;
			}
		}
	}
	table4Model->setHorizontalHeaderLabels(H_HeaderList);
	ui.tableView_4->setColumnWidth(0,120);

	QList<QStandardItem*> items;
	for (int i=0;i<pTimes.count();i++)
	{
		QDateTime pTime = QDateTime::fromString(QString::number(pTimes[i]),"yyyyMMddhhmm");
		QString timeString= pTime.toString("yyyy-MM-dd hh:mm");

		cMoldNoErrorInfo pInfo = pInfos.at(i);
		items.clear();
		QStandardItem *item1 = new QStandardItem(timeString);
		items<<item1;
		QStandardItem *item2 = new QStandardItem(QString::number(pInfo.m_iMoldNoAllCount));
		items<<item2;
		QStandardItem *item3 = new QStandardItem(QString::number(pInfo.m_iMoldNoFailCount));
		items<<item3;
		for (int j=0;j<pMoldNos.count();j++)
		{
			QStandardItem *item4 = new QStandardItem(QString::number(pInfo.GetMoldNoCount(pMoldNos[j])));
			items<<item4;
		}
		table4Model->insertRow(table4Model->rowCount(),items);
	}
}

void widget_count::slots_updateMoldNoInfo2(long long time ,cMoldNoErrorInfo pInfo)
{
	while(table5Model->rowCount()>0 )
	{
		QList<QStandardItem*> listItem = table5Model->takeRow(0);
		qDeleteAll(listItem);
		listItem.clear();
	}
	table5Model->clear();
	QList<int> pMoldNos;
	for(int j=0;j<MOLDNO_MAX_COUNT;j++)
	{
		int tmpValue =pInfo.GetMoldNoCount(j);
		if(tmpValue != 0)
			pMoldNos<<j;
	}

	QDateTime pTime = QDateTime::fromString(QString::number(time),"yyyyMMddhhmm");
	QString timeString= pTime.toString("yyyy-MM-dd hh:mm");

	QStringList H_HeaderList;
	H_HeaderList<<tr("time")<<tr("MoldNo")<<tr("Count");
	QList<int> nErrorTypes;
	for (int j=1;j<pMainFrm->m_sErrorInfo.m_iErrorTypeCount;j++)
	{
		for (int i=0;i<pMoldNos.count();i++)
		{
			int pTmpcount = pInfo.m_iMoldNoTypeCount[j][pMoldNos[i]] ;
			if(pTmpcount != 0)
			{
				nErrorTypes<<j;
				H_HeaderList<<pMainFrm->m_sErrorInfo.m_vstrErrorType[j];
				break;
			}
		}
	}
	table5Model->setHorizontalHeaderLabels(H_HeaderList);
	ui.tableView_5->setColumnWidth(0,120);

	for(int i=0;i<pMoldNos.count();i++)
	{
		QList<QStandardItem*> items;
		QStandardItem *item0 = new QStandardItem(timeString);
		items<<item0;
		QStandardItem *item1 = new QStandardItem(QString::number(pMoldNos[i]));
		items<<item1;
		QStandardItem *item2 = new QStandardItem(QString::number(pInfo.m_iMoldNoCount[pMoldNos[i]] ));
		items<<item2;
		for (int j=0;j<nErrorTypes.count();j++)
		{
			int pTmpcount = pInfo.m_iMoldNoTypeCount[nErrorTypes[j]][pMoldNos[i]] ;
			QStandardItem *item3 = new QStandardItem(QString::number(pTmpcount ));
			items<<item3;
		}
		table5Model->insertRow(table5Model->rowCount(),items);
	}
}

void widget_count::slot_updateMoldNoTable2(QModelIndex modelIndex)
{
	int iListNo = modelIndex.row();
	QModelIndex index = table4Model->index(iListNo,0);
	QString name = table4Model->data(index).toString();
	QDateTime pDateTime=QDateTime::fromString(name,"yyyy-MM-dd hh:mm");
	QString timeStr = pDateTime.toString("yyyyMMddhhmm");
	long long tmpTime=0;
	cMoldNoErrorInfo tmpInfo;
	pMainFrm->m_Datebase->queryMoldNoByOnce(timeStr,tmpTime,tmpInfo);
	slots_updateMoldNoInfo2(tmpTime,tmpInfo);
}

void widget_count::slots_ShowPie(QModelIndex modelIndex)
{
	int iListNo = modelIndex.row();
	QModelIndex index = table2Model->index(iListNo,0);
	QString name = table2Model->data(index).toString();
	QDateTime pDateTime=QDateTime::fromString(name,"yyyy-MM-dd hh:mm");
	QString timeStr = pDateTime.toString("yyyyMMddhhmm");
	long long tmpTime=0;
	cErrorInfo tmpInfo;
	pMainFrm->m_Datebase->queryByOnce(timeStr,tmpTime,tmpInfo);

	ui.widget_TimePie->setWidgetName(tr("Defect Count - %1").arg(name));
	slots_ShowPieImage1(tmpInfo);
}

void widget_count::slots_updateCountInfo(int total,int failNum,float modelRate)
{
	if (total >= 0 && failNum >= 0)
	{
		ui.label_total->setText(tr("Product Number")+"\n"+QString::number(total));
		ui.label_failur->setText(tr("Reject Number")+"\n"+QString::number(failNum));
		double failRate=0.0;
		if (0 != total)
		{
			failRate = (double)failNum *100 / total;
		}
		ui.label_failurRate->setText(tr("Reject Rate")+"\n"+QString::number(failRate,'f',2)+"%");
	}
	if (modelRate >= 0)
	{
		ui.label_modelRate->setText(tr("Model Rate") + "\n" + QString::number(modelRate *100 ,'f',2)+"%");
	}	
}

void widget_count::slots_UpdateTable1(cErrorInfo pCountdates)
{
	while(table1Model->rowCount()>0 )
	{
		QList<QStandardItem*> listItem = table1Model->takeRow(0);
		qDeleteAll(listItem);
		listItem.clear();
	}
	QList<QStandardItem*> items;
	for (int i=1;i<pMainFrm->m_sErrorInfo.m_iErrorTypeCount;i++)
	{
		items.clear();
		int ErrorByType=pCountdates.GetErrorByTypeCount(i);
		if (ErrorByType != 0)
		{
			QStandardItem *item1 = new QStandardItem(pMainFrm->m_sErrorInfo.m_vstrErrorType[i]);
			items<<item1;
			QStandardItem *item2 = new QStandardItem(QString::number(ErrorByType));
			items<<item2;
			QStandardItem *item3 = new QStandardItem(QString::number(pCountdates.iFrontErrorByType[i]));
			items<<item3;
			QStandardItem *item4 = new QStandardItem(QString::number(pCountdates.iClampErrorByType[i]));
			items<<item4;
			QStandardItem *item5 = new QStandardItem(QString::number(pCountdates.iRearErrorByType[i]));
			items<<item5;

			table1Model->insertRow(table1Model->rowCount(),items);
		}
	}
}

void widget_count::slots_UpdateTable2(QList<long long> pTimes,QList<cErrorInfo> pInfos)
{
	if (pInfos.isEmpty() || pTimes.count() != pInfos.count())
	{
		QMessageBox::information(this,tr("Report: query Detect Info"),tr("No data record of this time is queried!"));
		return;
	}
	while(table2Model->rowCount()>0 )
	{
		QList<QStandardItem*> listItem = table2Model->takeRow(0);
		qDeleteAll(listItem);
		listItem.clear();
	}

	QList<QStandardItem*> items;
	for (int i=0;i<pTimes.count();i++)
	{
		QDateTime pTime = QDateTime::fromString(QString::number(pTimes[i]),"yyyyMMddhhmm");
		QString timeString= pTime.toString("yyyy-MM-dd hh:mm");

		cErrorInfo pInfo = pInfos.at(i);
		items.clear();
		QStandardItem *item1 = new QStandardItem(timeString);
		items<<item1;
		QStandardItem *item2 = new QStandardItem(QString::number(pInfo.iAllCount));
		items<<item2;
		QStandardItem *item3 = new QStandardItem(QString::number(pInfo.GetFailCount()));
		items<<item3;
		QStandardItem *item4 = new QStandardItem(QString("%1%").arg(pInfo.GetFailRate()*100,2,'f',2));
		items<<item4;
		QStandardItem *item5 = new QStandardItem(QString::number(pInfo.GetFrontCount()));
		items<<item5;
		QStandardItem *item6 = new QStandardItem(QString::number(pInfo.GetClampCount()));
		items<<item6;
		QStandardItem *item7 = new QStandardItem(QString::number(pInfo.GetRearCount()));
		items<<item7;
		table2Model->insertRow(table2Model->rowCount(),items);
	}
}

void widget_count::slots_UpdateTable2(QString ptime ,cErrorInfo pCountdate)
{
	//cErrorCountbyTime pInfo = pCountdate;
	QList<QStandardItem*> items;
	QStandardItem *item1 = new QStandardItem(ptime);
	items<<item1;
	QStandardItem *item2 = new QStandardItem(QString::number(pCountdate.iAllCount));
	items<<item2;
	QStandardItem *item3 = new QStandardItem(QString::number(pCountdate.GetFailCount()));
	items<<item3;
	QStandardItem *item4 = new QStandardItem(QString("%1%").arg(pCountdate.GetFailRate()*100,2,'f',2));
	items<<item4;
	QStandardItem *item5 = new QStandardItem(QString::number(pCountdate.GetFrontCount()));
	items<<item5;
	QStandardItem *item6 = new QStandardItem(QString::number(pCountdate.GetClampCount()));
	items<<item6;
	QStandardItem *item7 = new QStandardItem(QString::number(pCountdate.GetRearCount()));
	items<<item7;
	table2Model->insertRow(table2Model->rowCount(),items);
}

void widget_count::slots_ShowPieImage1(cErrorInfo pCountdates)
{
	PieModel_1->removeRows(0,PieModel_1->rowCount(QModelIndex()),QModelIndex());
	if (pCountdates.iAllCount == 0)
		return;
	QFont itemFont("微软雅黑",10);
	itemFont.setBold(true);
	int tempCount=0;
	QList<int> Types;
	QList<int> Datas;
	for (int i=0;i<50;i++)
	{
		bool havedata = false;
		if (pCountdates.iFrontErrorByType[i] != 0)
			havedata = true;
		if (pCountdates.iClampErrorByType[i] != 0)
			havedata = true;
		if (pCountdates.iRearErrorByType[i] != 0)
			havedata = true;
		if(havedata)
		{
			Types<<i;
			Datas<<pCountdates.GetErrorByTypeCount(i);
		}
	}

	for (int i=0;i<Types.count()-1;i++)
	{
		for(int j=0;j<Types.count()-1-i;j++)
		{
			if(Datas[j] < Datas[j+1])
			{
				int tmp=0;
				tmp = Datas[j+1];
				Datas[j+1] = Datas[j];
				Datas[j] = tmp;

				tmp = Types[j+1];
				Types[j+1] = Types[j];
				Types[j] = tmp;

			}
		}	
	}

	if (Types.count() <=10)
	{
		for(int i = 0;i < Types.count();i++)
		{
			PieModel_1->insertRows(i, 1, QModelIndex());
			PieModel_1->setData(PieModel_1->index(i, 0, QModelIndex()), pMainFrm->m_sErrorInfo.m_vstrErrorType[Types[i]]+":" + QString::number(Datas[i])+"("+QString::number((double)Datas[i] / pCountdates.iAllCount *100,'f',2)+"%)");
			PieModel_1->setData(PieModel_1->index(i, 1, QModelIndex()), QString::number(Datas[i]));
			PieModel_1->setData(PieModel_1->index(i, 0, QModelIndex()), PieItemcolors[i], Qt::DecorationRole);
			PieModel_1->item(i,0)->setFont(itemFont);
		}
	}
	else
	{
		int t_fail = 0;
		for(int i = 0;i < 9;i++)
		{
			t_fail += Datas[i];

			PieModel_1->insertRows(i, 1, QModelIndex());
			PieModel_1->setData(PieModel_1->index(i, 0, QModelIndex()), pMainFrm->m_sErrorInfo.m_vstrErrorType[Types[i]]+":" + QString::number(Datas[i])+"("+QString::number((double)Datas[i] / pCountdates.iAllCount *100,'f',2)+"%)");
			PieModel_1->setData(PieModel_1->index(i, 1, QModelIndex()), QString::number(Datas[i]));
			PieModel_1->setData(PieModel_1->index(i, 0, QModelIndex()), PieItemcolors[i], Qt::DecorationRole);
			PieModel_1->item(i,0)->setFont(itemFont);
		}
		t_fail = pCountdates.GetFailCount() - t_fail ;
		PieModel_1->insertRows(9, 1, QModelIndex());
		PieModel_1->setData(PieModel_1->index(9, 0, QModelIndex()), tr("Other:") + QString::number(t_fail)+"("+QString::number((double)t_fail/pCountdates.iAllCount*100,'f',2)+"%)");
		PieModel_1->setData(PieModel_1->index(9, 1, QModelIndex()), QString::number(t_fail));
		PieModel_1->setData(PieModel_1->index(9, 0, QModelIndex()), PieItemcolors[9], Qt::DecorationRole);
		PieModel_1->item(9,0)->setFont(itemFont);
	}

}

void widget_count::slots_ShowPieImage2(QList<cErrorTypeCountInfo> pCountdates)
{
	if (pCountdates.isEmpty())
	{
		return;
	}
	QFont itemFont("微软雅黑",10);
	itemFont.setBold(true);

	PieModel_2->removeRows(0,PieModel_2->rowCount(QModelIndex()),QModelIndex());
	int tempCount=pCountdates.count();
	int ErrorAllfail=0;
	for(int i = 0;i < tempCount;i++)
	{
		ErrorAllfail += pCountdates.at(i).iErrorFailCount;
	}
	int m_failCount = pCountdates.at(0).iFailCount;
	m_failCount = ErrorAllfail < m_failCount ? ErrorAllfail : m_failCount;
	int m_AllCount = pCountdates.at(0).iCheckCount;
	if (tempCount <=10)
	{
		int t_fail = 0;
		for(int i = 0;i < tempCount;i++)
		{
			int t_data = 0 ;
			if (i != (tempCount-1))
			{
				t_data = pCountdates.at(i).iErrorFailCount;
				t_fail += t_data;
			}
			else
			{
				t_data = m_failCount - t_fail ;
			}
			PieModel_2->insertRows(i, 1, QModelIndex());
			PieModel_2->setData(PieModel_2->index(i, 0, QModelIndex()), pCountdates[i].iErrorTxt+":" + QString::number(t_data)+"("+QString::number((double)t_data/m_AllCount*100,'f',2)+"%)");
			PieModel_2->setData(PieModel_2->index(i, 1, QModelIndex()), QString::number(t_data));
			PieModel_2->setData(PieModel_2->index(i, 0, QModelIndex()), PieItemcolors[i], Qt::DecorationRole);
			PieModel_2->item(i,0)->setFont(itemFont);
		}
	}
	else
	{
		int t_fail = 0;
		for(int i = 0;i < 9;i++)
		{
			int t_data = pCountdates.at(i).iErrorFailCount;
			if (ErrorAllfail != 0)
			{
				t_data = m_failCount * t_data  / ErrorAllfail ;
			}
			t_fail += t_data;

			PieModel_2->insertRows(i, 1, QModelIndex());
			PieModel_2->setData(PieModel_2->index(i, 0, QModelIndex()), pCountdates[i].iErrorTxt+":" + QString::number(t_data)+"("+QString::number((double)t_data/m_AllCount*100,'f',2)+"%)");
			PieModel_2->setData(PieModel_2->index(i, 1, QModelIndex()), QString::number(t_data));
			PieModel_2->setData(PieModel_2->index(i, 0, QModelIndex()), PieItemcolors[i], Qt::DecorationRole);
			PieModel_2->item(i,0)->setFont(itemFont);
		}
		t_fail = m_failCount - t_fail ;
		PieModel_2->insertRows(9, 1, QModelIndex());
		PieModel_2->setData(PieModel_2->index(9, 0, QModelIndex()), tr("Other:") + QString::number(t_fail)+"("+QString::number((double)t_fail/m_AllCount*100,'f',2)+"%)");
		PieModel_2->setData(PieModel_2->index(9, 1, QModelIndex()), QString::number(t_fail));
		PieModel_2->setData(PieModel_2->index(9, 0, QModelIndex()), PieItemcolors[9], Qt::DecorationRole);
		PieModel_2->item(9,0)->setFont(itemFont);
	}

}

void widget_count::slots_ShowShiftIamge(QString startTime,QString endTime,QList<long long> pTimes,QList<cErrorInfo> pInfos)
{
	if (pInfos.isEmpty() || pTimes.isEmpty())
	{
		QMessageBox::information(this,tr("Report: query histogram"),tr("%1 - %2 \nNo data record of this period is queried!").arg(startTime).arg(endTime));
		return;
	}
	//QMessageBox::information(this,"info",QString("Start:%1,End:%2,TimeCount:%3,InfoCount:%4").arg(startTime).arg(endTime).arg(pTimes.count()).arg(pInfos.count()));
	//CLogFile::write(QString("Start:%1,End:%2,TimeCount:%3,InfoCount:%4").arg(startTime).arg(endTime).arg(pTimes.count()).arg(pInfos.count()),DebugLog);
	QDateTime startDateT = QDateTime::fromString(startTime,"yyyyMMddhhmm");
	QDateTime endDateT = QDateTime::fromString(endTime,"yyyyMMddhhmm");
	int plottableCount = ui.CustomPlot->plottableCount();
	if (plottableCount != 0)
		ui.CustomPlot->clearPlottables();

	// create empty bar chart objects:
	CustomBars *fossil = new CustomBars(ui.CustomPlot->xAxis, ui.CustomPlot->yAxis);
	CustomBars *nuclear = new CustomBars(ui.CustomPlot->xAxis, ui.CustomPlot->yAxis);
	nuclear->setAntialiased(false);
	fossil->setAntialiased(false);
	// set names and colors:
	QList<QColor> colors;
	colors<<QColor(0, 0, 255) << QColor(250, 0, 0);

	fossil->setName(tr("All Detect Number"));
	fossil->setPen(QPen(colors[0]));
	colors[0].setAlpha(180);
	fossil->setBrush(colors[0]);
	nuclear->setName(tr("Failure Number"));
	nuclear->setPen(QPen(colors[1]));
	colors[1].setAlpha(180);
	nuclear->setBrush(colors[1]);

	//设置并排显示
	QCPBarsGroup *group = new QCPBarsGroup(ui.CustomPlot);  
	QList<QCPBars*> bars;
	bars <<fossil <<nuclear ;// << regen;
	foreach (QCPBars *bar, bars) {
		// 设置柱状图的宽度类型为以key坐标轴计算宽度的大小，其实默认就是这种方式
		bar->setWidthType(QCPBars::wtPlotCoords);
		bar->setWidth(bar->width() / bars.size()); // 设置柱状图的宽度大小
		group->append(bar);  // 将柱状图加入柱状图分组中
	}
	group->setSpacingType(QCPBarsGroup::stAbsolute);  // 设置组内柱状图的间距，按像素
	group->setSpacing(2);     // 设置较小的间距值，这样看起来更紧凑

	int startH = startDateT.time().hour();
	int endH = endDateT.time().hour();
	int startM = startDateT.time().minute();
	int endM = endDateT.time().minute();
	// prepare x axis with country labels:
	QVector<double> ticks;
	QVector<QString> labels;
	// Add data:
	QVector<double> fossilData, nuclearData;
	int m_range = 1;
	if (pMainFrm->m_sSystemInfo.iSaveRecordInterval == 60)
	{
		if (startH < endH )
		{
			if (startM > 0 )
				startH += 1;
			if (endM > 0 )
				endH += 1;
		}
		else
		{
			if (startM > 0 )
				startH += 1;
			if (endM > 0 )
				endH += 1;
			endH += 24;
		}
		int tmp = endH - startH;
		if ( tmp > 0 )
		{
			int index=0;
			for (int i=1;i<=tmp;i++)
			{
				ticks<<i;
				QString str = startDateT.addSecs(i*60*60).toString("hh:mm\nyyyy-MM-dd");
				labels<<str;
				QDateTime tmpT= QDateTime::fromString(QString::number(pTimes[index]),"yyyyMMddhhmm");
				if (str == tmpT.toString("hh:mm\nyyyy-MM-dd") )
				{
					fossilData << pInfos[index].iAllCount;
					nuclearData << pInfos[index].GetFailCount();
					if(index < pTimes.count()-1 )
						index++;
				}
				else
				{
					fossilData << 0;
					nuclearData << 0;
				}
			}
		}
		else
			return;
	}
	else
	{
		if (startH > endH )
		{
			endH += 24;
		}
		int tmp = (endH - startH)*2;
		if (startM>0 && startM<=30)
			tmp -= 1;
		if (endM>0 && endM<=30)
			tmp += 1;
		if ( tmp > 1 )
		{
			int index=0;
			for (int i=1;i<=tmp;i++)
			{
				ticks<<i;
				QString str = startDateT.addSecs(i*30*60).toString("hh:mm\nyyyy-MM-dd");
				labels<<str;
				QDateTime tmpT= QDateTime::fromString(QString::number(pTimes[index]),"yyyyMMddhhmm");
				if (str == tmpT.toString("hh:mm\nyyyy-MM-dd") )
				{
					fossilData << pInfos[index].iAllCount;
					nuclearData << pInfos[index].GetFailCount();
					if(index < pTimes.count()-1 )
						index++;
				}
				else
				{
					fossilData << 0;
					nuclearData << 0;
				}
			}			
		}
		else
			return;
	}

	QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
	textTicker->addTicks(ticks, labels);
	ui.CustomPlot->xAxis->setTicker(textTicker);
	//ui.CustomPlot->xAxis->setTickLabelRotation(30);
	ui.CustomPlot->xAxis->setSubTicks(false);
	ui.CustomPlot->xAxis->setTickLength(0, labels.count()+1);
	ui.CustomPlot->xAxis->setRange(0, labels.count()+1);
	ui.CustomPlot->xAxis->setBasePen(QPen(Qt::white));
	ui.CustomPlot->xAxis->setTickPen(QPen(Qt::white));
	ui.CustomPlot->xAxis->grid()->setVisible(true);
	ui.CustomPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
	ui.CustomPlot->xAxis->setTickLabelColor(Qt::white);
	ui.CustomPlot->xAxis->setLabelColor(Qt::white);
	ui.CustomPlot->xAxis->setLabel(tr("Time(Hour)"));

	int MaxNumber=0;
	for(int i=0;i<fossilData.count();i++)
	{
		MaxNumber = MaxNumber > fossilData[i] ? MaxNumber : fossilData[i];
	}
	if (MaxNumber >= 10000)
	{
		MaxNumber +=10000;
	}
	else if (MaxNumber <10000 && MaxNumber >=1000)
	{
		MaxNumber += 1000;
	}
	else if (MaxNumber <1000 && MaxNumber >= 100)
	{
		MaxNumber+=100;
	}
	else if (MaxNumber <100 && MaxNumber >=10)
	{
		MaxNumber += 10 ;
	}
	else
	{
		MaxNumber += 2;
	}

	// prepare y axis:
	ui.CustomPlot->yAxis->setRange(0, MaxNumber);
	ui.CustomPlot->yAxis->setPadding(5); // a bit more space to the left border
	ui.CustomPlot->yAxis->setLabel(tr("Failure Number Table(Ps)"));
	ui.CustomPlot->yAxis->setBasePen(QPen(Qt::white));
	ui.CustomPlot->yAxis->setTickPen(QPen(Qt::white));
	ui.CustomPlot->yAxis->setSubTickPen(QPen(Qt::white));
	ui.CustomPlot->yAxis->grid()->setSubGridVisible(true);
	ui.CustomPlot->yAxis->setTickLabelColor(Qt::white);
	ui.CustomPlot->yAxis->setLabelColor(Qt::white);
	ui.CustomPlot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
	ui.CustomPlot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

	//set data
	fossil->setData(ticks, fossilData);
	nuclear->setData(ticks, nuclearData);

	// setup legend:
	ui.CustomPlot->legend->setVisible(true);
	ui.CustomPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
	ui.CustomPlot->legend->setBrush(QColor(255, 255, 255, 100));
	ui.CustomPlot->legend->setBorderPen(Qt::NoPen);
	QFont legendFont = font();
	legendFont.setPointSize(10);
	ui.CustomPlot->legend->setFont(legendFont);
	ui.CustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.CustomPlot->replot();
}


void widget_count::slots_ShowShiftIamge(QList<long long> pTimes,QList<cErrorInfo> pInfos)
{
	if (pInfos.isEmpty() || pTimes.isEmpty())
	{
		QMessageBox::information(this,tr("Report: query shift histogram"),tr("No data record of this time is queried!"));
		return;
	}
	int plottableCount = ui.CustomPlot->plottableCount();
	if (plottableCount != 0)
		ui.CustomPlot->clearPlottables();
	// create empty bar chart objects:
	CustomBars *fossil = new CustomBars(ui.CustomPlot->xAxis, ui.CustomPlot->yAxis);
	CustomBars *nuclear = new CustomBars(ui.CustomPlot->xAxis, ui.CustomPlot->yAxis);
	nuclear->setAntialiased(false);
	fossil->setAntialiased(false);
	// set names and colors:
	QList<QColor> colors;
	colors<<QColor(0, 0, 255) << QColor(250, 0, 0);

	fossil->setName(tr("Single Shift All Detect Number"));
	fossil->setPen(QPen(colors[0]));
	colors[0].setAlpha(180);
	fossil->setBrush(colors[0]);
	nuclear->setName(tr("Single Shift Failure Number"));
	nuclear->setPen(QPen(colors[1]));
	colors[1].setAlpha(180);
	nuclear->setBrush(colors[1]);

	//设置并排显示
	QCPBarsGroup *group = new QCPBarsGroup(ui.CustomPlot);  
	QList<QCPBars*> bars;
	bars <<fossil <<nuclear ;// << regen;
	foreach (QCPBars *bar, bars) {
		// 设置柱状图的宽度类型为以key坐标轴计算宽度的大小，其实默认就是这种方式
		bar->setWidthType(QCPBars::wtPlotCoords);
		bar->setWidth(bar->width() / bars.size()); // 设置柱状图的宽度大小
		group->append(bar);  // 将柱状图加入柱状图分组中
	}
	group->setSpacingType(QCPBarsGroup::stAbsolute);  // 设置组内柱状图的间距，按像素
	group->setSpacing(2);     // 设置较小的间距值，这样看起来更紧凑

	// prepare x axis with country labels:
	QVector<double> ticks;
	QVector<QString> labels;
	ticks<<1<<2<<3;
	labels<<tr("Shift1")<<tr("Shift2")<<tr("Shift3");

	QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
	textTicker->addTicks(ticks, labels);
	ui.CustomPlot->xAxis->setTicker(textTicker);
	ui.CustomPlot->xAxis->setTickLabelRotation(0);
	ui.CustomPlot->xAxis->setSubTicks(false);
	ui.CustomPlot->xAxis->setTickLength(0, 4);
	ui.CustomPlot->xAxis->setRange(0, 4);
	ui.CustomPlot->xAxis->setBasePen(QPen(Qt::white));
	ui.CustomPlot->xAxis->setTickPen(QPen(Qt::white));
	ui.CustomPlot->xAxis->grid()->setVisible(true);
	ui.CustomPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
	ui.CustomPlot->xAxis->setTickLabelColor(Qt::white);
	ui.CustomPlot->xAxis->setLabelColor(Qt::white);
	ui.CustomPlot->xAxis->setLabel(tr("Shift(1,2,3)"));

	QVector<double> fossilData, nuclearData;
	int shiftCount[3]={0};
	int shiftFailCount[3]={0};
	for (int i=0;i<pTimes.count();i++)
	{
		QDateTime tmpTime=QDateTime::fromString(QString::number(pTimes[i]),"yyyyMMddhhmm");
		if (tmpTime.time() > pMainFrm->m_sSystemInfo.shift1Time  && tmpTime.time() <= pMainFrm->m_sSystemInfo.shift2Time)
		{
			shiftCount[0] += pInfos[i].iAllCount;
			shiftFailCount[0] += pInfos[i].GetFailCount();
		}
		else if (tmpTime.time() > pMainFrm->m_sSystemInfo.shift2Time  && tmpTime.time() <= pMainFrm->m_sSystemInfo.shift3Time)
		{
			shiftCount[1] += pInfos[i].iAllCount;
			shiftFailCount[1] += pInfos[i].GetFailCount();
		}
		else
		{
			shiftCount[2] += pInfos[i].iAllCount;
			shiftFailCount[2] += pInfos[i].GetFailCount();
		}
	}
	fossilData<<shiftCount[0]<<shiftCount[1]<<shiftCount[2];
	nuclearData<<shiftFailCount[0]<<shiftFailCount[1]<<shiftFailCount[2];

	int MaxNumber=0;
	for(int i=0;i<fossilData.count();i++)
	{
		MaxNumber = MaxNumber > fossilData[i] ? MaxNumber : fossilData[i];
	}
	if (MaxNumber >= 10000)
	{
		MaxNumber +=10000;
	}
	else if (MaxNumber <10000 && MaxNumber >=1000)
	{
		MaxNumber += 1000;
	}
	else if (MaxNumber <1000 && MaxNumber >= 100)
	{
		MaxNumber+=100;
	}
	else if (MaxNumber <100 && MaxNumber >=10)
	{
		MaxNumber += 10 ;
	}
	else
	{
		MaxNumber += 2;
	}

	// prepare y axis:
	ui.CustomPlot->yAxis->setRange(0, MaxNumber);
	ui.CustomPlot->yAxis->setPadding(5); // a bit more space to the left border
	ui.CustomPlot->yAxis->setLabel(tr("Shift Failure Number Table(Ps)"));
	ui.CustomPlot->yAxis->setBasePen(QPen(Qt::white));
	ui.CustomPlot->yAxis->setTickPen(QPen(Qt::white));
	ui.CustomPlot->yAxis->setSubTickPen(QPen(Qt::white));
	ui.CustomPlot->yAxis->grid()->setSubGridVisible(true);
	ui.CustomPlot->yAxis->setTickLabelColor(Qt::white);
	ui.CustomPlot->yAxis->setLabelColor(Qt::white);
	ui.CustomPlot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
	ui.CustomPlot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

	//setdata
	fossil->setData(ticks, fossilData);
	nuclear->setData(ticks, nuclearData);

	// setup legend:
	ui.CustomPlot->legend->setVisible(true);
	ui.CustomPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
	ui.CustomPlot->legend->setBrush(QColor(255, 255, 255, 100));
	ui.CustomPlot->legend->setBorderPen(Qt::NoPen);
	QFont legendFont = font();
	legendFont.setPointSize(10);
	ui.CustomPlot->legend->setFont(legendFont);
	ui.CustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.CustomPlot->replot();

}

//自定义QCustomPlot直方图bar
CustomBars::CustomBars(QCPAxis *keyAxis , QCPAxis *valueAxis)
	:QCPBars(keyAxis,valueAxis),
	mTextAlignment(Qt::AlignCenter),
	mSpacing(5),
	mFont(QFont(QLatin1String("san serif"),9)),
	m_TextSuffix("")
{

}

void CustomBars::setTextAlignment(Qt::Alignment alignment)
{
	mTextAlignment = alignment;
}

void CustomBars::setSpacing(double spacing)
{
	mSpacing = spacing;
}

void CustomBars::setFont(const QFont &font)
{
	mFont = font;
}

void CustomBars::setTextSuffix(QString Suffix)
{
	m_TextSuffix = Suffix;
}

void CustomBars::draw(QCPPainter *painter) Q_DECL_OVERRIDE
{
	if (!mKeyAxis || !mValueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return; }
	if (mDataContainer->isEmpty()) return;

	QCPBarsDataContainer::const_iterator visibleBegin, visibleEnd;
	getVisibleDataBounds(visibleBegin, visibleEnd);

	// loop over and draw segments of unselected/selected data:
	QList<QCPDataRange> selectedSegments, unselectedSegments, allSegments;
	getDataSegments(selectedSegments, unselectedSegments);
	allSegments << unselectedSegments << selectedSegments;
	for (int i=0; i<allSegments.size(); ++i)
	{
		bool isSelectedSegment = i >= unselectedSegments.size();
		QCPBarsDataContainer::const_iterator begin = visibleBegin;
		QCPBarsDataContainer::const_iterator end = visibleEnd;
		mDataContainer->limitIteratorsToDataRange(begin, end, allSegments.at(i));
		if (begin == end)
			continue;

		for (QCPBarsDataContainer::const_iterator it=begin; it!=end; ++it)
		{
			// check data validity if flag set:
#ifdef QCUSTOMPLOT_CHECK_DATA
			if (QCP::isInvalidData(it->key, it->value))
				qDebug() << Q_FUNC_INFO << "Data point at" << it->key << "of drawn range invalid." << "Plottable name:" << name();
#endif
			// draw bar:
			if (isSelectedSegment && mSelectionDecorator)
			{
				mSelectionDecorator->applyBrush(painter);
				mSelectionDecorator->applyPen(painter);
			} else
			{
				painter->setBrush(mBrush);
				painter->setPen(mPen);
			}
			applyDefaultAntialiasingHint(painter);
			QRectF barRect=getBarRect(it->key, it->value);
			painter->drawPolygon(getBarRect(it->key, it->value));

			//add text
			// 计算文字的位置
			painter->setFont(mFont);                     // 设置字体
			QString text = QString::number(it->value);   // 取得当前value轴的值，保留两位精度
			text += m_TextSuffix;

			QRectF textRect = painter->fontMetrics().boundingRect(0, 0, 0, 0, Qt::TextDontClip | mTextAlignment, text);  // 计算文字所占用的大小

			if (mKeyAxis.data()->orientation() == Qt::Horizontal) 
			{    // 当key轴为水平轴的时候
				if (mKeyAxis.data()->axisType() == QCPAxis::atTop)     // 上轴，移动文字到柱状图下面
					textRect.moveTopLeft(barRect.bottomLeft() + QPointF(0, mSpacing));
				else                                                   // 下轴，移动文字到柱状图上面
					textRect.moveBottomLeft(barRect.topLeft() - QPointF(0, mSpacing));
				textRect.setWidth(barRect.width());
				painter->drawText(textRect, Qt::TextDontClip | mTextAlignment, text);
			}
			else 
			{                                                  // 当key轴为竖直轴的时候
				if (mKeyAxis.data()->axisType() == QCPAxis::atLeft)   // 左轴，移动文字到柱状图右边
					textRect.moveTopLeft(barRect.topRight() + QPointF(mSpacing, 0));
				else                                                  // 右轴，移动文字到柱状图左边
					textRect.moveTopRight(barRect.topLeft() - QPointF(mSpacing, 0));
				textRect.setHeight(barRect.height());
				painter->drawText(textRect, Qt::TextDontClip | mTextAlignment, text);
			}
		}
	}

	// draw other selection decoration that isn't just line/scatter pens and brushes:
	if (mSelectionDecorator)
		mSelectionDecorator->drawDecoration(painter, selection());
}

ExportExcelThread::ExportExcelThread(QObject *parent /*= 0*/)
{

}

ExportExcelThread::~ExportExcelThread()
{

}

void ExportExcelThread::run()
{
	ExcelObject *m_ExcelObj=new ExcelObject();
	ExcelObject::ERRORTYPE errCode=m_ExcelObj->init();
	switch(errCode)
	{
	case ExcelObject::INIT_ERROR1:
		{
			QMessageBox::warning(NULL,tr("Error"),tr("Failed to create Excel object, please install Microsoft Excel."),QMessageBox::Apply);
			return;
		}
		break;
	case ExcelObject::INIT_ERROR2:
		{
			QMessageBox::warning(NULL,tr("Error"),tr("Get Microsoft Excel workbook error,Please check that Microsoft Excel is installed correctly."),QMessageBox::Apply);
			return;
		}
		break;
	default:
		break;
	}

	QDate pDate = m_Date;
	QString temp = pDate.toString(Qt::ISODate);
	temp += " Report.xls";
	QString fileName = pMainFrm->m_sConfigInfo.m_strAppPath +"CountInfo/Report/"+temp ;

	if (m_ExcelObj->m_Version.toFloat() > 11)
	{ //xls是excel2003及以前版本生成的文件格式，而xlsx是excel2007及以后版本生成的文件格式。
		fileName = fileName + "x";
	}

	QFileInfo fileInfo(fileName);
	QDir fileDir(fileInfo.absolutePath());
	if (!fileDir.exists())
	{
		fileDir.mkpath(fileInfo.absolutePath());
	}
	QFile m_file(fileName);
	if (m_file.exists())
	{
		m_file.remove();
	}

	QString DateStr = pDate.toString(Qt::ISODate).replace("-","");
	int row =1;
	QList<long long> tmpTimes;
	QList<cErrorInfo> tmpInfos;
	pMainFrm->m_Datebase->queryByDay(DateStr ,tmpTimes,tmpInfos);
	for (int i=0;i<tmpTimes.count();i++)
	{
		cErrorInfo info = tmpInfos[i];
		int fristRow=row;
		QDateTime time = QDateTime::fromString(QString::number(tmpTimes[i]), "yyyyMMddhhmm");  
		m_ExcelObj->SetGeneralInfo(row++,tr("time: %1\nAll Count: %2   Fail Count: %3   Fail Rate: %4\nFront: %5   Clamp: %6   Rear: %7")
									.arg(time.toString("yyyy-MM-dd hh:mm:ss"))
									.arg(info.iAllCount)
									.arg(info.GetFailCount())
									.arg(QString::number(tmpInfos[i].GetFailRate() * 100 ,'f',2) + "%")
									.arg(info.GetFrontCount())
									.arg(info.GetClampCount())
									.arg(info.GetRearCount()));
		//标题
		m_ExcelObj->SetTatilRow(row++);
		QStringList dataList;
		for (int k=1;k<pMainFrm->m_sErrorInfo.m_iErrorTypeCount;k++)
		{
			int ErrorBytype=info.GetErrorByTypeCount(k);
			if(ErrorBytype != 0)
			{
				dataList.clear();
				dataList.append(pMainFrm->m_sErrorInfo.m_vstrErrorType[k]);
				dataList.append(QString::number(ErrorBytype));
				dataList.append(QString::number(info.iFrontErrorByType[k]));
				dataList.append(QString::number(info.iClampErrorByType[k]));
				dataList.append(QString::number(info.iRearErrorByType[k]));
				m_ExcelObj->SetRowData(row++,dataList);
			}
		}

		//合计
		dataList.clear();
		dataList<<tr("SUM")
			<<QString::number(info.GetFailCount())
			<<QString::number(info.GetFrontCount())
			<<QString::number(info.GetClampCount())
			<<QString::number(info.GetRearCount());

		m_ExcelObj->SetRowData(row++,dataList);

		m_ExcelObj->SetBolder(fristRow,row++);
	}

	m_ExcelObj->SaveAs(fileName);
	delete m_ExcelObj;
	m_ExcelObj = NULL;

	QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(fileName)));
	
}
