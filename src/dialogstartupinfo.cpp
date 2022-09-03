#include "dialogstartupinfo.h"
#include "ui_dialogstartupinfo.h"

DialogStartupInfo::DialogStartupInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogStartupInfo)
{
    ui->setupUi(this);
}

void DialogStartupInfo::setDialogTitle(const QString str) {
    this->ui->mainTitle->setText(str);
}

void DialogStartupInfo::setDialogDescription(const QString str) {
    this->ui->mainText->setText(str);
}

DialogStartupInfo::~DialogStartupInfo()
{
    delete ui;
}
