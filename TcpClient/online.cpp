#include "online.h"
#include "ui_online.h"
#include "tcpclient.h"

Online::Online(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

void Online::showUsr(PDU *pdu)
{
    if (NULL == pdu)
    {
        return;
    }
    uint uiSize = pdu->uiMsgLen / 32;
    char caTmp[32];
    ui->online_lw->clear();
    for (uint i = 0; i < uiSize; i++)
    {
        memcpy(caTmp, (char*)(pdu->caMsg) + i * 32, 32);
        ui->online_lw->addItem(caTmp);
    }
}

void Online::on_addFriend_pd_clicked()
{
    QListWidgetItem * pItem = ui->online_lw->currentItem();
    if (pItem == NULL)
    {
        return;
    }
    QString strPerUsrName = pItem->text();
    QString strLoginName = TcpClient::getInstance().getLoginName();
    PDU * pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData, strPerUsrName.toStdString().c_str(), strPerUsrName.toUtf8().size());
    memcpy(pdu->caData + 32, strLoginName.toStdString().c_str(), strLoginName.toUtf8().size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

