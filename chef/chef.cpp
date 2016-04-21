#include "chef.h"
#include "ui_chef.h"

chef::chef(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chef)
{
    ui->setupUi(this);
}

chef::~chef()
{
    delete ui;
}
