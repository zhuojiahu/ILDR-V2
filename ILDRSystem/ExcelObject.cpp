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
	//���̱߳����ʼ��
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	excel=new QAxObject(this);
	if(!excel->setControl("Excel.Application"))
	{
		return INIT_ERROR1;
	}
	m_Version = excel->property("Version").toString();
/*		qDebug()<<"Excel Version: "<<m_Version;*/
	excel->dynamicCall("SetVisible (bool Visible)","false");//����ʾ����
	excel->setProperty("DisplayAlerts", false);//����ʾ�κξ�����Ϣ�����Ϊtrue��ô�ڹر��ǻ�������ơ��ļ����޸ģ��Ƿ񱣴桱����ʾ
	workbooks = excel->querySubObject("WorkBooks");//��ȡ����������
	if (workbooks == NULL)
	{
		return INIT_ERROR2;
	}
	workbooks->dynamicCall("Add");//�½�һ��������
	workbook = excel->querySubObject("ActiveWorkBook");//��ȡ��ǰ������
	if (workbook == NULL)
	{
		return INIT_ERROR2;
	}
	worksheet = workbook->querySubObject("Worksheets(int)", 1);
	return INIT_OK;
}

void ExcelObject::SetGeneralInfo(int row,QString strInfo)
{
	//������
	QAxObject *cell=worksheet->querySubObject("Cells(int,int)", row, 1);
	cell->dynamicCall("SetValue(const QString&)",strInfo);
	cell->querySubObject("Font")->setProperty("Size", 12);
	cell->querySubObject("Interior")->setProperty("Color",QColor(250, 250, 250));
	//�����и�
	cell->setProperty("RowHeight", 80);
	//worksheet->querySubObject("Range(const QString&)", QString("%1:1").arg(row))->setProperty("RowHeight", 80);
	//�ϲ�������
	QString cellTitle;
	cellTitle.append(QString("A%1:").arg(row));
	cellTitle.append(QChar('E'));
	cellTitle.append(QString("%1").arg(row));
	QAxObject *range = worksheet->querySubObject("Range(const QString&)", cellTitle);
	range->setProperty("WrapText", true); //�Զ�������
	range->setProperty("MergeCells", true);
	range->setProperty("HorizontalAlignment", -4131);	//����루xlLeft����-4131  ���У�xlCenter����-4108  �Ҷ��루xlRight����-4152
	range->setProperty("VerticalAlignment", -4108);		//�϶��루xlTop��-4160 ���У�xlCenter����-4108  �¶��루xlBottom����-4107
}

void ExcelObject::SetTatilRow( int row )
{
#if 1
	//�����п�
	QString columnName;
	columnName.append(QChar('A'));
	columnName.append(":");
	columnName.append(QChar('E'));
	QAxObject *col = worksheet->querySubObject("Columns(const QString&)", columnName);
	col->setProperty("ColumnWidth",12);
	//���ñ����ı�
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
	range->setProperty("HorizontalAlignment", -4108);//����루xlLeft����-4131  ���У�xlCenter����-4108  �Ҷ��루xlRight����-4152
	range->setProperty("VerticalAlignment", -4108);	//�϶��루xlTop��-4160 ���У�xlCenter����-4108  �¶��루xlBottom����-4107

#else
	QString columnName;
	columnName.append(QChar( 'A'));
	columnName.append(":");
	columnName.append(QChar(24-1 + 'A'));
	QAxObject *col = worksheet->querySubObject("Columns(const QString&)", columnName);
	col->setProperty("ColumnWidth",12);
	//�б���
	for(int i=0;i<24;i++)
	{
		QAxObject *cell=worksheet->querySubObject("Cells(int,int)", row, i+1);
		//QTableView ��ȡ���ͷ��������Ϣ
		//columnName=ui->tableView_right->model()->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString();
		switch(i)
		{
		case 0:
			cell->dynamicCall("SetValue(const QString&)", QString::fromLocal8Bit("ͳ�ƽ��"));
			break;
		case 1:
			cell->dynamicCall("SetValue(const QString&)", QString::fromLocal8Bit("�ϼ�"));
			break;
		case 2:
			cell->dynamicCall("SetValue(const QString&)", QString::fromLocal8Bit("����ռ��"));
			break;
		default:
			cell->dynamicCall("SetValue(const QString&)", tr("Camera%1").arg(i-2));
			break;
		}
		cell->querySubObject("Font")->setProperty("Bold", true);
		cell->querySubObject("Interior")->setProperty("Color",QColor(191, 191, 191));
		cell->setProperty("HorizontalAlignment", -4108);//����루xlLeft����-4131  ���У�xlCenter����-4108  �Ҷ��루xlRight����-4152
		cell->setProperty("VerticalAlignment", -4108);	//�϶��루xlTop��-4160 ���У�xlCenter����-4108  �¶��루xlBottom����-4107
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
			cell->setProperty("HorizontalAlignment", -4108);//����루xlLeft����-4131  ���У�xlCenter����-4108  �Ҷ��루xlRight����-4152
			cell->setProperty("VerticalAlignment", -4108);	//�϶��루xlTop��-4160 ���У�xlCenter����-4108  �¶��루xlBottom����-4107
		}
		else
		{
			cell->setProperty("HorizontalAlignment", -4152);//����루xlLeft����-4131  ���У�xlCenter����-4108  �Ҷ��루xlRight����-4152
			cell->setProperty("VerticalAlignment", -4108);	//�϶��루xlTop��-4160 ���У�xlCenter����-4108  �¶��루xlBottom����-4107
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
	workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(fileName));//������fileName
	workbook->dynamicCall("Close()");//�رչ�����
	excel->dynamicCall("Quit()");	//�ر�excel
}