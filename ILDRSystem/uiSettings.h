#ifndef UI_SETTINGS_H_
#define UI_SETTINGS_H_

#include <QWidget>
#include <QDateTime>

namespace Ui
{
    class uiSettings;
}
class QTimer;

class UISettings : public QWidget
{
    Q_OBJECT
public:
	explicit UISettings(QWidget *parent = 0);
	virtual ~UISettings();

private:
    Ui::uiSettings* ui;
};

#endif //UI_SETTINGS_H_
