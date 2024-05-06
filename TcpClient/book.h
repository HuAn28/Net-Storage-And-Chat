#ifndef BOOK_H
#define BOOK_H

#include "protocol.h"
#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>

class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void updateFileList(const PDU *pdu);
    void clearEnterDir();
    QString getEnterDir();
    void setDownloadStatus(bool status);
    bool getDownloadStatus();
    QString getSaveFilePath();
    QString getShareFileName();

signals:

public slots:
    void createDir();
    void flushFile();
    void delDir();
    void renameFile();
    void enterDir(const QModelIndex &index);
    void returnPre();
    void uploadFile();
    void uploadFileData();
    void delRegFile();
    void downloadFile();
    void shareFile();
    void moveFile();
    void selectDestDir();

public:
    qint64 m_iTotal;    // 总的文件大小
    qint64 m_iRecved;   // 已收到多少

private:
    QListWidget * m_pBookListW;
    QPushButton * m_pReturnPB;
    QPushButton * m_pCreateDirPB;
    QPushButton * m_pDelDirPB;
    QPushButton * m_pRenamePB;
    QPushButton * m_pFlushFilePB;
    QPushButton * m_pUploadPB;
    QPushButton * m_pDownLoadPB;
    QPushButton * m_pDelFilePB;
    QPushButton * m_pShareFilePB;
    QPushButton * m_pMoveFilePB;
    QPushButton * m_pSelectDirPB;

    QString m_strEnterDir;
    QString m_strUploadFilePath;
    QString m_strSaveFilePath;
    QString m_strShareFileName;
    QString m_strMoveFileName;
    QString m_strMoveFilePath;
    QString m_strDestDir;

    QTimer * m_pTimer;

    bool m_bDownload;
};

#endif // BOOK_H
