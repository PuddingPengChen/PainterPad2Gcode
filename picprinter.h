#ifndef PICPRINTER_H
#define PICPRINTER_H

#include <QThread>

class PicPrinter : public QThread
{
    Q_OBJECT
public:
    explicit PicPrinter(QObject *parent = 0);

signals:

public slots:

private:


};

#endif // PICPRINTER_H
