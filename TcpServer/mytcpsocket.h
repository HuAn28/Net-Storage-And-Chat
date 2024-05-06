#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include <QFile>
#include <QTimer>

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    void copyDir(QString strSrcDir, QString strDestDir);

signals:
    void offline(MyTcpSocket * mysocket);

public slots:
    void recvMsg();
    void clientOffline();
    void sendFileToClient();

// private:
public:
    QString m_strName;

    QFile m_file;
    qint64 m_iTotal;
    qint64 m_iRecved;
    bool m_bUpload;

    QTimer * m_pTimer;
};

#endif // MYTCPSOCKET_H
