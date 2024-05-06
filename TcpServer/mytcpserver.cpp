#include "mytcpserver.h"
#include <QDebug>

MyTcpServer::MyTcpServer() {}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "new clinet connect";
    MyTcpSocket *pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(socketDescriptor);
    m_tcpSocketList.append(pTcpSocket);

    connect(pTcpSocket, SIGNAL(offline(MyTcpSocket*)), this, SLOT(deleteSocket(MyTcpSocket*)));
}

void MyTcpServer::resend(const char *pername, PDU *pdu)
{
    if (pername == NULL || pdu == NULL)
    {
        return;
    }
    QString strName = pername;
    for (int i = 0; i < m_tcpSocketList.size(); i++)
    {
        if (m_tcpSocketList.at(i)->m_strName == strName)
        {
            m_tcpSocketList.at(i)->write((char*)pdu, pdu->uiPDULen);
            break;
        }
    }
}

void MyTcpServer::deleteSocket(MyTcpSocket *mysocket)
{
    QList<MyTcpSocket*>::Iterator iter = m_tcpSocketList.begin();
    for (; iter != m_tcpSocketList.end(); iter++)
    {
        if (mysocket == *iter)
        {
            (*iter)->deleteLater();
            *iter = NULL;
            m_tcpSocketList.erase(iter);
            break;
        }
    }

    for (int i = 0; i < m_tcpSocketList.size(); i++)
    {
        qDebug() << m_tcpSocketList.at(i)->m_strName;
    }
}
