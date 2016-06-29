#include "QSettings"
#include "ipinput.h"
#include "ui_ipinput.h"

IPinput::IPinput(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IPinput)
{
    ui->setupUi(this);
    QSettings* settings = new QSettings("printerConfig.ini",QSettings::IniFormat);
    settings->beginGroup("print");
    ui->lineEdit->setText(settings->value("serverip").toString());
    settings->endGroup();
}

IPinput::~IPinput()
{
    delete ui;
}

void IPinput::on_connectIP_clicked()
{
    QSettings* settings = new QSettings("printerConfig.ini",QSettings::IniFormat);
    settings->beginGroup("print");
    QString ip = ui->lineEdit->text();
    settings->setValue("serverip",ip);
    settings->endGroup();
    emit Sig_ip(ui->lineEdit->text());
    this->close();
}

void IPinput::on_lineEdit_returnPressed()
{
    QSettings* settings = new QSettings("printerConfig.ini",QSettings::IniFormat);
    settings->beginGroup("print");
    QString ip = ui->lineEdit->text();
    settings->setValue("serverip",ip);
    settings->endGroup();
    emit Sig_ip(ui->lineEdit->text());
    this->close();
}
