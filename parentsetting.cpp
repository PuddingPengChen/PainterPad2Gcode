#include "parentsetting.h"
#include "ui_parentsetting.h"
#include <QSettings>
#include <QMessageBox>
#include "windows.h"
#include <QFileDialog>

ParentSetting::ParentSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParentSetting)
{
    ui->setupUi(this);
    steps = 1.0;
    //初始化打印机界面，读取设置文件中的值
    QSettings* settings = new QSettings("printerConfig.ini",QSettings::IniFormat);
    settings->beginGroup("print");
    QString speed = settings->value("speed").toString();
    QString excluder = settings->value("excluder").toString();
    QString zvalue = settings->value("zvalue").toString();
    QString rectA = settings->value("rectA").toString();
    QString rectS = settings->value("rectS").toString();
    QString flimant = settings->value("flimant").toString();
    QString widthE = settings->value("widthE").toString();
    QString temps = settings->value("temps").toString();


    ui->tempture->setText(temps);
    ui->speeding->setText(speed);
    ui->extrudingSpeed->setText(excluder);

    ui->zvalues->setText(zvalue);
    ui->rectA->setText(rectA);
    ui->rectS->setText(rectS);
    ui->flimant->setText(flimant);
    ui->widthE->setText(widthE);
    settings->endGroup();
    this->setWindowFlags(Qt::FramelessWindowHint);
}

ParentSetting::~ParentSetting()
{
    delete ui;
}

void ParentSetting::on_xUp_clicked()    //X+
{
    move("X");
}

void ParentSetting::on_xDown_clicked()  //X-
{
    move("X-");
}

void ParentSetting::on_yUp_clicked()    //Y+
{
    move("Y");
}

void ParentSetting::on_yDown_clicked()     //Y-
{
    move("Y-");
}

void ParentSetting::on_zUp_clicked()    //Z+
{
    move("Z");
}

void ParentSetting::on_zDown_clicked()      //Z-
{
    move("Z-");
}

void ParentSetting::on_eOut_clicked()       //E out
{

    move("E");
}

void ParentSetting::on_eIn_clicked()        //E back
{
    move("E-");
}

void ParentSetting::on_xHome_clicked()      //X home
{
    home("X");
}

void ParentSetting::on_yHome_clicked()      //Y home
{
    home("Y");
}
void ParentSetting::on_zHome_clicked()      //Z home
{
    home("Z");
}

void ParentSetting::on_Home_clicked()       //Home
{
    emit Sig_cmds("G28 \n");
}

void ParentSetting::on_current_z_clicked()      //current Z
{
    emit Sig_cmds("M880 \n");
}

void ParentSetting::on_stoped_clicked()     //暂停
{
    emit Sig_cmds("Stop");
}

void ParentSetting::on_recovery_clicked()       //恢复
{
    emit Sig_cmds("Recover");
}

void ParentSetting::on_cancle_clicked()         //取消
{
    emit Sig_cmds("Cancle");
}


void ParentSetting::on_printGcode_clicked()     //print gcode
{
    QString file = QFileDialog::getOpenFileName(this,tr("Open File"),"./",tr("gcode (*.gcode)"));
    emit Sig_printgcode(file);
}
void ParentSetting::on_small_pressed()      //0.1mm
{
    steps = 0.1;
}

void ParentSetting::on_middle_clicked()     //1.0mm
{
    steps = 1.0;
}

void ParentSetting::on_huge_clicked()       //10.0mm
{
    steps = 10.0;
}

void ParentSetting::move(QString axis)
{
    QString cmd = tr("G1 %1%2 F4000 \n").arg(axis).arg(steps);
    emit Sig_cmds(cmd);
}
void ParentSetting::home(QString axis)
{
    QString cmd = tr("G28 %1 \n").arg(axis);
    emit Sig_cmds(cmd);
}

void ParentSetting::on_pushButton_clicked()     //重新设置参数
{
    QSettings* settings = new QSettings("printerConfig.ini",QSettings::IniFormat);
    settings->beginGroup("print");
    QString speed = ui->speeding->text();
    QString excluder = ui->extrudingSpeed->text();
    QString zvalue = ui->zvalues->text();
    QString rectA = ui->rectA->text();
    QString rectS = ui->rectS->text();
    QString flimant = ui->flimant->text();
    QString widthE = ui->widthE->text();
    QString temps = ui->tempture->text();

    settings->setValue("speed",speed);
    settings->setValue("excluder",excluder);
    settings->setValue("zvalue",zvalue);
    settings->setValue("rectA",rectA);
    settings->setValue("rectS",rectS);
    settings->setValue("flimant",flimant);
    settings->setValue("widthE",widthE);
    settings->setValue("temps",temps);
    settings->endGroup();

    QMessageBox::information(this,tr("通知"),tr("修改参数成功！"));
}


void ParentSetting::on_back_clicked()       //back
{
     this->close();
}
void ParentSetting::slotStop()
{
     emit Sig_cmds("Stop");
}
void ParentSetting::slotCancle()
{
    emit Sig_cmds("Cancle");
}

void ParentSetting::on_pushButton_2_clicked()   //预热
{
    QSettings* settings = new QSettings("printerConfig.ini",QSettings::IniFormat);
    settings->beginGroup("print");
    int po = settings->value("temps").toInt();
    settings->endGroup();
    emit Sig_cmds(tr("T%1").arg(po));
}
