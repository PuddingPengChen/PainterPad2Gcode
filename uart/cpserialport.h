#ifndef CPSERIALPORT_H
#define CPSERIALPORT_H

#include <QWidget>
#include <QDebug>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

namespace Ui {
class CPSerialPort;
}

class CPSerialPort : public QWidget
{
    Q_OBJECT

public:
    explicit CPSerialPort(QWidget *parent = 0);
    ~CPSerialPort();

public slots:
    void Slot_ResponseUart();
    void WritePort(QString str);
private:
    Ui::CPSerialPort *ui;
    QSerialPort *mySerialport;
    QStringList comList;
    bool bConnect;
signals:
    void Sig_GetMessage(QString);
private slots:
    void on_updateUart_clicked();
    void on_btnConnect_clicked();
};

#endif // CPSERIALPORT_H
