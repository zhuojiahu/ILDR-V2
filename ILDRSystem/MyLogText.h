#ifndef MYLOGTEXT_H
#define MYLOGTEXT_H

#include <QPlainTextEdit>
#include <QAction>
#include "common.h"

#define MAXLOGTEXTCOUNT 100		//文本框最大日志100条

class CMyLogText : public QPlainTextEdit
{
	Q_OBJECT
		
public:
	CMyLogText(QWidget *parent);
	~CMyLogText();
	void writeLog(QString strInfo,QColor color);
protected:
	void contextMenuEvent(QContextMenuEvent *event);
	void setInsertTextColor(QColor col);
private slots:
	void slots_clearLog();
	void slots_writeLogText(QString strInfo,e_SaveLogType logtype);

private:
	QAction *clearAction;

};

#endif // MYLOGTEXT_H
