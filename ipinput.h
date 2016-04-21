#ifndef IPINPUT_H
#define IPINPUT_H

#include <QWidget>

namespace Ui {
class IPinput;
}

class IPinput : public QWidget
{
    Q_OBJECT

public:
    explicit IPinput(QWidget *parent = 0);
    ~IPinput();
signals:
    void Sig_ip(QString ip);
private slots:
    void on_connectIP_clicked();

private:
    Ui::IPinput *ui;
};

#endif // IPINPUT_H
