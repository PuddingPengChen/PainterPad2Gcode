#include "svgpath.h"
#include <math.h>
#include <QSettings>

svgPath::svgPath(QObject *parent) :
    QObject(parent)
{
    QSettings* psetting = new QSettings("Config.ini",QSettings::IniFormat);
    psetting->beginGroup("laser");
   int slidervalue =  psetting->value("laserPower").toInt();
   int laserspeed =  psetting->value("laserSpeed").toInt();
   int picWidth = psetting->value("printWidth").toInt();
    psetting->endGroup();
    QString power = "M4 P"+QString::number(slidervalue)+"\n";
    setLaster(power);
    QString speed = "G90\nM4 P0\nG0 F9000\nM220 S"+QString::number(laserspeed)+"\n";
//    QString speed = "G90\nG21\nM4 P0\n";
    setHeader(speed);
    setPrintPicWidth(picWidth);
}

void  svgPath::fillPathVector(QString file,bool doBez,bool exportFile)
{
    svgToGcode(file.toLatin1().data(),1);
}
void svgPath::setLaster(QString pon)
{
    setPrintLaster(pon.toLatin1().data());
}
void svgPath::setHeader(QString pheader)
{
    setPrintHead(pheader.toLatin1().data());
}
void svgPath::setPrintPicWidth(int i)
{
    setPrintWidth(i);
}

//parse the gcode
//store the gcode in the vector
