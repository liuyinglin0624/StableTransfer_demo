#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

#include <QDialog>
#include<QFile>
#include<QThread>
#include"Packdef.h"
#include<QTime>

namespace Ui {
class DownLoadDialog;
}

// 工作者线程
class Worker:public QObject
{
    Q_OBJECT
public:
    Worker(){}
    ~Worker(){}
public slots:
    void doWork(char* buf);
};

class DownLoadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownLoadDialog(QWidget *parent = nullptr);
    ~DownLoadDialog();
signals:
    void SIG_DownLoadThread(char* szBuf);
    void SIG_SendDownLoadRs(char* szBuf,int nLen);
    void SIG_UpdateDialogInfo(qint64 nLen);
private slots:
    void on_pb_pause_clicked();

    void on_pb_close_clicked();

    void on_pb_openposition_clicked();

    void slot_UpdateDialogInfo(qint64 nLen);
public:
    void SetInfo(QString strName,int64_t size,int videoID,int uploaderID);
    void DownLoadData(char* szBuf);
    void DownLoadData_thread(char* szBuf);
public:
    static DownLoadDialog* m_pDiaLog;
private:
    Ui::DownLoadDialog *ui;
    QString m_strFileName;
    int64_t m_nFileSize;
    int     m_nVideoID;
    int     m_nUpLoaderID;
    QFile*   m_pFile;
    QString m_strDownLoad;
    QString m_strDownLoadPath;
    QTime   m_Lasttime;

    // 用于下载的对象
    Worker* m_pWorker;
    QThread* m_pDownLoadThread;

    // 用于下载的状态变量
    bool m_bDownLoadFlag;
    CRITICAL_SECTION cs_Lock;// 线程安全
    int64_t m_nPausePosition;  // 记录暂停的位置
    int64_t m_nPauseLen;   // 记录暂停时写入的文件大小

};

#endif // DOWNLOADDIALOG_H
