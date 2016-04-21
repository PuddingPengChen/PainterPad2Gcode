#ifndef IPAINTER_H
#define IPAINTER_H

#include <QWidget>
#include "picture.h"
#include "./SliceEngine/fffProcessor.h"
#include "./SliceEngine/settings.h"
#include "infordialog.h"

namespace Ui {
class iPainter;
}

class iPainter : public QWidget
{
    Q_OBJECT

public:
    explicit iPainter(QWidget *parent = 0);
    ~iPainter();

private slots:
    void on_newFile_clicked();

    void on_erase_clicked();

    void on_blackPen_clicked();

    void on_redPen_clicked();

    void on_bluePen_clicked();

    void on_greenPen_clicked();

    void on_self_clicked();

    void on_small_clicked();

    void on_middle_clicked();

    void on_big_clicked();

    void on_open_clicked();

    void on_save_clicked();

    void on_deleteFile_clicked();
    void on_paintfood_clicked();

    void on_animal_clicked();

    void on_human_clicked();

    void on_plant_clicked();

    void on_building_clicked();
    void showModel(QString);
    void on_back_clicked();

    void on_stop_clicked();

    void on_cancle_clicked();

signals:
    void Sig_file(QString);
    void Sig_stop();
    void Sig_cancle();
private:
    Ui::iPainter *ui;
    bool _saved;
    bool _opened;
    int pen_size;
    QString _filename;

    Picture* animal;
    Picture* human;
    Picture* tool;
    Picture* katong;
    Picture* plant;

    void Enable_User_Waiting_Cursor();
    void Disable_User_Waiting_Cursor();

    QImage changeGray(QString img,int grayscale);
    InforDialog* dialog;
};

#endif // IPAINTER_H
