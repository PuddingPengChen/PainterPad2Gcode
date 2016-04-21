#ifndef PICTURE_H
#define PICTURE_H

#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
namespace Ui {
class Picture;
}

class Picture : public QWidget
{
    Q_OBJECT

public:
    explicit Picture(QWidget *parent = 0,QString dir = "");
    ~Picture();
    void loadImages(QString dir);
    void reUpdate();
public slots:
    void Slot_ItemClicked(QListWidgetItem * item);
private slots:
    void on_pre_clicked();

    void on_next_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::Picture *ui;
    int currentPage;
    int currentPic;
    int totalPic;
    QString dirP;
    const int W_ICONSIZE = 180; //图片宽度
    const int H_ICONSIZE = 160; //图片高度

signals:
    void Sig_drawPath(QString);
};

#endif // PICTURE_H
