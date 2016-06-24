#include "gmodel.h"

#include <QFile>
#include <QTextStream>
#include <QVarLengthArray>

#include <QtOpenGL>
#include <QDebug>
#include <QRegExp>
#include <QObject>


GModel::GModel()
{
}
//for render the picture
void GModel::loadFile(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return;

    //如下两个变量存放模型的边界
    QVector3D boundsMin( 1e9, 1e9, 1e9);
    QVector3D boundsMax(-1e9,-1e9,-1e9);


    //读取obj格式的文件
    QTextStream in(&file);
    //now begin to read the verters

    QVector3D pi;
    float currentZ = 0.0;
    float currentX = 0.0;
    float currentY = 0.0;
    QRegExp regZ("[\w\d\s]*Z([0-9\.]*[0-9]*)");
    QRegExp regL(";LAYER: *([0-9]*)");
    QRegExp regXY("G1 [\w\d\s]* *X([0-9\.]*[0-9]*) *Y([0-9\.]*[0-9]*)");

    while(!in.atEnd())
    {
        QString current_str = in.readLine();
        partIsland t_island;
        while(!current_str.startsWith("G0"))
        {
            if(current_str.contains(regXY))
            {
                currentX = regXY.cap(1).toFloat();
                currentY = regXY.cap(2).toFloat();
                boundsMin.setX( qMin(boundsMin.x(),currentX));
                boundsMax.setX( qMax(boundsMax.x(), currentX));
                boundsMin.setY(qMin(boundsMin.y(),currentY));
                boundsMax.setY(qMax(boundsMax.y(), currentY));
                boundsMin.setZ(qMin(boundsMin.z(),currentZ));
                boundsMax.setZ(qMax(boundsMax.z(), currentZ));
                qDebug()<< QObject::tr("Xs=%1").arg(currentX);
                pi = QVector3D(currentX,currentY,currentZ);
                m_points.push_back(pi);
                t_island.island.append(pi);
                current_str = in.readLine();
            }
            else
            {
                break;
            }
        }
        if(t_island.island.count()>1)
        {
            m_parts.append(t_island);
        }
    }
    bounds = boundsMax - boundsMin;
    outMin = boundsMin;
    scale = 100 / qMax(bounds.x(), qMax(bounds.y(), bounds.z()));

    for(int j=0;j<m_points.size();j++)
    {
        m_points[j] = (m_points[j] - (boundsMin + bounds * 0.5)) * scale;
        qDebug()<< QObject::tr("X=%1").arg(m_points[j].x());
    }
}

//for filter the model data
void GModel::filterGcode(QString filePath,QPointF pos,QPointF scope,QString outfiles)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return;
    qDebug()<<"in the filter";
    //如下两个变量存放模型的边界
    QVector3D boundsMin( 99999, 99999, 99999);
    QVector3D boundsMax(-99999,-99999,-99999);
    QVector3D _bounds(0,0,0);
    QVector<QVector3D> _points;
    double _scale = 1.0;

    QFile outfile(outfiles);
    if(!outfile.open(QIODevice::ReadWrite))
        return ;
    QTextStream out(&outfile);
    QTextStream in(&file);
    //now begin to read the verters

    QVector3D pi;
    float currentZ = 0.0;
    float currentX = 0.0;
    float currentY = 0.0;
    QRegExp regZ("[\w\d\s]*Z([0-9\.]*[0-9]*)");
    QRegExp regL(";LAYER: *([0-9]*)");
    QRegExp regXY("G1 [\w\d\s]* *X([0-9\.]*[0-9]*) *Y([0-9\.]*[0-9]*)");
    QRegExp reg0XY("G0 [\w\d\s]* *X([0-9\.]*[0-9]*) *Y([0-9\.]*[0-9]*)");

    //get the bounding of the gcode
    while(!in.atEnd())
    {
        QString current_str = in.readLine();
        if(current_str.contains(regXY))
        {
            currentX = regXY.cap(1).toFloat();
            currentY = regXY.cap(2).toFloat();
            boundsMin.setX( qMin(boundsMin.x(),currentX));
            boundsMax.setX( qMax(boundsMax.x(), currentX));
            boundsMin.setY(qMin(boundsMin.y(),currentY));
            boundsMax.setY(qMax(boundsMax.y(), currentY));
        }
        else if(current_str.contains(reg0XY))
        {

            currentX = reg0XY.cap(1).toFloat();
            currentY = reg0XY.cap(2).toFloat();
            boundsMin.setX( qMin(boundsMin.x(),currentX));
            boundsMax.setX( qMax(boundsMax.x(), currentX));
            boundsMin.setY(qMin(boundsMin.y(),currentY));
            boundsMax.setY(qMax(boundsMax.y(), currentY));

        }

    }
    _bounds = boundsMax - boundsMin;
    _scale = qMax(scope.x(),scope.y())/ qMax(_bounds.x(), qMax(_bounds.y(), _bounds.z()));
    qDebug()<<QObject::tr("_bound x=%1 y=%2").arg(_bounds.x()).arg(_bounds.y());
    //rewrite the gcode ,and save the gcode file
    in.seek(0);
    bool write_speed = false;
    while(!in.atEnd())
    {
        QString current_str = in.readLine();
        //        qDebug()<<(current_str);
        if(current_str.startsWith("M4"))
        {
            out<<current_str<<"\n";
        }
        else if(current_str.contains(reg0XY))
        {
            currentX = reg0XY.cap(1).toFloat();
            currentY = reg0XY.cap(2).toFloat();
            QVector3D sv(currentX,currentY,currentZ);
            sv = (sv - boundsMin)*_scale;
            QString str = QObject::tr("G0 X%1 Y%2 F3000 \n ").arg(sv.x()+pos.x()).arg(sv.y()+pos.y());
            out<<str;
            write_speed = true;
            qDebug()<<str;
        }
        else if(current_str.contains(regXY))
        {
            currentX = regXY.cap(1).toFloat();
            currentY = regXY.cap(2).toFloat();
            QVector3D su(currentX,currentY,currentZ);
            su = (su - boundsMin)*_scale;
            if(write_speed)
            {
                write_speed = false;
                out<<QObject::tr("G1 X%1 Y%2 F2000 \n").arg(su.x()+pos.x()).arg(su.y()+pos.y());
            }
            else
            {
                out<<QObject::tr("G1 X%1 Y%2 \n").arg(su.x()+pos.x()).arg(su.y()+pos.y());
                qDebug()<<QObject::tr("G1 X%1 Y%2 \n").arg(su.x()).arg(su.y());
            }
        }
        else
        {
            out<<current_str<<"\n";
        }
    }

    file.close();
    outfile.close();
}

void GModel::resetData()
{
    m_parts.clear();
}
