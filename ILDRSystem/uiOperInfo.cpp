#include "uiOperInfo.h"
#include "ui_uiOperInfo.h"

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

void UIOperInfo::slots_updateResult(int _nImgNO, QString _id, QDateTime _dt, QString _cavityNum, QDateTime _gmtTime)
{
    imgNO = _nImgNO;
    id = _id;
    dtResult = _dt;
    CavityNumber = _cavityNum;
    gmtTimeStamp = _gmtTime;

    ui->lRelsultImgNO->setText(QString::number(imgNO));
    if(id == "")
    {
        ui->lRelsultID->setText("Error");
        ui->lRelsultTime->setText("");
        ui->lRelsultTimeStamp->setText("");
    }
    else
    {
        ui->lRelsultID->setText(id);
        ui->lRelsultTime->setText(QString("%1,%2,%3").arg(dtResult.date().toString(Qt::ISODate))
            .arg(dtResult.time().toString("hh:mm:ss")).arg(CavityNumber));
        ui->lRelsultTimeStamp->setText(gmtTimeStamp.toString("dd/MM/yyyy hhh mmm sss t"));
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
