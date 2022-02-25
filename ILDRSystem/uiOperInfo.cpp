#include "uiOperInfo.h"
#include "ui_uiOperInfo.h"

#include <ctime>

#include <QTimer>

UIOperInfo::UIOperInfo(QWidget *parent /*= 0*/)
    : QWidget(parent)
    , ui(new Ui::uiOperInfo)
{
    ui->setupUi(this);

    bSucceed = false;
    
    nTotal = 0;
    nRead = 0;
    nReject = 0;
    nInTime = 0;
    nEngraved = 0;
}

UIOperInfo::~UIOperInfo()
{
    delete ui;
}

void UIOperInfo::slots_updateResult(BottleResult res)
{
    curResult = res;

    ui->lRelsultImgNO->setText(QString::number(curResult.imgNO));
    if(curResult.idLine == "")
    {
        ui->lRelsultID->setText("Error");
        ui->lRelsultTimeStamp->setText("");
        ui->lRelsultCavity->setText("");
    }
    else
    {
        ui->lRelsultID->setText(curResult.idLine);
        ui->lRelsultCavity->setText(curResult.cavityNum);
        ui->lRelsultTimeStamp->setText(curResult.dtStamp.toString("dd/MM/yyyy hh:mm:ss"));
    }
}

void UIOperInfo::slots_updateCount(int total, int reject, int read, int intime, int engraved)
{
    nTotal = total;
    nRead = read;
    nReject = reject;
    nInTime = intime;
    nEngraved = engraved;
    ui->lCountTotal->setText(QString::number(nTotal));
    ui->lCountReject->setText(QString::number(nReject));
    ui->lCountRead->setText(QString("%1/%2").arg(nRead).arg(nTotal-nRead));
    ui->lCountInTime->setText(QString("%1/%2").arg(nInTime).arg(nRead - nInTime));
    ui->lCountEngraved->setText(QString("%1/%2").arg(nEngraved).arg(nTotal - nEngraved));
}

void UIOperInfo::slots_resetCurrent()
{
    slots_updateCount(0,0,0,0,0);
}
