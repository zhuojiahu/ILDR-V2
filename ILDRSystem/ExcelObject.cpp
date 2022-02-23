#include "ExcelObject.h"
#include <QDebug>


ExcelObject::ExcelObject(QObject *parent)
	: QObject(parent)
{
	m_Version = "";
}

ExcelObject::~ExcelObject()
{
	delete excel;
}

ExcelObject::ERRORTYPE ExcelObject::init()
{
	//多线程必须初始化
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	excel=new QAxObject(this);
	if(!excel->setControl("Excel.Application"))
	{
		return INIT_ERROR1;
	}
	m_Version = excel->property("Version").toString();
/*		qDebug()<<"Excel Version: "<<m_Version;*/
	excel->dynamicCall("SetVisible (bool Visible)","false");//不显示窗体
	excel->setProperty("DisplayAlerts", false);//不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
	workbooks = excel->querySubObject("WorkBooks");//获取工作簿集合
	if (workbooks == NULL)
	{
		return INIT_ERROR2;
	}
	workbooks->dynamicCall("Add");//新建一个工作簿
	workbook = excel->querySubObject("ActiveWorkBook");//获取当前工作簿
	if (workbook == NULL)
	{
		return INIT_ERROR2;
	}
	worksheet = workbook->querySubObject("Worksheets(int)", 1);
	return INIT_OK;
}

void ExcelObject::SetGeneralInfo(int row,QString strInfo)
{
	//标题行
	QAxObject *cell=worksheet->querySubObject("Cells(int,int)", row, 1);
	cell->dynamicCall("SetValue(const QString&)",strInfo);
	cell->querySubObject("Font")->setProperty("Size", 12);
	cell->querySubObject("Interior")->setProperty("Color",QColor(250, 250, 250));
	//调整行高
	cell->setProperty("RowHeight", 80);
	//worksheet->querySubObject("Range(const QString&)", QString("%1:1").arg(row))->setProperty("RowHeight", 80);
	//合并标题行
	QString cellTitle;
	cellTitle.append(QString("A%1:").arg(row));
	cellTitle.append(QChar('E'));
	cellTitle.append(QString("%1").arg(row));
	QAxObject *range = worksheet->querySubObject("Range(const QString&)", cellTitle);
	range->setProperty("WrapText", true); //自动化换行
	range->setProperty("MergeCells", true);
	range->setProperty("HorizontalAlignment", -4131);	//左对齐（xlLeft）：-4131  居中（xlCenter）：-4108  右对齐（xlRight）：-4152
	range->setProperty("VerticalAlignment", -4108);		//上对齐（xlTop）-4160 居中（xlCenter）：-4108  下对齐（xlBottom）：-4107
}

void ExcelObject::SetTatilRow( int row )
{
#if 1
	//设置列宽
	QString columnName;
	columnName.append(QChar('A'));
	columnName.append(":");
	columnName.append(QChar('E'));
	QAxObject *col = worksheet->querySubObject("Columns(const QString&)", columnName);
	col->setProperty("ColumnWidth",12);
	//设置标题文本
	QStringList Tatilstrs;
	Tatilstrs<<tr("Result")<<tr("SUM")<<tr("Front")<<tr("Clamp")<<tr("Rear");

	QString rowName;
	rowName.append(QChar('A'));
	rowName.append(QString("%1").arg(row));
	rowName.append(":");
	rowName.append(QChar('E'));
	rowName.append(QString("%1").arg(row));
	QAxObject *range = worksheet->querySubObject("Range(QString)",rowName);
	QVariant d(Tatilstrs);
	range->setProperty("Value", d);
	range->querySubObject("Font")->setProperty("Bold", true);
	range->querySubObject("Interior")->setProperty("Color",QColor(191, 191, 191));
	range->setProperty("HorizontalAlignment", -4108);//左对齐（xlLeft）：-4131  居中（xlCenter）：-4108  右对齐（xlRight）：-4152
	range->setProperty("VerticalAlignment", -4108);	//上对齐（xlTop）-4160 居中（xlCenter）：-4108  下对齐（xlBottom）：-4107

#else
	QString columnName;
	columnName.append(QChar( 'A'));
	columnName.append(":");
	columnName.append(QChar(24-1 + 'A'));
	QAxObject *col = worksheet->querySubObject("Columns(const QString&)", columnName);
	col->setProperty("ColumnWidth",12);
	//列标题
	for(int i=0;i<24;i++)
	{
		QAxObject *cell=worksheet->querySubObject("Cells(int,int)", row, i+1);
		//QTableView 获取表格头部文字信息
		//columnName=ui->tableView_right->model()->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString();
		switch(i)
		{
		case 0:
			cell->dynamicCall("SetValue(const QString&)", QString::fromLocal8Bit("统计结果"));
			break;
		case 1:
			cell->dynamicCall("SetValue(const QString&)", QString::fromLocal8Bit("合计"));
			break;
		case 2:
			cell->dynamicCall("SetValue(const QString&)", QString::fromLocal8Bit("错误占比"));
			break;
		default:
			cell->dynamicCall("SetValue(const QString&)", tr("Camera%1").arg(i-2));
			break;
		}
		cell->querySubObject("Font")->setProperty("Bold", true);
		cell->querySubObject("Interior")->setProperty("Color",QColor(191, 191, 191));
		cell->setProperty("HorizontalAlignment", -4108);//左对齐（xlLeft）：-4131  居中（xlCenter）：-4108  右对齐（xlRight）：-4152
		cell->setProperty("VerticalAlignment", -4108);	//上对齐（xlTop）-4160 居中（xlCenter）：-4108  下对齐（xlBottom）：-4107
	}
#endif
}

