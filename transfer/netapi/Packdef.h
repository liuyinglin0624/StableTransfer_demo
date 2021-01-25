#ifndef __DEF_PACKDEF__
#define __DEF_PACKDEF__

#include<QHostAddress>
#include<QByteArray>
#include<QDebug>
#include<QFile>
#include<QDateTime>
#include <winsock2.h>
#define BOOL bool
#define DEF_PACK_BASE  (10000)


//注册
#define  DEF_PACK_REGISTER_RQ    (DEF_PACK_BASE + 0)
#define  DEF_PACK_REGISTER_RS    (DEF_PACK_BASE + 1)

//登录
#define  DEF_PACK_LOGIN_RQ    (DEF_PACK_BASE + 2)
#define  DEF_PACK_LOGIN_RS    (DEF_PACK_BASE + 3)

//创建房间
#define DEF_PACK_CREATEROOM_RQ  (DEF_PACK_BASE + 4)
#define DEF_PACK_CREATEROOM_RS  (DEF_PACK_BASE + 5)

//加入房间
#define DEF_PACK_JOINROOM_RQ  (DEF_PACK_BASE + 6)
#define DEF_PACK_JOINROOM_RS  (DEF_PACK_BASE + 7)

//房间列表请求
#define DEF_PACK_ROOM_MEMBER    (DEF_PACK_BASE + 8)

//音频数据
#define DEF_PACK_AUDIO_FRAME    (DEF_PACK_BASE + 9)

//退出房间请求
#define DEF_PACK_LEAVEROOM_RQ   (DEF_PACK_BASE + 10)

//退出房间回复
#define DEF_PACK_LEAVEROOM_RS   (DEF_PACK_BASE + 11)

//更新图片请求
#define DEF_PACK_UPDATEPICTURE_RQ  (DEF_PACK_BASE + 12)
#define DEF_PACK_UPDATEPICTURE_RS  (DEF_PACK_BASE + 13)

//下載文件內容
#define DEF_PACK_DOWNLOADFILEBLOCK_RQ  (DEF_PACK_BASE + 14)
#define DEF_PACK_DOWNLOADFILEBLOCK_RS  (DEF_PACK_BASE + 15)

// 稳定文件下载
#define DEF_PACK_STABLEDOWNLOAD_RQ  (DEF_PACK_BASE + 16)
#define DEF_PACK_STABLEDOWNLOAD_RS  (DEF_PACK_BASE + 17)

// 稳定文件传输数据
#define DEF_PACK_STABLEDOWNLOAD_DATA_RQ (DEF_PACK_BASE + 18)
#define DEF_PACK_STABLEDOWNLOAD_DATA_RS (DEF_PACK_BASE + 19)

//注册请求结果
#define userid_is_exist      0
#define register_sucess      1

//登录请求结果
#define userid_no_exist      0
#define password_error       1
#define login_sucess         2
#define user_online          3

//创建房间结果
#define room_is_exist        0
#define create_success       1

//加入房间结果
#define room_no_exist        0
#define join_success         1

//上传请求结果
#define file_is_exist        0
#define file_uploaded        1
#define file_uploadrq_sucess 2
#define file_upload_refuse   3

//上传回复结果
#define fileblock_failed     0
#define fileblock_success    1

//下载请求结果
#define file_downrq_failed   0
#define file_downrq_success  1

#define _downloadfileblock_fail  0
#define _downloadfileblock_success	1

// 稳定下载请求结果
#define data_downloadrq_noexist 0
#define data_downloadrq_success 1

// 稳定传输数据结果
#define stable_download_fail     0
#define stable_download_success  1

#define DEF_PACK_COUNT (100)

#define MAX_PATH            (280 )
#define MAX_SIZE            (60  )
#define DEF_HOBBY_COUNT     (8  )
#define MAX_CONTENT_LEN     (4096 )
#define _DEF_PORT (8000)


/////////////////////网络//////////////////////////////////////


