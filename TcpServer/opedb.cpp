#include "opedb.h"
#include <QMessageBox>

OpeDB::OpeDB()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

OpeDB::~OpeDB()
{
    m_db.close();
}

OpeDB& OpeDB::getInstance()
{
    static OpeDB instance;
    return instance;
}

void OpeDB::init()
{
    m_db.setHostName("localhost");
    m_db.setDatabaseName("D:\\Project\\QT\\TcpServer\\cloud.db");
    if (m_db.open())
    {
        QSqlQuery query;
        query.exec("select * from usrInfo");
        while (query.next())
        {
            QString data = QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
            qDebug() << data;
        }
    }
    else
    {
        QMessageBox::critical(NULL, "打开数据库", "打开数据库失败");
    }
}

bool OpeDB::handleRegist(const char *name, const char *pwd)
{
    if (name == NULL || pwd == NULL)
    {
        return false;
    }
    QString data = QString("insert into usrInfo(name, pwd, online) values(\'%1\', \'%2\', 0)").arg(name).arg(pwd);
    QSqlQuery query;
    return query.exec(data);

}

bool OpeDB::handleLogin(const char *name, const char *pwd)
{
    if (name == NULL || pwd == NULL)
    {
        return false;
    }
    QString data = QString("select * from usrInfo where name=\'%1\' and pwd=\'%2\' and online=0").arg(name).arg(pwd);
    QSqlQuery query;
    query.exec(data);
    if (query.next())
    {
        data = QString("update usrInfo set online=1 where name=\'%1\' and pwd=\'%2\'").arg(name).arg(pwd);
        QSqlQuery query;
        query.exec(data);
        return true;
    }
    return false;
}

void OpeDB::handleOffline(const char *name)
{
    if (name == NULL)
    {
        return;
    }
    QString data = QString("update usrInfo set online=0 where name=\'%1\'").arg(name);
    QSqlQuery query;
    query.exec(data);
}

QStringList OpeDB::handleAllOnline()
{
    QString data = QString("select name from usrInfo where online=1");
    QSqlQuery query;
    query.exec(data);

    QStringList result;
    result.clear();

    while (query.next())
    {
        result.append(query.value(0).toString());
    }
    return result;
}

int OpeDB::handleSearchUsr(const char *name)
{
    if (name == NULL)
    {
        return -1;          // 传过来的名字为空值
    }
    QString data = QString("select online from usrInfo where name=\'%1\'").arg(name);
    QSqlQuery query;
    query.exec(data);
    if (query.next())
    {
        int ret = query.value(0).toInt();
        if (ret == 1)
        {
            return 1;       // 用户在线
        }
        else if (ret == 0)
        {
            return 2;       // 用户不在线
        }
        else
        {
            return -2;      // 用户状态为错误的值
        }
    }
    else
    {
        return -3;          // 用户不存在
    }
}

int OpeDB::handleAddFriend(const char *pername, const char *name)
{
    if (pername == NULL || name == NULL)
    {
        return -1;
    }
    QString data = QString("select * from friend where (id=(select id from usrInfo where name=\'%1\') and friendId = (select id from usrInfo where name=\'%2\'))"
                           "or (id = (select id from usrInfo where name=\'%3\') and friendId = (select id from usrInfo where name=\'%4\'))").arg(pername).arg(name).arg(name).arg(pername);

    qDebug() << data;
    QSqlQuery query;
    query.exec(data);

    if (query.next())
    {
        return 0;   // 双方已经是好友
    }

    data = QString("select online from usrInfo where name=\'%1\'").arg(pername);
    query.exec(data);
    if (query.next())
    {
        int ret = query.value(0).toInt();
        if (ret == 1)
        {
            return 1;       // 用户在线
        }
        else if (ret == 0)
        {
            return 2;       // 用户不在线
        }
        else
        {
            return -2;      // 用户状态为错误的值
        }
    }
    else
    {
        return -3;          // 用户不存在
    }
}

bool OpeDB::addFriend(const char *pername, const char *name)
{
    QSqlQuery query;

    query.exec(QString("SELECT id FROM usrInfo WHERE name=\'%1\'").arg(name));
    query.next();
    QString id1 = query.value(0).toString();
    query.clear();

    query.exec(QString("SELECT id FROM usrInfo WHERE name=\'%1\'").arg(pername));
    query.next();
    QString id2 = query.value(0).toString();
    query.clear();

    QString data = QString("insert into friend(id, friendId) values(%1, %2)").arg(id1).arg(id2);
    qDebug() << data;
    return query.exec(data);
}

QStringList OpeDB::handleFlushFriend(const char *name)
{
    QStringList strFriendList;
    strFriendList.clear();
    if (name == NULL)
    {
        return strFriendList;
    }
    QString data = QString("select name from usrInfo where online=1 and id in (select id from friend where friendId in (select id from usrInfo where name=\'%1\'))").arg(name);
    qDebug() << data;
    QSqlQuery query;
    query.exec(data);
    while (query.next())
    {
        strFriendList.append(query.value(0).toString());
    }

    query.clear();
    data = QString("select name from usrInfo where online=1 and id in (select friendId from friend where id in (select id from usrInfo where name=\'%1\'))").arg(name);
    qDebug() << data;
    query.exec(data);
    while (query.next())
    {
        strFriendList.append(query.value(0).toString());
    }
}

bool OpeDB::handleDelFriend(const char *name, const char *friendName)
{
    if (name == NULL)
    {
        return false;
    }
    QString data =  QString("DELETE FROM friend WHERE id IN (SELECT id FROM usrInfo WHERE name=\'%1\') AND friendId IN (SELECT id FROM usrInfo WHERE name=\'%2\')").arg(name).arg(friendName);
    QSqlQuery query;
    query.exec(data);

    query.clear();
    data =  QString("DELETE FROM friend WHERE id IN (SELECT id FROM usrInfo WHERE name=\'%1\') AND friendId IN (SELECT id FROM usrInfo WHERE name=\'%2\')").arg(friendName).arg(name);
    query.exec(data);

    return true;
}
