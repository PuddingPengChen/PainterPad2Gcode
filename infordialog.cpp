#include "infordialog.h"
#include "ui_infordialog.h"

InforDialog::InforDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InforDialog)
{
    ui->setupUi(this);
}

InforDialog::~InforDialog()
{
    delete ui;
}

void InforDialog::setTitles(QString title)
{
    ui->infotitle->setText(title);
}

void InforDialog::setInformation(QString infos)
{
    ui->info->setText(infos);
}
