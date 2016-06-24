#ifndef POTRACEWIDGET_H
#define POTRACEWIDGET_H

#include <QWidget>
#include <QxPotrace>
#include <QSvgGenerator>
#include <QPainter>
#include <QFileDialog>
#include <QImage>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QDebug>


#include "svgpath.h"

namespace Ui {
class PotraceWidget;
}

class PotraceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PotraceWidget(QWidget *parent = 0);
    ~PotraceWidget();
    void loadImage(QString);
    void outSvg();
    void outGcode();

protected:
      void paintEvent(QPaintEvent *event);

private:
    void paint(QPainter &painter);
    QxPotrace potrace;
    Ui::PotraceWidget *ui;
    QGraphicsView *view;
    QGraphicsScene *scene;

    svgPath* svgs;
};

#endif // POTRACEWIDGET_H
