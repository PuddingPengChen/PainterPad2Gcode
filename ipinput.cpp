#include "ipinput.h"
#include "ui_ipinput.h"

IPinput::IPinput(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IPinput)
{
    ui->setupUi(this);
}

IPinput::~IPinput()
{
    delete ui;
}

void IPinput::on_connectIP_clicked()
{
    emit Sig_ip(ui->lineEdit->text());
    this->close();
}

void IPinput::on_lineEdit_returnPressed()
{
    emit Sig_ip(ui->lineEdit->text());
    this->close();
}
