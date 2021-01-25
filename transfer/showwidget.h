#ifndef SHOWWIDGET_H
#define SHOWWIDGET_H

#include <QWidget>
#include"qmytcpclient.h"
#include<QFile>
#include<QMap>
#include<QThread>
#include<downloaddialog.h>

QT_BEGIN_NAMESPACE
namespace Ui { class ShowWidget; }
QT_END_NAMESPACE

struct FileInfo
{
    int m_nFileID;
    int m_nVideoID;
    int64_t m_nFilePos;
    QFile* pFile;
    int64_t m_nFileSize;
    QString m_strFileName;
    QString m_strFilePath;
    QString m_strRtmp;
};

class DownLoadWorker:public QObject
{
    Q_OBJECT
public:
    DownLoadWorker(){}
    ~DownLoadWorker(){}
public slots:
    void doWork(char* buf);
};


class ShowWidget : public QWidget
{
    Q_OBJECT

public:
    ShowWidget(QWidget *parent = nullptr);
    ~ShowWidget();
signals:
    void SIG_DownLoadFileBlock(char* buf);
public:
    // DownLoadWorker;
    void DownLoadFileBlock(char* szBuf);
private slots:
    void slot_ReadyDate(char* szBuf, int nLen);

    void slot_SendDownLoadRs(char* szBuf,int nLen);
private slots:
    void on_pb_reflush_clicked();

    void on_pb_1_clicked();

private:
    void deal_RegisterRs(char* szBuf, int nLen);
    void deal_UpdatePicture(char* szBuf, int nLen);
    void deal_DownLoadFileBlock(char* szBuf, int nLen);
    void deal_StableDownLoadRs(char* szBuf, int nLen);
    void deal_DownLoadDataRq(char* szBuf, int nLen);
private:
    void Release();
public:
     static ShowWidget * m_pWidget;
private:
    Ui::ShowWidget *ui;
    QMyTcpClient* m_pClient;
    QMap<int,FileInfo*> m_FileIDtoFileInfo;
    // 用于下载文件的worker
    DownLoadWorker* m_pWorker;
    QThread* m_pDownLoadThread;

    // 稳定文件下载的对话框与下载文件之间的映射关系
    QMap<int,DownLoadDialog*> m_mapVideoIDtoDownLoadDig;
};
#endif // SHOWWIDGET_H
