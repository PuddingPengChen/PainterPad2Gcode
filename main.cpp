#include "welcom.h"
#include <QApplication>
#include "./keyboard/frmnum.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qApp->setFont(QFont("Microsoft YaHei", 10));
    //qApp->setFont(QFont("WenQuanYi Micro Hei", 15));

    //在main函数中就加载输入法面板,保证整个应用程序可以使用输入法
    //以下方法打开中文输入法
    frmNum::Instance()->Init("lightgray",10);
    Welcom w;
    w.showMaximized();

    return a.exec();
}
