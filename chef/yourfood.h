#ifndef YOURFOOD_H
#define YOURFOOD_H

#include <QWidget>

namespace Ui {
class yourfood;
}

class yourfood : public QWidget
{
    Q_OBJECT

public:
    explicit yourfood(QWidget *parent = 0);
    ~yourfood();

private:
    Ui::yourfood *ui;
};

#endif // YOURFOOD_H
