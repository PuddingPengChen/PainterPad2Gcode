#include "potracewidget.h"
#include "ui_potracewidget.h"
#include <qmath.h>

PotraceWidget::PotraceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PotraceWidget)
{
    ui->setupUi(this);
    view = new QGraphicsView();
    scene = new QGraphicsScene();
    view = new QGraphicsView();
    view->setScene(scene);
    svgs = new svgPath();
}

PotraceWidget::~PotraceWidget()
{
    delete ui;
}
void PotraceWidget::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    paint(painter);
    painter.end();
}
void PotraceWidget::paint(QPainter &painter)
{
    painter.setClipRect(QRect(0, 0, 200, 200));
    painter.setPen(Qt::NoPen);

    QPainterPath *po = new QPainterPath();
    float w = 10000;
    float h = 10000;
//    foreach(const QxPotrace::Polygon &polygon, potrace.polygons())
//    {

//      for(int s = 0;s<polygon.boundary.count();s++)
//      {
//          w = qMin(w,(float)polygon.boundary.at(s).x());
//          h = qMin(h,(float)polygon.boundary.at(s).y());
//      }

//      foreach(const QPolygonF &hole, polygon.holes)
//      {
//          for(int n = 0;n<polygon.boundary.count();n++)
//          {
//              w = qMin(w,(float)hole.at(n).x());
//              h = qMin(h,(float)hole.at(n).y());
//          }

//      }

//    }
    foreach(const QxPotrace::Polygon &polygon, potrace.polygons())
    {
      po->addPolygon(polygon.boundary);
      foreach(const QPolygonF &hole, polygon.holes)
      {
          po->addPolygon(hole);
      }
    }
    qDebug()<<tr("wi = %1,he = %2").arg(w).arg(h);
    painter.drawPath(*po);
}

void PotraceWidget::loadImage(QString str)
{
    QString fileName = str;
//    QString fileName = QFileDialog::getOpenFileName();
//    if(fileName.isEmpty())
//      return ;
    qDebug()<<(fileName);

    QImage image(fileName);
    if(image.isNull())
      return ;


    if(!potrace.trace(image)
       || potrace.polygons().isEmpty())
      return ;
}
void PotraceWidget::outSvg()
{
//    QString newPath = QFileDialog::getSaveFileName(this, tr("Save SVG"),
//           ".", tr("SVG files (*.svg)"));

       QString newPath = "out.svg";

       QSvgGenerator generator;
       generator.setFileName(newPath);
       generator.setSize(QSize(200, 200));
       generator.setViewBox(QRect(0, 0, 200, 200));
       generator.setTitle(tr("SVG Generator Example Drawing"));
       generator.setDescription(tr("An SVG drawing created by the SVG Generator "
                                   "Provide by makeblock."));
       QPainter painter;
       painter.begin(&generator);
       paint(painter);
       painter.end();

}
void PotraceWidget::outGcode()
{
    svgs->fillPathVector("out.svg",true,false);
}
