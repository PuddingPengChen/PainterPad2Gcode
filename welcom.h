#ifndef WELCOM_H
#define WELCOM_H

#include <QMainWindow>
#include "./painter/ipainter.h"
#include "parentsetting.h"
#include "ipinput.h"
#include "door.h"

#include "./uart/cpserialport.h"

namespace Ui {
class Welcom;
}

class Welcom : public QMainWindow
{
    Q_OBJECT

public:
    explicit Welcom(QWidget *parent = 0);
    ~Welcom();
public slots:
    void connectStatus(QString);
    void showMsg(QString);
private slots:
    void on_ipaint_clicked();

    void on_parentSetting_clicked();

    void on_connectIP_clicked();

    void on_closeBtn_clicked();

private:
    Ui::Welcom *ui;
    iPainter* mPainter;
    ParentSetting* mPrinterSettings;
    IPinput * IPDialog;
    Door* doors;
    CPSerialPort * myport;
signals:
    void Sig_ip(QString);
};

#endif // WELCOM_H
