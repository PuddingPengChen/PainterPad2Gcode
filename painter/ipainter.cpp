#include "ipainter.h"
#include "ui_ipainter.h"
#include "./painter/paintarea.h"
#include <QMessageBox>
#include <QDebug>
#include "infordialog.h"

iPainter::iPainter(QWidget *parent) :
    QWidget(parent),w_icon(60),h_icon(60),
    ui(new Ui::iPainter)
{

    ui->setupUi(this);
    pen_size = 5;
    _saved = false;
    _opened = false;
    _printAble = false;
    _filename = "";

    animal = new Picture(NULL,"./src/animal/");
    tool = new Picture(NULL,"./src/tool/");
    human = new Picture(NULL,"./src/human/");
    plant = new Picture(NULL,"./src/plant/");
    katong = new Picture(NULL,"./src/katong/");

    PicGrayEdit = new PicEdit();
//    chrome = new QWebView();
//    chrome->load(QUrl("qrc:/potrace/index.html"));

    connect(animal,SIGNAL(Sig_drawPath(QString)),this,SLOT(showModel(QString)));
    connect(tool,SIGNAL(Sig_drawPath(QString)),this,SLOT(showModel(QString)));
    connect(human,SIGNAL(Sig_drawPath(QString)),this,SLOT(showModel(QString)));
    connect(plant,SIGNAL(Sig_drawPath(QString)),this,SLOT(showModel(QString)));

    connect(PicGrayEdit,SIGNAL(Sig_Print()),this,SLOT(on_paintfood_clicked()));

    dialog = new InforDialog();
    newGcode = new PotraceWidget();
    this->setWindowFlags(Qt::FramelessWindowHint);
    on_newFile_clicked();
    ui->listWidget->setIconSize(QSize(w_icon,h_icon));
    ui->listWidget->setResizeMode(QListWidget::Adjust);
//    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setMovement(QListView::Static);
    ui->listWidget->setSpacing(10);
}

iPainter::~iPainter()
{
    delete ui;
}
void iPainter::showModel(QString img)
{
    qDebug()<<"::::::"<<img;
    QImage show = changeGray(img,180);
    show.save("drink.png","PNG",100);
    ui->painter_area->openImage("drink.png");
    ui->painter_area->ReSize(841,581);
    _filename = "drink.png";
    _opened = true;
}
QImage iPainter::changeGray(QString img,int grayscale)
{
    QImage* tempImg = new QImage(img);
    QRgb *pixels = (QRgb *)tempImg->scanLine(0);

    QRgb thread = qRgb(grayscale,grayscale,grayscale);
    QRgb threadw = qRgb(180,180,180);
    QRgb threadb = qRgb(255,255,255);
    for(int y =0;y<tempImg->height();y++)
    {
        for(int x=0;x<tempImg->width();x++)
        {
            if(pixels[x+y*(tempImg->width())]<thread)
                pixels[x+y*(tempImg->width())] = threadw;
            else
                pixels[x+y*(tempImg->width())] = threadb;
        }
    }
    return *tempImg;
}
//上侧工具栏
void iPainter::on_newFile_clicked()     //新建
{
    ui->painter_area->clearImage();
    _opened = false;
    _saved = false;
    _filename = "";
    ui->painter_area->setPenColor(Qt::black);
    ui->painter_area->setBackColor(QColor(0, 0, 0, 0));
    ui->painter_area->setPenWidth(4);
    ui->painter_area->clearHistory();
    ui->painter_area->setGeometry(0, 0, ui->frame->width(), ui->frame->height());
    ui->painter_area->setGeometry(0, 0, ui->frame->width(), ui->frame->height());
    ui->painter_area->ReSize(841,581);
    ui->painter_area->setTool(1);
    QPixmap mouseCursor = QPixmap(":/img/dot.png");
    QCursor _Cur(mouseCursor, 2,2);
    ui->painter_area->setCursor(_Cur);
}
void iPainter::on_open_clicked()    //打开图片
{
    QString filename =  QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Images (*.png *.bmp *.jpg)"));
    PicGrayEdit->LoadPicture(filename);
    PicGrayEdit->show();
//    ui->painter_area->openImage(filename);
//    ui->painter_area->ReSize(841,581);
//    _filename = filename;
    _opened = true;

    //potrace javascript
//    QImage potrace(filename);
//    potrace.save("potrace.png","PNG",100);
//    QVariant po = chrome->page()->mainFrame()->evaluateJavaScript("handleFiles(\"potrace.png\");");
//    QString outs = po.toString();
//    QFile file("out.svg");
//    if (!file.open(QIODevice::WriteOnly|QIODevice::Text)) {
//        QMessageBox::critical(NULL, tr("提示"), tr("无法创建文件"));
//        return ;
//    }
//    QTextStream out(&file);
//    out<<outs;
//    file.close();

}
void iPainter::on_deleteFile_clicked()      //删除
{
    on_newFile_clicked();
}


