#include "MyLogText.h"
#include <QTime>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QMenu>


CMyLogText::CMyLogText(QWidget *parent)
	: QPlainTextEdit(parent)
{
	setMaximumBlockCount(MAXLOGTEXTCOUNT);//保留100段日志记录
	clearAction = new QAction(this);
	connect(clearAction,SIGNAL(triggered()),this,SLOT(slots_clearLog()));
}

CMyLogText::~CMyLogText()
{

}

void CMyLogText::slots_writeLogText(QString strInfo,e_SaveLogType logtype)
{
	//写入文件框
	QString strDis;
	QString curDateTime = QDateTime::currentDateTime().date().toString(Qt::ISODate)+" "+QDateTime::currentDateTime().time().toString(Qt::ISODate);	
	switch(logtype)
	{
	case OperationLog:
		strDis = "<font color=black>"+strInfo+"<\font>";
		break;
	case DebugLog:
		strDis = "<font color=red>"+strInfo+"<\font>";
		break;
	case AbnormityLog:
		strDis = "<font color=black>"+strInfo+"<\font>";
		break;
	default:
		strDis = "<font color=black>"+strInfo+"<\font>";
		break;
	}
	//if (bWarning)
	//{
	//	strDis = "<font color=red>"+strInfo+"<\font>";
	//}
	//else
	//{
	//	strDis = "<font color=black>"+strInfo+"<\font>";
	//}
	strDis = "<font color=blue>"+curDateTime+">> "+"<\font>"+strDis;
	appendHtml(strDis);
	setCenterOnScroll(false);
}

void CMyLogText::writeLog(QString strInfo,QColor color)
{
	//写入文件框
	QString strDis;
	QString curDateTime = QDateTime::currentDateTime().time().toString(Qt::ISODate);	
	setInsertTextColor(color);

	strDis = strInfo;
	strDis = curDateTime+">> "+strDis;
	appendHtml(strDis);
	setCenterOnScroll(false);
}
//功能：重载右键上下文菜单
void CMyLogText::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = new QMenu;
	clearAction->setText(tr("Clear"));
	menu->addAction(clearAction);
	menu->exec(event->globalPos());
	delete menu;
}
void CMyLogText::slots_clearLog()
{
	clear();
}
void CMyLogText::setInsertTextColor(QColor col)//,BackOrFore wground)
{
	QTextCharFormat fmt;
	fmt.setForeground(col);
	QTextCursor cursor = textCursor();
	cursor.mergeCharFormat(fmt);
	mergeCurrentCharFormat(fmt);//调用QTextEdit或者QPlianTextEdit的api;
}