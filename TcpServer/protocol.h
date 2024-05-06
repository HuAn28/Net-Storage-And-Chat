#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define REGIST_OK "Regist ok."
#define REGIST_FAILED "Regist failed: Name existed."
#define LOGIN_OK "Login ok."
#define LOGIN_FAILED "Login failed: Name error or pwd error or relogin."
#define SEARCH_USR_NO "No such people."
#define SEARCH_USR_ONLINE "Online."
#define SEARCH_USR_OFFLINE "Offline."
#define SEARCH_USR_ERROR "Server has an error."
#define FRIEND_UNKNOW_ERROR "Unknow error."
#define FRIEND_EXISTED "Friend exist."
#define FRIEND_OFFLINE "Usr offline."
#define FRIEND_NOEXIST "Usr not exist."
#define DEL_FRIEND_OK "Delete friend ok."
#define DIR_NO_EXIST "Cur dir not exist."
#define FILE_NAME_EXIST "File name exist."
#define CREATE_DIR_OK "Create dir ok."
#define DEL_DIR_OK "Delete dir ok."
#define DEL_DIR_FAILURED "Delete dir failured: Is reguler file."
#define RENAME_FILE_OK "Rename file ok."
#define RENAME_FILE_FAILURED "Rename file failured."
#define ENTER_DIR_FAILURED "Enter dir failured: Is reguler file."
#define ENTER_DIR_SUCCESS "Enter dir success."
#define UPLOAD_FILE_OK "Upload file ok."
#define UPLOAD_FILE_FAILURED "Upload file failured."
#define DEL_FILE_OK "Delete file ok."
#define DEL_FILE_FAILURED "Delete file failured: Is reguler failured."
#define MOVE_FILE_OK "Move file ok."
#define MOVE_FILE_FAILURED "Move file failured: Is reguler file."
#define COMMON_ERR "Operate failed: System is busy."

typedef unsigned int uint;

enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN = 0,
    ENUM_MSG_TYPE_REGIST_REQUEST,           // 注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,           // 注册回复
    ENUM_MSG_TYPE_LOGIN_REQUEST,            // 登录请求
    ENUM_MSG_TYPE_LOGIN_RESPOND,            // 登录回复
    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,       // 在线用户请求
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND,       // 在线用户回复
    ENUM_MSG_TYPE_SERCH_USR_REQUEST,        // 搜索用户请求
    ENUM_MSG_TYPE_SERCH_USR_RESPOND,        // 搜索用户回复
    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,       // 加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,       // 加好友回复
    ENUM_MSG_TYPE_ADD_FRIEND_AGGREE,        // 同意添加好友
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,        // 拒绝添加好友
    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST,     // 刷新好友请求
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND,     // 刷新好友回复
    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,    // 删除好友请求
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND,    // 删除好友回复
    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST,     // 私聊请求
    ENUM_MSG_TYPE_GROUP_CHAT_REQUEST,       // 群聊请求
    ENUM_MSG_TYPE_CREATE_DIR_REQUEST,       // 创建文件夹请求
    ENUM_MSG_TYPE_CREATE_DIR_RESPOND,       // 创建文件夹回复
    ENUM_MSG_TYPE_FLUSH_FILE_REQUEST,       // 刷新文件请求
    ENUM_MSG_TYPE_FLUSH_FILE_RESPOND,       // 刷新文件回复
    ENUM_MSG_TYPE_DEL_DIR_REQUEST,          // 删除目录请求
    ENUM_MSG_TYPE_DEL_DIR_RESPOND,          // 删除目录回复
    ENUM_MSG_TYPE_RENAME_FILE_REQUEST,      // 重命名文件请求
    ENUM_MSG_TYPE_RENAME_FILE_RESPOND,      // 重命名文件回复
    ENUM_MSG_TYPE_ENTER_DIR_REQUEST,        // 进入文件夹请求
    ENUM_MSG_TYPE_ENTER_DIR_RESPOND,        // 进入文件夹回复
    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST,      // 上传文件请求
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND,      // 上传文件回复
    ENUM_MSG_TYPE_DEL_FILE_REQUEST,         // 删除文件请求
    ENUM_MSG_TYPE_DEL_FILE_RESPOND,         // 删除文件回复
    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST,    // 下载文件请求
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND,    // 下载文件回复
    ENUM_MSG_TYPE_SHARE_FILE_REQUEST,       // 共享文件请求
    ENUM_MSG_TYPE_SHARE_FILE_RESPOND,       // 共享文件回复
    ENUM_MSG_TYPE_SHARE_FILE_NOTE,          // 共享文件通知
    ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND,  // 共享文件通知回复
    ENUM_MSG_TYPE_MOVE_FILE_REQUEST,        // 移动文件请求
    ENUM_MSG_TYPE_MOVE_FILE_RESPOND,        // 移动文件回复
    // ENUM_MSG_TYPE_REQUEST,
    // ENUM_MSG_TYPE_RESPOND,
    ENUM_MSG_TYPE_MAX = 0x00ffffff,
};

struct FileInfo
{
    char caFileName[32];    // 文件名字
    int iFileType;          // 文件类型
};

struct PDU
{
    uint uiPDULen;      // 送的协议数据单元大小
    uint uiMsgType;     // 消息类型
    char caData[64];
    uint uiMsgLen;      // 实际消息长度
    int caMsg[];        // 实际消息
};

PDU *mkPDU(uint uiMsgLen);

#endif // PROTOCOL_H