void ExcelObject::SetRowData( int row,QStringList pdatalist )
{
	if (pdatalist.isEmpty())
		return;
#if 1
	QString columnName;
	columnName.append(QChar('A'));
	columnName.append(QString("%1").arg(row));
	columnName.append(":");
	columnName.append(QChar('E'));
	columnName.append(QString("%1").arg(row));
	QAxObject *range = worksheet->querySubObject("Range(QString)",columnName);
	QVariant d(pdatalist);
	range->setProperty("Value", d);
	range->querySubObject("Interior")->setProperty("Color",QColor(250, 250, 250));
#else
	for (int i=0;i<pdatalist.count();i++)
	{
		QAxObject *cell=worksheet->querySubObject("Cells(int,int)", row, i+1);
		cell->dynamicCall("SetValue(const QString&)", pdatalist[i]);
		if (i==0)
		{
			cell->querySubObject("Font")->setProperty("Bold", true);
			cell->querySubObject("Interior")->setProperty("Color",QColor(235, 235, 235));
			cell->setProperty("HorizontalAlignment", -4108);//左对齐（xlLeft）：-4131  居中（xlCenter）：-4108  右对齐（xlRight）：-4152
			cell->setProperty("VerticalAlignment", -4108);	//上对齐（xlTop）-4160 居中（xlCenter）：-4108  下对齐（xlBottom）：-4107
		}
		else
		{
			cell->setProperty("HorizontalAlignment", -4152);//左对齐（xlLeft）：-4131  居中（xlCenter）：-4108  右对齐（xlRight）：-4152
			cell->setProperty("VerticalAlignment", -4108);	//上对齐（xlTop）-4160 居中（xlCenter）：-4108  下对齐（xlBottom）：-4107
		}
		
	}
#endif
}


void ExcelObject::SetBolder( int startRow,int endRow )
{
	QString lrange;
	lrange.append(QString("A%1:").arg(startRow));
	lrange.append(24 - 1 + 'A');
	lrange.append(QString::number(endRow));
	QAxObject *range = worksheet->querySubObject("Range(const QString&)", lrange);
	range->querySubObject("Borders")->setProperty("LineStyle", QString::number(1));
	range->querySubObject("Borders")->setProperty("Color", QColor(0, 0, 0));
	QString rowsName;
	rowsName.append(QString("%1:%2").arg(startRow+1).arg(endRow));
	range = worksheet->querySubObject("Rows(const QString&)", rowsName);
	range->setProperty("RowHeight", 20);
}

void ExcelObject::SaveAs( QString fileName )
{
	workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(fileName));//保存至fileName
	workbook->dynamicCall("Close()");//关闭工作簿
	excel->dynamicCall("Quit()");	//关闭excel
}