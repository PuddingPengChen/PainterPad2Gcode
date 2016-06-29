#include <QTextStream>
#include <QDebug>
#include "gsendmodel.h"

GSendModel::GSendModel(QObject *parent) :
    QObject(parent)
{
    layers=0;
    g_cmds=0;
    totaltimes=0;
    print_file = "test.gcode";
    target_gcode = new QFile("test.gcode");
    in =new  QTextStream(target_gcode);
}

GSendModel::~GSendModel()
{
    target_gcode->close();
}
void GSendModel::SetTargetFile(QString file)
{
    layers=0;
    g_cmds=0;
    totaltimes=0;
    print_file = file;
    target_gcode->close();
    target_gcode->setFileName(print_file);
    if(!target_gcode->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the g-code file!"<<print_file;
    }
    in->setDevice(target_gcode);
    quint64 pos = in->device()->pos();
    QRegExp regLt(QString(";Layer count: *([0-9]*)"));
    QRegExp regL(QString(";LAYER: *([0-9]*)"));
    QRegExp regT(QString(";TIME: *([0-9]*)"));
    while(!(in->atEnd()))
    {
        QString yi = in->readLine();
        if(yi.startsWith("G"))
        {
            g_cmds++;
        }
        if(yi.contains(regLt))
        {
            if((yi.indexOf(regLt))>=0)
            {
                qDebug()<<"total layers is :"<<regLt.cap(1);
            }
        }
        if(yi.contains(regT))
        {
            if((yi.indexOf(regT)))
            {
                qDebug()<<"total time is :"<<regT.cap(1);
            }
        }
    }
    in->device()->seek(pos);
    qDebug()<<tr("total comands : %1").arg(g_cmds);
}
unsigned int GSendModel::getTotalLayer()
{
    return layers;
}
unsigned int GSendModel::getTotalGcmd()
{
    return g_cmds;
}
unsigned int GSendModel::getTotalTime()
{
    return totaltimes;
}
QString GSendModel::readOneLine()
{
    QString pi = in->readLine()+"\n";
    bool goon = true;
    //QRegExp reg(QString("([a-zA-Z0-9.\s]*) *;"));
    QRegExp reg(QString("(.*) *;"));
    if(in->atEnd())
    {
        currentLine = "end";
        return currentLine;
    }
    while(goon&&(!in->atEnd()))
    {
        if((pi[0]==';')||(pi[0]=='\n'))
        {
           pi = in->readLine()+"\n";
        }
        else if(pi.startsWith("M109"))
        {
            pi = in->readLine()+"\n";
        }
        else
        {
            if(pi.contains(reg))
            {
                int pos = pi.indexOf(reg);
                if(pos>=0)
                currentLine = reg.cap(1)+"\n";
            }
            else
            {
                currentLine = pi;
            }
           goon = false;
        }
    }
    qDebug()<<"read gcode:"<<currentLine;
    return currentLine;
}
void GSendModel::reset()
{
    currentLine = "";
    layers = 0;
    totaltimes = 0;
    g_cmds = 0;
    target_gcode->close();
}

