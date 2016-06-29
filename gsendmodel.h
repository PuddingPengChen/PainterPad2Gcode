#ifndef GSENDMODEL_H
#define GSENDMODEL_H

#include <QFile>
#include <QObject>
#include <QTextStream>

class GSendModel : public QObject
{
    Q_OBJECT
public:
    explicit GSendModel(QObject *parent = 0);
    ~GSendModel();
//    QString ReadOneLine();
    QString readOneLine();
    void SetTargetFile(QString file);
    void reset();
    unsigned int  getTotalLayer();
    unsigned int  getTotalGcmd();
    unsigned int  getTotalTime();

public slots:
private:
    QFile* target_gcode;
    QString print_file;

    QTextStream* in;
    QString currentLine;

    unsigned int layers;
    unsigned int g_cmds;
    unsigned int totaltimes;
private slots:

};

#endif // GSENDMODEL_H
