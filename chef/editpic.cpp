#include "editpic.h"
#include "ui_editpic.h"

editPic::editPic(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::editPic)
{
    ui->setupUi(this);
}

editPic::~editPic()
{
    delete ui;
}
