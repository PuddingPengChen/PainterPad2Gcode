#ifndef PARENTSETTING_H
#define PARENTSETTING_H

#include <QWidget>

namespace Ui {
class ParentSetting;
}

class ParentSetting : public QWidget
{
    Q_OBJECT

public:
    explicit ParentSetting(QWidget *parent = 0);
    ~ParentSetting();
public slots:
    void slotStop();
    void slotCancle();
signals:
    void Sig_cmds(QString);
    void Sig_printgcode(QString);
private slots:
    void on_xUp_clicked();

    void on_xDown_clicked();

    void on_yUp_clicked();

    void on_yDown_clicked();

    void on_zUp_clicked();

    void on_zDown_clicked();

    void on_eOut_clicked();

    void on_eIn_clicked();

    void on_xHome_clicked();

    void on_yHome_clicked();

    void on_zHome_clicked();

    void on_Home_clicked();

    void on_current_z_clicked();

    void on_small_pressed();

    void on_middle_clicked();

    void on_huge_clicked();

    void on_pushButton_clicked();

    void on_stoped_clicked();

    void on_recovery_clicked();

    void on_cancle_clicked();

    void on_printGcode_clicked();

    void on_back_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::ParentSetting *ui;
    int steps;
    void move(QString axis);
    void home(QString axis);
};

#endif // PARENTSETTING_H
