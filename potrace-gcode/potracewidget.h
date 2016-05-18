#ifndef POTRACEWIDGET_H
#define POTRACEWIDGET_H

#include <QWidget>
#include <QxPotrace>
#include <QFileDialog>
#include <QImage>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QSvgGenerator>
#include <QDebug>
#include <QPainter>

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
    Ui::PotraceWidget *ui;
    QGraphicsView *view;
    QGraphicsScene *scene;
    QxPotrace potrace;
    svgPath* svgs;
};

#endif // POTRACEWIDGET_H
