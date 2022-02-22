#include "uiSettings.h"
#include "ui_uiSettings.h"

#include <QTimer>

UISettings::UISettings(QWidget *parent /*= 0*/)
    : QWidget(parent)
    , ui(new Ui::uiSettings)
{
    ui->setupUi(this);

}

UISettings::~UISettings()
{
    delete ui;
}
