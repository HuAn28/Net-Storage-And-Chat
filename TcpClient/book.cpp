#include "book.h"
#include "opewidget.h"
#include "tcpclient.h"
#include "sharefile.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

Book::Book(QWidget *parent) : QWidget{parent}
{
    m_strEnterDir.clear();
    m_bDownload = false;
    m_pTimer = new QTimer;

    m_pBookListW = new QListWidget;
    m_pReturnPB = new QPushButton("返回");
    m_pCreateDirPB = new QPushButton("创建文件夹");
    m_pDelDirPB = new QPushButton("删除文件夹");
    m_pRenamePB = new QPushButton("重命名文件夹");
    m_pFlushFilePB = new QPushButton("刷新文件");

    QVBoxLayout * pDirVBL = new QVBoxLayout;
    pDirVBL->addWidget(m_pReturnPB);
    pDirVBL->addWidget(m_pCreateDirPB);
    pDirVBL->addWidget(m_pDelDirPB);
    pDirVBL->addWidget(m_pRenamePB);
    pDirVBL->addWidget(m_pFlushFilePB);

    m_pUploadPB = new QPushButton("上传文件");
    m_pDownLoadPB = new QPushButton("下载文件");
    m_pDelFilePB = new QPushButton("删除文件");
    m_pShareFilePB = new QPushButton("共享文件");
    m_pMoveFilePB = new QPushButton("移动文件");
    m_pSelectDirPB = new QPushButton("目标目录");
    m_pSelectDirPB->setEnabled(false);

    QVBoxLayout * pFileVBL = new QVBoxLayout;
    pFileVBL->addWidget(m_pUploadPB);
    pFileVBL->addWidget(m_pDownLoadPB);
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pShareFilePB);
    pFileVBL->addWidget(m_pMoveFilePB);
    pFileVBL->addWidget(m_pSelectDirPB);

    QHBoxLayout * pMain = new QHBoxLayout;
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);

    connect(m_pCreateDirPB, SIGNAL(clicked(bool)), this, SLOT(createDir()));
    connect(m_pFlushFilePB, SIGNAL(clicked(bool)), this, SLOT(flushFile()));
    connect(m_pDelDirPB, SIGNAL(clicked(bool)), this, SLOT(delDir()));
    connect(m_pRenamePB, SIGNAL(clicked(bool)), this, SLOT(renameFile()));
    connect(m_pBookListW, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(enterDir(QModelIndex)));
    connect(m_pReturnPB, SIGNAL(clicked(bool)), this, SLOT(returnPre()));
    connect(m_pUploadPB, SIGNAL(clicked(bool)), this, SLOT(uploadFile()));
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(uploadFileData()));
    connect(m_pDelFilePB, SIGNAL(clicked(bool)), this, SLOT(delRegFile()));
    connect(m_pDownLoadPB, SIGNAL(clicked(bool)), this, SLOT(downloadFile()));
    connect(m_pShareFilePB, SIGNAL(clicked(bool)), this, SLOT(shareFile()));
    connect(m_pMoveFilePB, SIGNAL(clicked(bool)), this, SLOT(moveFile()));
    connect(m_pSelectDirPB, SIGNAL(clicked(bool)), this, SLOT(selectDestDir()));
}

void Book::updateFileList(const PDU *pdu)
{
    if (pdu == NULL)
    {
        return;
    }

    QListWidgetItem * pItemTmp = NULL;
    int row = m_pBookListW->count();
    while (m_pBookListW->count() > 0)
    {
        pItemTmp = m_pBookListW->item(row - 1);
        m_pBookListW->removeItemWidget(pItemTmp);
        delete pItemTmp;
        row = row - 1;
    }

    FileInfo * pFileInfo = NULL;
    int iCount = pdu->uiMsgLen / sizeof(FileInfo);
    for (int i = 0; i < iCount; i++)
    {
        pFileInfo = (FileInfo*)(pdu->caMsg) + i;
        QListWidgetItem * pItem = new QListWidgetItem;
        if (pFileInfo->iFileType == 0)
        {
            pItem->setIcon(QIcon(QPixmap(":/map/dir.jpg")));
        }
        else if (pFileInfo->iFileType == 1)
        {
            pItem->setIcon(QIcon(QPixmap(":/map/reg.jpg")));
        }
        pItem->setText(pFileInfo->caFileName);
        m_pBookListW->addItem(pItem);
    }
}

void Book::clearEnterDir()
{
    m_strEnterDir.clear();
}

QString Book::getEnterDir()
{
    return m_strEnterDir;
}

void Book::setDownloadStatus(bool status)
{
    m_bDownload = status;
}