//左侧工具栏
void iPainter::on_erase_clicked()       //橡皮擦
{
    ui->painter_area->setTool(2);
    QPixmap mouseCursor = QPixmap(":/img/erase.png");
    QCursor _Cur(mouseCursor, 5, 18);
    ui->painter_area->setCursor(_Cur);
}

void iPainter::on_blackPen_clicked()        //黑笔
{
    ui->painter_area->setTool(1);
    ui->painter_area->setPenWidth(pen_size);
    ui->painter_area->setPenColor(Qt::black);
    QPixmap mouseCursor = QPixmap(":/img/dot.png");
    QCursor _Cur(mouseCursor, 2,2);
    ui->painter_area->setCursor(_Cur);
}

void iPainter::on_redPen_clicked()      //红笔
{
    ui->painter_area->setTool(1);
    ui->painter_area->setPenWidth(pen_size);
    ui->painter_area->setPenColor(Qt::red);
    QPixmap mouseCursor = QPixmap(":/img/dot.png");
    QCursor _Cur(mouseCursor, 2,2);
    ui->painter_area->setCursor(_Cur);
}

void iPainter::on_bluePen_clicked()     //蓝笔
{
    ui->painter_area->setTool(1);
    ui->painter_area->setPenWidth(pen_size);
    ui->painter_area->setPenColor(Qt::blue);
    QPixmap mouseCursor = QPixmap(":/img/dot.png");
    QCursor _Cur(mouseCursor, 2,2);
    ui->painter_area->setCursor(_Cur);
}

void iPainter::on_greenPen_clicked()        //绿笔
{
    ui->painter_area->setTool(1);
    ui->painter_area->setPenWidth(pen_size);
    ui->painter_area->setPenColor(Qt::green);
    QPixmap mouseCursor = QPixmap(":/img/dot.png");
    QCursor _Cur(mouseCursor, 2,2);
    ui->painter_area->setCursor(_Cur);
}

void iPainter::on_self_clicked()        //自定义
{
    QColor color = QColorDialog::getColor(QColor(0, 255, 0));
    ui->painter_area->setTool(1);
    ui->painter_area->setPenWidth(pen_size);
    ui->painter_area->setPenColor(color);
    QPixmap mouseCursor = QPixmap(":/img/dot.png");
    QCursor _Cur(mouseCursor, 2,2);
    ui->painter_area->setCursor(_Cur);
}

void iPainter::on_small_clicked()       //细 笔
{
    pen_size = 3;
    ui->painter_area->setPenWidth(pen_size);
}

void iPainter::on_middle_clicked()      //中等 笔
{
    pen_size = 5;
    ui->painter_area->setPenWidth(pen_size);
}

void iPainter::on_big_clicked()         //粗 笔
{
    pen_size = 7;
    ui->painter_area->setPenWidth(pen_size);
}



void iPainter::on_save_clicked()
{
    if (_opened == true || _saved == true)
        ui->painter_area->saveImage(_filename, "png");
    else
    {
        QString filename =  QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Images (*.png)"));
        ui->painter_area->saveImage(filename, "png");
        _filename = filename;
        _saved = true;
    }
}

void iPainter::on_paintfood_clicked() //一键打印
{
    //保存图片
//    ui->painter_area->saveImage("print.png", "PNG");
    //图片转换成stl
    //图片切片
    //保存gcode
//    on_btnList_clicked();
//    if(printList.count()==0)
//    {
//        _printAble = true;
//        PrintNow();
//    }
//    else
    {
        PrintNext();
    }

}
void iPainter::on_stop_clicked()        //暂停
{
    emit Sig_stop();
}
void iPainter::on_cancle_clicked()      //取消
{
    emit Sig_cancle();
}

void iPainter::Enable_User_Waiting_Cursor()
{
    QCursor curs;
    curs.setShape(Qt::WaitCursor);
    QApplication::setOverrideCursor(curs);
}

void iPainter::Disable_User_Waiting_Cursor()
{
    QCursor curs;
    curs.setShape(Qt::ArrowCursor);
    QApplication::setOverrideCursor(curs);
}

void iPainter::on_animal_clicked()      //动物模板
{
    animal->showMaximized();
}

void iPainter::on_human_clicked()       //人物模板
{
    human->showMaximized();
}

void iPainter::on_plant_clicked()       //植物
{
    plant->showMaximized();
}

void iPainter::on_building_clicked()    //工具
{
    tool->showMaximized();
}

void iPainter::on_back_clicked()        //关闭
{
    this->close();
}