#define DEF_MAX_BUF	  1024
#define DEF_BUFF_SIZE	  4096
#define SERVER_IP     "192.168.153.129"
#define SERVER_PORT       8899
typedef int PackType;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//登录请求
typedef struct STRU_LOGIN_RQ
{
    STRU_LOGIN_RQ()
    {
        m_nType = DEF_PACK_LOGIN_RQ;
        memset(m_szUser,0,MAX_SIZE);
        memset(m_szPassword,0,MAX_SIZE);
    }

    PackType m_nType;   //包类型
    char     m_szUser[MAX_SIZE] ; //用户ID
    char     m_szPassword[MAX_SIZE];  //密码
}STRU_LOGIN_RQ;


//登录回复
typedef struct STRU_LOGIN_RS
{
    STRU_LOGIN_RS()
    {
        m_nType= DEF_PACK_LOGIN_RS;
    }
    PackType m_nType;   //包类型
    int  m_UserID;
    int  m_lResult ; //注册结果

}STRU_LOGIN_RS;


//注册请求
typedef struct STRU_REGISTER_RQ
{
    STRU_REGISTER_RQ()
    {
        m_nType = DEF_PACK_REGISTER_RQ;
        memset(m_szUser,0,MAX_SIZE);
        memset(m_szPassword,0,MAX_SIZE);
    }

    PackType m_nType;   //包类型
    char     m_szUser[MAX_SIZE] ; //用户名
    char     m_szPassword[MAX_SIZE];  //密码

}STRU_REGISTER_RQ;

//注册回复
typedef struct STRU_REGISTER_RS
{
    STRU_REGISTER_RS()
    {
        m_nType= DEF_PACK_REGISTER_RS;
    }
    PackType m_nType;   //包类型
    int  m_lResult ; //注册结果

}STRU_REGISTER_RS;

//创建房间请求
typedef struct STRU_CREATEROOM_RQ
{
    STRU_CREATEROOM_RQ()
    {
        m_nType = DEF_PACK_CREATEROOM_RQ;
        m_UserID = 0;
        m_RoomID = 0;
    }

    PackType m_nType;   //包类型
    int m_UserID;
    int m_RoomID;

}STRU_CREATEROOM_RQ;

//创建房间回复
typedef struct STRU_CREATEROOM_RS
{
    STRU_CREATEROOM_RS()
    {
        m_nType= DEF_PACK_CREATEROOM_RS;
        m_lResult = 0;
        m_RoomId = 0;
        m_AudioPort = 0;
        memset( m_AudioIP , 0, 20);
    }
    PackType m_nType;   //包类型
    int  m_lResult ;    //注册结果
    int  m_RoomId;
    int  m_AudioPort;
    char m_AudioIP[20];

}STRU_CREATEROOM_RS;

//加入房间请求
typedef struct STRU_JOINROOM_RQ
{
    STRU_JOINROOM_RQ()
    {
        m_nType = DEF_PACK_JOINROOM_RQ;
        m_UserID = 0;
        m_RoomID = 0;
    }

    PackType m_nType;   //包类型
    int m_UserID;
    int m_RoomID;

}STRU_JOINROOM_RQ;
//加入房间回复
typedef struct STRU_JOINROOM_RS
{
    STRU_JOINROOM_RS()
    {
        m_nType= DEF_PACK_JOINROOM_RS;
        m_lResult = 0;
        m_RoomId = 0;
        m_AudioPort = 0;
        memset(m_AudioIP,0,20);
    }
    PackType m_nType;   //包类型
    int  m_lResult ;    //注册结果
    int  m_RoomId;
    int m_AudioPort;
    char m_AudioIP[20];

}STRU_JOINROOM_RS;

//房间成员请求
typedef struct STRU_ROOM_MEMBER_RQ
{
    STRU_ROOM_MEMBER_RQ()
    {
        m_nType= DEF_PACK_ROOM_MEMBER;
        m_AudioPort = 0;
        m_UserID =0;
        memset(m_szUser,0,MAX_SIZE);
        memset(m_RtmpUrl,0,MAX_SIZE);
        memset(m_AudioIP,0,20);
    }
    PackType m_nType;   //包类型
    int m_UserID;
    int m_AudioPort;
    char m_szUser[MAX_SIZE];
    char m_RtmpUrl[MAX_SIZE];
    char m_AudioIP[20];

}STRU_ROOM_MEMBER_RQ;


