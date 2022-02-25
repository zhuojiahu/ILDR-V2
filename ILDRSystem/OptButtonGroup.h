#ifndef OPTION_BUTTION_GROUP_H
#define OPTION_BUTTION_GROUP_H

#include <QButtonGroup>
 
class OptButtonGroup : public QButtonGroup
{
	Q_OBJECT

public:
	OptButtonGroup(QObject *parent);
	virtual ~OptButtonGroup();

    void selectAll();
    void selectNo();
    void invertSelection();
    void setManage(bool);
};

#endif // OPTION_BUTTION_GROUP_H