//add to list
void iPainter::on_btnList_clicked()
{
    ui->painter_area->saveImage("queue.png", "PNG");
//    ui->painter_area->saveImage(tr("%1.png").arg(printList.count()), "PNG");
//    QImage po(tr("%1.png").arg(printList.count()));
    QImage po(tr("queue.png"));
    printList.append(po);
    po = po.mirrored();
    _printAble = true;
    ui->labelCount->setText(tr("List count:%1").arg(printList.count()));
    QPixmap of = QPixmap::fromImage(po);
    QListWidgetItem *pItem = new QListWidgetItem(QIcon(of.scaled(w_icon,h_icon))," ");
    ui->listWidget->addItem(pItem);
    QMessageBox::information(this,tr("Notice"),tr("Has added to the print list!"));


}
//print next gcode if existed
void iPainter::PrintNext()
{
    ui->labelCount->setText(tr("List count:%1").arg(printList.count()));
    if(printList.count()>0)
    {
        _printAble = true;
        PrintNow();

    }
    else
    {
        _printAble = false;
    }
}
//print function
void iPainter::PrintNow()
{

    if(_printAble)
    {
        QImage printImage;
//        if(!_opened)
        {
            printImage = printList.dequeue();
            ui->listWidget->clear();

            ui->labelCount->setText(tr("List count:%1").arg(printList.count()));
            for(int i=0;i<printList.count();i++)
            {
                QPixmap of = QPixmap::fromImage(printList.at(i));
                QListWidgetItem *pItem = new QListWidgetItem(QIcon(of.scaled(w_icon,h_icon))," ");
                ui->listWidget->addItem(pItem);
            }


        }
        ui->label->setText("Processing...");
        ui->label->repaint();
        printImage.save("out.png","PNG",100);
//        fffProcessor *processor;
//        ConfigSettings config;
//        processor = new fffProcessor(config);
//        QProcess* free;     //调用外部可执行程序或shell命令的类

//        QTime time;
//        time.start();
//        Enable_User_Waiting_Cursor();
//        printImage.save("print.png","PNG",100);

//    //    if(free->execute("./load/load.exe",po))
//        QStringList pp;
//        pp.append("./python/load.py");
//        pp.append("print.png");
//        pp.append("temp.stl");
//        pp.append("invert");
//        pp.append("160");
//         if(free->execute("./python/python.exe",pp))
//        {
//            qDebug()<<"load finished!!";
//        }
//        qDebug()<<"enter 2 printfood!";
//        char*  stl;
//        char* gcode;
//        QString direct = "print.gcode";
//        QByteArray b_stl = pp.at(2).toLatin1();
//        stl=b_stl.data();

//        QByteArray b_gcode = direct.toLatin1();
//        gcode = b_gcode.data();
//        processor->setTargetFile(gcode);
//        processor->processFile(stl);
//        Disable_User_Waiting_Cursor();
//         qDebug()<<time.elapsed()/1000.0<<"s";
//        ui->label->setText("Process finsh!");
//        QMessageBox::information(this,"Notice","Image convert finish，pree OK to begin ...");
//
        Enable_User_Waiting_Cursor();
        newGcode->loadImage("out.png");
        newGcode->outSvg();
        newGcode->outGcode();
        Disable_User_Waiting_Cursor();
        QMessageBox::information(this,"Notice","Image convert finish，pree OK to begin ...");
//        ui->label->setText("Process finsh!");
        ui->label->setText("Painter");


        //发送gcode
        emit Sig_file("print.gcode");
        //发送开始打印指令
        _opened = false;
    }
}

void iPainter::on_btnRemove_clicked()
{
    printList.removeAt(ui->listWidget->currentRow());
    ui->listWidget->clear();

    ui->labelCount->setText(tr("List count:%1").arg(printList.count()));
    for(int i=0;i<printList.count();i++)
    {
        QPixmap of = QPixmap::fromImage(printList.at(i));
        QListWidgetItem *pItem = new QListWidgetItem(QIcon(of.scaled(w_icon,h_icon))," ");
        ui->listWidget->addItem(pItem);
    }

    qDebug()<<tr("current count %1").arg(printList.count());

}

//Print Current
void iPainter::on_btnPrintC_clicked()
{
    ui->label->setText("Processing...");
    ui->label->repaint();
    Enable_User_Waiting_Cursor();
    ui->painter_area->saveImage("current.png", "PNG");
    newGcode->loadImage("current.png");
    newGcode->outSvg();
    newGcode->outGcode();
    Disable_User_Waiting_Cursor();
    QMessageBox::information(this,"Notice","Image convert finish，pree OK to begin ...");
//        ui->label->setText("Process finsh!");
    ui->label->setText("Painter");


    //发送gcode
    emit Sig_file("print.gcode");
    //发送开始打印指令
    _opened = false;
    QString p = "Wi458.589";
    qDebug()<<p.right(p.length()-2);
}
