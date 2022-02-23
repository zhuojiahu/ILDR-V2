#ifndef EXCELOBJECT_H
#define EXCELOBJECT_H

#include <QAxObject>
#include <QColor>
#include <QDir>
#include "Windows.h"

class ExcelObject : public QObject
{
	Q_OBJECT

public:
	enum ERRORTYPE{
		INIT_OK = 0,
		INIT_ERROR1,			//没有安装office
		INIT_ERROR2				//没有激活office
	};

	ExcelObject(QObject *parent=0);
	~ExcelObject();

	ERRORTYPE init();
	void SetGeneralInfo(int row,QString strInfo);
	void SetTatilRow(int row);
	void SetRowData(int row,QStringList pdatalist);
	void SetBolder(int startRow,int endRow);

	void SaveAs(QString fileName);

	QString m_Version;

private:
	QAxObject *excel;
	QAxObject *workbooks;
	QAxObject *workbook ;
	QAxObject *worksheet;
};

#endif // EXCELOBJECT_H
