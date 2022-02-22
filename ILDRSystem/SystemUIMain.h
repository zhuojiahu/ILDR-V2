#ifndef ILDRSYSTEM_H
#define ILDRSYSTEM_H

#include <QWidget>

class QStackedWidget;
class QLabel;

class SystemUIMain : public QWidget
{
    Q_OBJECT
public:
    SystemUIMain(QWidget *parent = 0);
    ~SystemUIMain();
    
public:
    void Init();
    bool changeLanguage(int nLangIdx);
signals:
    //操作
    void signals_turnPage(int);
    //设置状态
    void signals_setNaviLockSt(bool);
    void signals_setCurrentPage(int page);
public slots:
    void slots_NaviOperation(int op);
    void slots_switchPage(int current_page);
    bool isLock()const;
    bool checkLock();

private:
    void initInterface();    //初始化界面
private:
    //ui控件
    QStackedWidget* statked_widget;


    QLabel* lStatus;    //用于显示状态信息
    QLabel* lVersion;   //用于显示程序版本


    //控制参数
    int iLastPage;
    bool bLock;
    
};

#endif // ILDRSYSTEM_H

