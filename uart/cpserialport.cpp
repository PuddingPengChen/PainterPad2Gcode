#include "cpserialport.h"
#include "ui_cpserialport.h"

CPSerialPort::CPSerialPort(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CPSerialPort)
{
    ui->setupUi(this);
    mySerialport = new QSerialPort();
    //init serial and show it in the UI combo
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if (serial.open(QIODevice::ReadWrite))
        {
            qDebug()<<info.portName()<<info.description()<<info.manufacturer();
            comList.append(info.portName());
            serial.close();
        }
    }
    ui->portCombo->addItems(comList);
}

CPSerialPort::~CPSerialPort()
{
    delete ui;
}


void CPSerialPort::Slot_ResponseUart()
{
    QString po = mySerialport->readAll();
//    QString po = mySerialport->readLine();
    emit Sig_GetMessage(po);
}
void CPSerialPort::WritePort(QString str)
{
    if(bConnect)
    {
        QString s = str + "\n";
        mySerialport->write(s.toLatin1());
//        emit Sig_GetMessage("Send:"+str);
    }
    else
    {
        QMessageBox::information(this,tr("Notice"),tr("Serial open failed."));
    }
}

void CPSerialPort::on_updateUart_clicked()
{
    ui->portCombo->clear();
    comList.clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if (serial.open(QIODevice::ReadWrite))
        {
            qDebug()<<info.portName()<<info.description()<<info.manufacturer();
            comList.append(info.portName());
            serial.close();
        }
    }
    ui->portCombo->addItems(comList);
}

void CPSerialPort::on_btnConnect_clicked()
{
    if(!bConnect)
    {
        QString comName = ui->portCombo->currentText();
        mySerialport->setPortName(comName);
        mySerialport->setBaudRate(115200);
        mySerialport->setDataBits(QSerialPort::Data8);
        mySerialport->setParity(QSerialPort::NoParity);
        mySerialport->setStopBits(QSerialPort::OneStop);
        mySerialport->setFlowControl(QSerialPort::NoFlowControl);
        if(mySerialport->open(QIODevice::ReadWrite))
        {
            connect(mySerialport,SIGNAL(readyRead()),this,SLOT(Slot_ResponseUart()));
            ui->btnConnect->setText(tr("断开"));
            bConnect = true;
            this->close();
            //QMessageBox::information(this,tr("Notice"),tr("Serial open successful."));
        }
        else
        {
            QMessageBox::information(this,tr("Notice"),tr("Serial open failed."));
        }

    }
    else
    {
        bConnect = false;
        ui->btnConnect->setText(tr("连接"));
        mySerialport->disconnect();
        mySerialport->close();
    }
}

