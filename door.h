#ifndef DOOR_H
#define DOOR_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>

class Door : public QObject
{
    Q_OBJECT
public:
    explicit Door(QObject *parent = 0);

signals:
    void Sig_connected(QString status);
    void Sig_progress(qint64 total,qint64 current);
    void Sig_msg(QString msg);
    void Sig_PrintNext();

public slots:
    void sendCommand(QString cmd);  //发送指令
    void sendFile(QString files);
    void connectServer(QString ip);
    void getServerMsg();
    void disconnectServer();


private:
    QTcpSocket *tcpClient;
    QFile *localFile;  //要发送的文件
    qint64 totalBytes;  //数据总大小
    qint64 bytesWritten;  //已经发送数据大小
    qint64 bytesToWrite;   //剩余数据大小
    qint64 loadSize;   //每次发送数据的大小
    qint8 sendClass ;
    QString fileName;  //保存文件路径
    QByteArray outBlock;  //数据缓冲区，即存放每次要发送的数据


    qint64 recvBytes;
    qint64 recvTotal;

    bool bConnect;
private slots:
    void send();  //连接服务器
    void startTransfer();  //发送文件
    void updateClientProgress(qint64); //发送数据，更新进度条
    void displayError(QAbstractSocket::SocketError); //显示错误
    void connectedSuccess();    //连接成功后的提示

};

#endif // DOOR_H
