#include "uiOperation.h"
#include "ui_UIOperation.h"

#include <QTimer>

UIOperation::UIOperation(QWidget *parent /*= 0*/)
    : QWidget(parent)
    , ui(new Ui::uiOperation)
{
    ui->setupUi(this);

    bSucceed = false;
    
    nTotal = 0;
    nRead = 0;
    nReject = 0;
    nInTime = 0;
    nEngraved = 0;

    trUpdate = new QTimer(this);
    connect(trUpdate, SIGNAL(timeout()), this, SLOT(slots_updateCount()));
    trUpdate->start(1000);
}

UIOperation::~UIOperation()
{
    trUpdate->stop();

    delete ui;
}

void UIOperation::slots_showCameraImage(int nImgNO, QImage* img)
{
    ui->lCamImage->setPixmap(QPixmap::fromImage(*img));
}

void UIOperation::slots_showResult(int nImgNO)
{

}

void UIOperation::slots_updateCount()
{
    ui->lCountTotal->setText(QString::number(nTotal));
    ui->lCountReject->setText(QString::number(nReject));
    ui->lCountRead->setText(QString("%1/%2").arg(nRead).arg(nTotal-nRead));
    ui->lCountInTime->setText(QString("%1/%2").arg(nInTime).arg(nRead - nInTime));
    ui->lCountEngraved->setText(QString("%1/%2").arg(nEngraved).arg(nTotal - nEngraved));
}

void UIOperation::slots_updateCount(int total, int reject, int read, int intime, int engraved)
{
    nTotal = total;
    nRead = read;
    nReject = reject;
    nInTime = intime;
    nEngraved = engraved;
}

void UIOperation::slots_resetCurrent()
{
    nTotal = 0;
    nRead = 0;
    nReject = 0;
    nInTime = 0;
    nEngraved = 0;
}

void UIOperation::on_btnReset_clicked()
{
    emit signals_resetCount();
}
