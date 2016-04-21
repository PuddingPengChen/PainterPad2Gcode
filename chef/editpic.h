#ifndef EDITPIC_H
#define EDITPIC_H

#include <QWidget>

namespace Ui {
class editPic;
}

class editPic : public QWidget
{
    Q_OBJECT

public:
    explicit editPic(QWidget *parent = 0);
    ~editPic();

private:
    Ui::editPic *ui;
};

#endif // EDITPIC_H
