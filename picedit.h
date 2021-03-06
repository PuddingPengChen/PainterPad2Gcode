#ifndef PICEDIT_H
#define PICEDIT_H

#include <QWidget>
#include "./potrace-gcode/potracewidget.h"

namespace Ui {
class PicEdit;
}

class PicEdit : public QWidget
{
    Q_OBJECT

public:
    explicit PicEdit(QWidget *parent = 0);
    ~PicEdit();
    void LoadPicture(QString file);
private slots:
    void on_startPrint_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_btngray_clicked();

private:
    Ui::PicEdit *ui;
    QImage gray_image;
    void GrayScalePicture(int grayscale);
    QImage Image2GrayScale(QImage image);
    void ExportGrayPix(QImage src);
    bool preOn;

    PotraceWidget* potrace;
    QImage currentImage;
signals:
    void Sig_Print();
};

#endif // PICEDIT_H
