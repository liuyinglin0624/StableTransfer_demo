#ifndef _TCPKERNEL_H
#define _TCPKERNEL_H

#include "TCPNet.h"
#include "Mysql.h"
#include"packdef.h"
#include<list>

class TcpKernel;
typedef void (TcpKernel::*PFUN)(int,char*,int nlen);

typedef struct
{
    PackType m_type;
    PFUN m_pfun;
} ProtocolMap;



class TcpKernel:public IKernel
{
public:
    int Open();
    void Close();
    void DealData(int,char*,int);

    //注册
    void RegisterRq(int,char*,int);
    //登录
    void LoginRq(int,char*,int);

    void UpdatePictureRq(int clientfd, char*szbuf, int nlen);

    void GetDownloadList(list<FileInfo *> &lstFileinfo);

    void StableDownLoadRq(int clientfd, char*szbuf, int nlen);

    void DownLoadDataRs(int clientfd, char*szbuf, int nlen);
 private:
    CMysql * m_sql;
    TcpNet * m_tcp;
};






////读取上传的视频流信息
//void UpLoadVideoInfoRq(int,char*);
//void UpLoadVideoStreamRq(int,char*);
//void GetVideoRq(int,char*);
//char* GetVideoPath(char*);
//void QuitRq(int,char*);
//void PraiseVideoRq(int,char*);
//void GetAuthorInfoRq(int,char*);
#endif
