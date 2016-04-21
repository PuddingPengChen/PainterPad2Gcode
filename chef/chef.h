#ifndef CHEF_H
#define CHEF_H

#include <QWidget>

namespace Ui {
class chef;
}

class chef : public QWidget
{
    Q_OBJECT

public:
    explicit chef(QWidget *parent = 0);
    ~chef();

private:
    Ui::chef *ui;
};

#endif // CHEF_H
