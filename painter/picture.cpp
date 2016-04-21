#include "picture.h"
#include "ui_picture.h"
#include <QDir>
#include <QDebug>


Picture::Picture(QWidget *parent,QString dir) :
    QWidget(parent),
    ui(new Ui::Picture)
{
    ui->setupUi(this);
    currentPage=1;
    currentPic=0;
    ui->listWidget->setIconSize(QSize(W_ICONSIZE,H_ICONSIZE));
    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setMovement(QListView::Static);
    ui->listWidget->setSpacing(50);
    ui->listWidget->setFocusPolicy(Qt::NoFocus);
    dirP = dir;
    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(Slot_ItemClicked(QListWidgetItem*)));
    reUpdate();
}

Picture::~Picture()
{
    delete ui;
}
void Picture::loadImages(QString dir)
{
    QDir dirs(dir);
    QStringList infloList = dirs.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
    totalPic = infloList.size();
    ui->listWidget->clear();
    qDebug()<<tr("total pic: %1").arg(totalPic);
    int showCount;
    if(currentPage==(totalPic/12))
    {
        if((totalPic%12)!=0)
            showCount = totalPic%12;
        else
             showCount = 12;
    }
    else
    {
        showCount = 12;
    }
    for(int i=0;i<showCount;i++)
    {
        QString strPath = dir+QString::number((currentPage-1)*12+(i+1))+".png";
        QPixmap objPixmap(strPath);
        qDebug()<<tr("src: %1").arg(strPath);
        QListWidgetItem* pItem = new QListWidgetItem(QIcon(objPixmap.scaled(QSize(W_ICONSIZE,H_ICONSIZE)))," ");
        pItem->setSizeHint(QSize(W_ICONSIZE,H_ICONSIZE));

        ui->listWidget->insertItem(i,pItem);
    }
    ui->label->setText(tr("第%1页").arg(currentPage));
}

//槽函数
void Picture::Slot_ItemClicked(QListWidgetItem * item)
{
    if(!item)
    {
        return;
    }
    int index = ui->listWidget->row(item);
    currentPic = index+1;
    qDebug()<<tr("Now you click the %1").arg(index+1);

}

void Picture::on_pre_clicked()          //下一页
{
    if(currentPage>=(totalPic/12))
    {
        currentPage = (totalPic/12);
    }
    else
    {
        currentPage++;
    }
    currentPic=0;
    reUpdate();

}


void Picture::on_next_clicked() //上一页
{
    if(currentPage<=1)
    {
        currentPic=1;
    }
    else
    currentPage--;
     reUpdate();
}
void Picture:: reUpdate()
{
    loadImages(dirP);
}

void Picture::on_pushButton_2_clicked()     //退出
{
    this->close();
}

void Picture::on_pushButton_clicked()       //画简笔画
{
    QString path = dirP+QString::number(currentPic+(currentPage-1)*12)+".png";
    emit Sig_drawPath(path);
    this->close();
}
