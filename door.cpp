#include "door.h"
#include <QDebug>
#include <QHostAddress>

Door::Door(QObject *parent) :
    QObject(parent)
{
    loadSize = 4*1024;
    totalBytes = 0;
    bytesWritten = 0;
    bytesToWrite = 0;
    sendClass = 0;
    tcpClient = new QTcpSocket(this);

    recvBytes = 0;
    recvTotal = 0;
    bConnect = false;
    //当连接服务器成功时，发出connected()信号，开始传送文件
    connect(tcpClient,SIGNAL(connected()),this,SLOT(connectedSuccess()));
    //当有数据发送成功时，我们更新进度条
    connect(tcpClient,SIGNAL(bytesWritten(qint64)),this,SLOT(updateClientProgress(qint64)));
    //出错处理
    connect(tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
    //接收数据
    connect(tcpClient,SIGNAL(readyRead()),this,SLOT(getServerMsg()));
}

void Door::send()   //连接到服务器，执行发送
{

}
void Door::connectedSuccess()
{
    qDebug()<<tr("连接成功！");
    emit Sig_connected("Suc");
    bConnect = true;
}
void Door::startTransfer()  //实现文件大小等信息的发送
{
    localFile = new QFile(fileName);
    if(!localFile->open(QFile::ReadOnly))
    {
        qDebug() << "open file error!";
        return;
    }
    totalBytes = localFile->size();
    qDebug()<<"file size="<<totalBytes<<";head size = "<<(sizeof(quint8)+sizeof(quint64));
    //文件总大小
    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_8);


    //这里的总大小是文件名大小等信息和实际文件大小的总和
    totalBytes += (sizeof(qint8)+sizeof(qint64));

    sendOut.device()->seek(0);

    //totalBytes是文件总大小  20 是文件类型
    sendOut<<totalBytes<<qint8(20);

    qDebug()<<"totalbytes="<<totalBytes;
    //发送完头数据后剩余数据的大小，即文件实际内容的大小
    bytesToWrite = totalBytes - tcpClient->write(outBlock);
    sendClass = 20;
    outBlock.resize(0);
}

void Door::updateClientProgress(qint64 numBytes) //更新进度条，实现文件的传送
{
    if(sendClass==20)
    {
        bytesWritten += (int)numBytes;
        //已经发送数据的大小
        if(bytesToWrite > 0) //如果已经发送了数据
        {
            //每次发送loadSize大小的数据，这里设置为4KB，如果剩余的数据不足4KB，
            //就发送剩余数据的大小
            outBlock = localFile->read(qMin(bytesToWrite,loadSize));

            //发送完一次数据后还剩余数据的大小
            bytesToWrite -= (int)tcpClient->write(outBlock);

            outBlock.resize(0);
            //清空发送缓冲区
        }
        else
        {
            localFile->close(); //如果没有发送任何数据，则关闭文件
        }
        //更新进度条
//        ui->clientProgressBar->setMaximum(totalBytes);
//        ui->clientProgressBar->setValue(bytesWritten);
        if(bytesWritten == totalBytes) //发送完毕
        {
//            ui->clientStatusLabel->setText(tr("传送文件 %1 成功").arg(fileName));
            localFile->close();
            qDebug()<<"write finish!";
//            ui->clientProgressBar->reset();

            totalBytes = 0;
            bytesWritten = 0;
            bytesToWrite = 0;
            sendClass = 0;
        }
    }

}

void Door::sendCommand(QString cmd)       //发送指令
{
    totalBytes = cmd.toLatin1().size();
    //文件总大小
    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_8);

    //这里的总大小是文件名大小等信息和实际文件大小的总和
    totalBytes += (sizeof(qint8)+sizeof(qint64));

    sendOut.device()->seek(0);

    //totalBytes是文件总大小  10 是类型
    sendOut<<totalBytes<<qint8(10)<<cmd;

    //发送完头数据后剩余数据的大小，即文件实际内容的大小
    tcpClient->write(outBlock);

    totalBytes = 0;
    bytesWritten = 0;
    outBlock.resize(0);
}
void Door::sendFile(QString files)
{
    fileName = files;
    startTransfer();
}


void Door::connectServer(QString ips)
{
    bytesWritten = 0;
    tcpClient->connectToHost(QHostAddress(ips),6666);//连接
}
void Door::disconnectServer()
{
    bConnect = false;
    tcpClient->disconnect();
}
void Door::getServerMsg()
{
    QString po = tcpClient->readAll();
    qDebug()<<"from server:"<<po;
    if(po=="Finish")
    {
        emit Sig_PrintNext();
    }
}
void Door::displayError(QAbstractSocket::SocketError) //显示错误
{
    qDebug() << tcpClient->errorString();
    tcpClient->close();

}

