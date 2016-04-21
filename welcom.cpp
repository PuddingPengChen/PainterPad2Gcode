#include "welcom.h"
#include "ui_welcom.h"
#include <QMessageBox>


Welcom::Welcom(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Welcom)
{
    ui->setupUi(this);
    mPainter = new iPainter();
    mPrinterSettings = new ParentSetting();
    IPDialog = new IPinput();
    doors = new Door();

    //IP---->DOOR
    connect(IPDialog,SIGNAL(Sig_ip(QString)),doors,SLOT( connectServer(QString)));

    //This---->DOOR
    connect(this,SIGNAL(Sig_ip(QString)),doors,SLOT( connectServer(QString)));

    //DOOR------>This
    connect(doors,SIGNAL(Sig_connected(QString)),this,SLOT(connectStatus(QString)));
    connect(doors,SIGNAL(Sig_msg(QString)),this,SLOT(showMsg(QString)));

    //ParentSetting---->DOOR
    connect(mPrinterSettings,SIGNAL(Sig_cmds(QString)),doors,SLOT(sendCommand(QString)));
    connect(mPrinterSettings,SIGNAL(Sig_printgcode(QString)),doors,SLOT(sendFile(QString)));

    //iPainter---->DOOR
    connect(mPainter,SIGNAL(Sig_file(QString)),doors,SLOT(sendFile(QString)));
    connect(mPainter,SIGNAL(Sig_stop()),mPrinterSettings,SLOT(slotStop()));
    connect(mPainter,SIGNAL(Sig_cancle()),mPrinterSettings,SLOT(slotCancle()));

    this->setWindowTitle(tr("Make-Block"));
    this->setWindowFlags(Qt::FramelessWindowHint);
}

Welcom::~Welcom()
{
    delete ui;
}
void Welcom::connectStatus(QString t)       //服务器发送过来的连接状态
{
    if(t=="Suc")
    {
        QMessageBox::information(this,tr("通知"),tr("已连接打印机"));
    }
}
//接收来自orangepi的数据
void Welcom::showMsg(QString t)
{
    qDebug()<<"get orange pi data:"<<t;
}
void Welcom::on_ipaint_clicked()
{
    mPainter->show();
}

void Welcom::on_parentSetting_clicked()         //家长设置
{
    mPrinterSettings->show();
}

void Welcom::on_connectIP_clicked()
{
    emit Sig_ip("192.168.0.1");
}

void Welcom::on_closeBtn_clicked()
{
    this->close();
}