bool Book::getDownloadStatus()
{
    return m_bDownload;
}

QString Book::getSaveFilePath()
{
    return m_strSaveFilePath;
}

QString Book::getShareFileName()
{
    return m_strShareFileName;
}

void Book::createDir()
{
    QString strNewDir = QInputDialog::getText(this, "新建文件夹", "新文件夹名字");
    if (strNewDir.isEmpty())
    {
        QMessageBox::warning(this, "新建文件夹", "新文件夹名字不能为空");
        return;
    }
    if (strNewDir.size() > 31)
    {
        QMessageBox::warning(this, "新建文件夹", "文件夹名字不能超过32个字符");
        return;
    }
    QString strName = TcpClient::getInstance().getLoginName();
    QString strCurPath = TcpClient::getInstance().getCurPath();
    PDU * pdu = mkPDU(strCurPath.toUtf8().size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
    strncpy(pdu->caData, strName.toStdString().c_str(), strName.toUtf8().size());
    strncpy(pdu->caData + 32, strNewDir.toStdString().c_str(), strNewDir.toUtf8().size());
    memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.toUtf8().size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::flushFile()
{
    QString strCurPath = TcpClient::getInstance().getCurPath();
    PDU * pdu = mkPDU(strCurPath.toUtf8().size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    strncpy((char*)(pdu->caMsg), strCurPath.toStdString().c_str(), strCurPath.toUtf8().size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::delDir()
{
    QString strCurPath = TcpClient::getInstance().getCurPath();
    QListWidgetItem * pItem = m_pBookListW->currentItem();
    if (pItem == NULL)
    {
        QMessageBox::warning(this, "删除文件夹", "请选择要删除的文件夹");
        return;
    }
    QString strDelName = pItem->text();
    PDU * pdu = mkPDU(strCurPath.toUtf8().size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_REQUEST;
    strncpy(pdu->caData,strDelName.toStdString().c_str(), strDelName.toUtf8().size());
    memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.toUtf8().size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::renameFile()
{
    QString strCurPath = TcpClient::getInstance().getCurPath();
    QListWidgetItem * pItem = m_pBookListW->currentItem();
    if (pItem == NULL)
    {
        QMessageBox::warning(this, "重命名文件", "请选择要重命名的文件");
        return;
    }
    QString strOldName = pItem->text();
    QString strNewName = QInputDialog::getText(this, "重命名文件", "请输入新的文件名");
    if (strNewName.isEmpty())
    {
        QMessageBox::warning(this, "重命名文件", "新文件名不能为空");
        return;
    }
    PDU * pdu = mkPDU(strCurPath.toUtf8().size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
    strncpy(pdu->caData, strOldName.toStdString().c_str(), strOldName.toUtf8().size());
    strncpy(pdu->caData + 32, strNewName.toStdString().c_str(), strNewName.toUtf8().size());
    memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.toUtf8().size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::enterDir(const QModelIndex &index)
{
    QString strDirName = index.data().toString();
    m_strEnterDir = strDirName;
    QString strCurPath = TcpClient::getInstance().getCurPath();
    PDU * pdu = mkPDU(strCurPath.toUtf8().size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_REQUEST;
    strncpy(pdu->caData, strDirName.toStdString().c_str(), strDirName.toUtf8().size());     // 要进入的目录
    memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.toUtf8().size());       // 当前目录
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::returnPre()
{
    QString strCurPath = TcpClient::getInstance().getCurPath();
    QString strRootPath = "./" + TcpClient::getInstance().getLoginName();
    if (strCurPath == strRootPath)
    {
        QMessageBox::warning(this, "返回", "返回失败: 已经在最开始的文件夹目录中");
        return;
    }
    int index = strCurPath.lastIndexOf('/');
    strCurPath.remove(index, strCurPath.size() - index);
    TcpClient::getInstance().setCurPath(strCurPath);
    flushFile();
}

void Book::uploadFile()
{
    QString strCurPath = TcpClient::getInstance().getCurPath();     // strCurPath:当前路径
    m_strUploadFilePath = QFileDialog::getOpenFileName();           // m_strUploadFilePath:要上传的文件路径
    if (m_strUploadFilePath.isEmpty())
    {
        QMessageBox::warning(this, "上传文件", "上传文件名字不能为空");
        return;
    }
    int index = m_strUploadFilePath.lastIndexOf('/');
    QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.size() - index - 1);
    QFile file(m_strUploadFilePath);
    qint64 fileSize = file.size();                                  // 获得文件大小
    PDU * pdu = mkPDU(strCurPath.toUtf8().size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
    memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.toUtf8().size());
    sprintf(pdu->caData, "%s %lld", strFileName.toStdString().c_str(), fileSize);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;

    m_pTimer->start(1000);
}

void Book::uploadFileData()
{
    m_pTimer->stop();
    QFile file(m_strUploadFilePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "上传文件", "打开文件失败");
        return;
    }
    char * pBuffer = new char[4096];
    qint64 ret = 0;
    while (true)
    {
        ret = file.read(pBuffer, 4096);
        if (ret > 0 && ret <= 4096)
        {
            TcpClient::getInstance().getTcpSocket().write(pBuffer, ret);
        }
        else if (ret == 0)
        {
            break;
        }
        else
        {
            QMessageBox::warning(this, "上传文件", "上传文件失败: 读文件失败");
            break;
        }
    }
    file.close();
    delete [] pBuffer;
    pBuffer = NULL;
}

void Book::delRegFile()
{
    QString strCurPath = TcpClient::getInstance().getCurPath();
    QListWidgetItem * pItem = m_pBookListW->currentItem();
    if (pItem == NULL)
    {
        QMessageBox::warning(this, "删除文件", "请选择要删除的文件");
        return;
    }
    QString strDelName = pItem->text();
    PDU * pdu = mkPDU(strCurPath.toUtf8().size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_REQUEST;
    strncpy(pdu->caData,strDelName.toStdString().c_str(), strDelName.toUtf8().size());
    memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.toUtf8().size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::downloadFile()
{
    QString strCurPath = TcpClient::getInstance().getCurPath();
    QListWidgetItem * pItem = m_pBookListW->currentItem();
    if (pItem == NULL)
    {
        QMessageBox::warning(this, "下载文件", "请选择要下载的文件");
    }
    else
    {
        QString strSaveFilePath = QFileDialog::getSaveFileName();
        if (strSaveFilePath.isEmpty())
        {
            QMessageBox::warning(this, "下载文件", "请指定要保存的位置");
            m_strSaveFilePath.clear();
        }
        else
        {
            m_strSaveFilePath = strSaveFilePath;
        }

        QString strCurPath = TcpClient::getInstance().getCurPath();
        PDU * pdu = mkPDU(strCurPath.toUtf8().size() + 1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
        QString strFileName = pItem->text();
        strcpy(pdu->caData, strFileName.toStdString().c_str());
        memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.toUtf8().size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    }
}

void Book::shareFile()
{
    QListWidgetItem * pItem = m_pBookListW->currentItem();
    if (pItem == NULL)
    {
        QMessageBox::warning(this, "下载文件", "请选择要共享的文件");
        return;
    }
    else
    {
        m_strShareFileName = pItem->text();
    }

    Friend * pFriend = OpeWidget::getInstance().getFriend();
    QListWidget * pFriendList = pFriend->getFriendList();
    ShareFile::getInstance().updateFriend(pFriendList);
    if (ShareFile::getInstance().isHidden())
    {
        ShareFile::getInstance().show();
    }
}

void Book::moveFile()
{
    QListWidgetItem * pCurItem = m_pBookListW->currentItem();
    if (pCurItem == NULL)
    {
        QMessageBox::warning(this, "移动文件", "请选择要移动的文件");
        return;
    }
    m_strMoveFileName = pCurItem->text();
    QString strCurPath = TcpClient::getInstance().getCurPath();
    m_strMoveFilePath = strCurPath + '/' + m_strMoveFileName;
    m_pSelectDirPB->setEnabled(true);
}

void Book::selectDestDir()
{
    QListWidgetItem * pCurItem = m_pBookListW->currentItem();
    if (pCurItem == NULL)
    {
        QMessageBox::warning(this, "移动文件", "请选择要移动的文件");
        return;
    }
    m_strMoveFileName = pCurItem->text();
    QString strCurPath = TcpClient::getInstance().getCurPath();
    m_strDestDir = strCurPath + '/' + m_strMoveFileName;
    m_pSelectDirPB->setEnabled(false);

    int srcLen = m_strMoveFilePath.toUtf8().size();
    int destLen = m_strDestDir.toUtf8().size();
    PDU * pdu = mkPDU(srcLen + destLen + 2);
    pdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
    sprintf(pdu->caData, "%d %d %s", srcLen, destLen, m_strMoveFileName.toStdString().c_str());

    memcpy(pdu->caMsg, m_strMoveFilePath.toStdString().c_str(), srcLen);
    memcpy((char*)(pdu->caMsg) + srcLen + 1, m_strDestDir.toStdString().c_str(), destLen);

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}
