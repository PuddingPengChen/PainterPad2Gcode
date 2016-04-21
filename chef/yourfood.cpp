#include "yourfood.h"
#include "ui_yourfood.h"

yourfood::yourfood(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::yourfood)
{
    ui->setupUi(this);
}

yourfood::~yourfood()
{
    delete ui;
}