//离开房间请求
typedef struct STRU_LEAVEROOM_RQ
{
    STRU_LEAVEROOM_RQ()
    {
        m_nType = DEF_PACK_LEAVEROOM_RQ;
        m_nUserId = 0;
        m_RoomId = 0;
    }
    PackType   m_nType;   //包类型
    int    m_nUserId; //用户ID
    int    m_RoomId;
}STRU_LEAVEROOM_RQ;
//离开房间回复
typedef struct STRU_LEAVEROOM_RS
{
    STRU_LEAVEROOM_RS()
    {
        m_nType = DEF_PACK_LEAVEROOM_RS;
        m_UserID = 0;
        memset(szUserName,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_UserID;
    char szUserName[MAX_SIZE];
}STRU_LEAVEROOM_RS;

// 更新图片请求
typedef struct STRU_UPDATEPICTURE_RQ
{
    STRU_UPDATEPICTURE_RQ()
    {
        m_nType = DEF_PACK_UPDATEPICTURE_RQ;
    }
    PackType   m_nType;   //包类型
}STRU_UPDATEPICTURE_RQ;

// 更新图片回复
typedef struct STRU_UPDATEPICTURE_RS
{
    PackType m_nType;
    int      m_nFileID;
    int64_t  m_nFileSize;
    int      m_nVideoID;
    char     m_szFileName[MAX_SIZE];
    char     m_szRtmp[MAX_PATH];
}STRU_UPDATEPICTURE_RS;

// 下载文件块请求
typedef struct STRU_DOWNLOAD_FILEBLOCK_RQ
{
    PackType m_nType;
    int     m_nUserID;
    int     m_nFileID;
    int     m_nBlockLen;
    char    m_szFileContent[MAX_CONTENT_LEN];
}STRU_DOWNLOAD_FILEBLOCK_RQ;

// 下载文件块回复
typedef struct STRU_DOWNLOAD_FILEBLOCK_RS
{
    PackType m_nType;
    int     m_nUserID;
    int     m_nFileID;
    int     m_nResult;
    int64_t m_nBlockLen;
}STRU_DOWNLOAD_FILEBLOCK_RS;

// 稳定下载请求
typedef struct STRU_STABLEDOWNLOAD_RQ
{
    STRU_STABLEDOWNLOAD_RQ()
    {
        m_nType = DEF_PACK_STABLEDOWNLOAD_RQ;
    }
    PackType m_nType;
    int      m_nVideoID;  // 想要下载的文件ID
}STRU_STABLEDOWNLOAD_RQ;

// 稳定下载回复(带头)
typedef struct STRU_STABLEDOWNLOAD_RS
{
    STRU_STABLEDOWNLOAD_RS()
    {
        m_nType = DEF_PACK_STABLEDOWNLOAD_RS;
    }
    PackType m_nType;
    int      m_nResult;    // 请求结果,
    int      m_nVideoID;   // 回复文件ID
    int     m_nUserID;     // 文件上传者ID
    int64_t  m_nFileSize;  // 文件大小
    char     m_szFileName[MAX_SIZE];  // 文件名
}STRU_STABLEDOWNLOAD_RS;


// 发送文件数据请求
typedef struct STRU_STABLEDOWNLOADATA_RQ
{
    STRU_STABLEDOWNLOADATA_RQ()
    {
        m_nType = DEF_PACK_STABLEDOWNLOAD_DATA_RQ;
    }
    PackType m_nType;
    int     m_nFileID;       // 对应的文件ID
    int64_t m_nPostion;      // 该长度的字节的写入位置
    int64_t m_nBlockLen;     // 文件字节长度
    char    m_szFileContent[MAX_CONTENT_LEN];
}STRU_STABLEDOWNLOADATA_RQ;

// 文件数据回复
typedef struct STRU_STABLEDOWNLOADATA_RS
{
    STRU_STABLEDOWNLOADATA_RS()
    {
        m_nType = DEF_PACK_STABLEDOWNLOAD_DATA_RS;
    }
    PackType m_nType;
    int     m_nResult;   // 传输的字节位置写入是否成功
    int     m_nFileID;   // 文件对应的ID
    int64_t m_nPostion;  // 字节写入位置
    int64_t m_nBlockLen;  // 上次传输的字节大小
}STRU_STABLEDOWNLOADATA_RS;

#endif
