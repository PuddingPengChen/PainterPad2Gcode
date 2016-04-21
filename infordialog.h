#ifndef INFORDIALOG_H
#define INFORDIALOG_H

#include <QDialog>

namespace Ui {
class InforDialog;
}

class InforDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InforDialog(QWidget *parent = 0);
    ~InforDialog();
    void setTitles(QString title);
    void setInformation(QString infos);
private:
    Ui::InforDialog *ui;
};

#endif // INFORDIALOG_H
