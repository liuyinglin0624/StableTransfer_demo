#include "downloaddialog.h"
#include "ui_downloaddialog.h"
#include<QDir>
#include<QFile>
#include<QDebug>
#include<QDesktopServices>
#include<QUrl>

void Worker::doWork(char* buf)
{
    qDebug() << "doWork" << QThread::currentThreadId();
    DownLoadDialog::m_pDiaLog->DownLoadData_thread(buf);
    delete [] buf;
}

DownLoadDialog* DownLoadDialog::m_pDiaLog = NULL;
DownLoadDialog::DownLoadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DownLoadDialog)
{
    ui->setupUi(this);
    this->m_pDiaLog = this;
    this->m_pWorker = new Worker;
    this->m_pDownLoadThread = new QThread(this);
    this->m_pWorker->moveToThread(m_pDownLoadThread);
    connect(this,SIGNAL(SIG_DownLoadThread(char* )) , this->m_pWorker,SLOT(doWork(char*)) );
    this->m_pDownLoadThread->start(); // 启动线程

    connect(this,SIGNAL(SIG_UpdateDialogInfo(qint64)),this,SLOT(slot_UpdateDialogInfo(qint64)) );
    this->m_Lasttime = QTime::currentTime();// 用于记录当前时间

    this->m_bDownLoadFlag =  true;
    InitializeCriticalSection(&this->cs_Lock);
}

DownLoadDialog::~DownLoadDialog()
{
    this->m_pDownLoadThread->terminate();  // 首先将线程函数关闭
    this->m_pDownLoadThread->wait();
    delete m_pDownLoadThread;
    delete m_pWorker;
    delete ui;
}
// 点击暂停
void DownLoadDialog::on_pb_pause_clicked()
{
    if(this->ui->pb_pause->text() == "暂停")
    {// 暂停下载
        EnterCriticalSection(&this->cs_Lock);
        this->m_bDownLoadFlag = false;
        LeaveCriticalSection(&this->cs_Lock);

        this->ui->pb_pause->setText("开始");
    }
    else if(this->ui->pb_pause->text() == "开始")
    {// 开始下载
        EnterCriticalSection(&this->cs_Lock);
        this->m_bDownLoadFlag = true;
        LeaveCriticalSection(&this->cs_Lock);
        // 发送回复包,继续传输
        STRU_STABLEDOWNLOADATA_RS* rs = new STRU_STABLEDOWNLOADATA_RS;
        rs->m_nFileID = this->m_nVideoID;
        rs->m_nResult = 1;
        rs->m_nPostion = this->m_nPausePosition;
        rs->m_nBlockLen = this->m_nPauseLen;
        emit SIG_SendDownLoadRs((char*)rs,sizeof(STRU_STABLEDOWNLOADATA_RS));
        this->ui->pb_pause->setText("暂停");
    }
}
// 点击取消
void DownLoadDialog::on_pb_close_clicked()
{// 取消下载任务
    // 取消下载,首先将数据传输暂停
    EnterCriticalSection(&this->cs_Lock);
    this->m_bDownLoadFlag = false;
    LeaveCriticalSection(&this->cs_Lock);
    // 将属性进行设置
    this->m_pDownLoadThread->terminate();  // 首先将线程函数关闭
    this->m_pDownLoadThread->wait();
    delete m_pDownLoadThread;
    delete m_pWorker;
    EnterCriticalSection(&this->cs_Lock);
    this->ui->lb_time->setText("预计时间  --:--:--");
    this->ui->lb_speed->setText("当前速度:0 KB/s");
    this->ui->lb_filename->setText("文件名:---");
    this->ui->lb_filesize->setText("文件大小: -MB");
    this->ui->probar_download->setValue(0);
    QFile fileClose(this->m_strDownLoad);
    fileClose.remove();
    LeaveCriticalSection(&this->cs_Lock);

}

// 点击打开所在文件夹
void DownLoadDialog::on_pb_openposition_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(this->m_strDownLoadPath));
}

// 设置文件相关信息
void DownLoadDialog::SetInfo(QString strName,int64_t size,int videoID,int uploaderID)
{
    this->m_strFileName = strName;
    this->m_nFileSize = size;
    this->m_nVideoID = videoID;
    this->m_nUpLoaderID = uploaderID;
    this->ui->lb_filename->setText("文件名:"+m_strFileName);
    // 文件大小为MB级别
    float sizeMB = size/1048576.0;
    QString strSizeMB = QString::number(sizeMB,'f',2);
    this->ui->lb_filesize->setText("文件大小:" + strSizeMB + "MB");

    this->ui->probar_download->setValue(0);
    this->ui->lb_speed->setText("当前速度:0 KB/s");
    this->ui->lb_time->setText("预计时间  --:--:--");

    QString strPath = QDir::currentPath(); // 获取当前的文件目录
    strPath = strPath + "/temp/";
    QDir DirTemp;
    DirTemp.setPath(strPath);
    if(!DirTemp.exists())
    {// 若当前的文件夹不存在
        DirTemp.mkdir(strPath);
    }
    this->m_strDownLoadPath = strPath;

    strPath = strPath + m_strFileName;
    m_strDownLoad = strPath;
    // 申请文件对象指针
    this->m_pFile = new QFile(strPath);

    this->repaint();
}
// 下载数据包
void DownLoadDialog::DownLoadData(char* szBuf)
{
    emit SIG_DownLoadThread(szBuf);
}

// 在线程中进行下载
void DownLoadDialog::DownLoadData_thread(char* szBuf)
{
    // 将接收到的数据请求,并进行下载
    STRU_STABLEDOWNLOADATA_RQ* rq = (STRU_STABLEDOWNLOADATA_RQ*)szBuf;

    // 注意:直接写入文件的模式,此处我们想要使用继续写入不覆盖的模式.采用"a"模式
    if(this->m_pFile->open(QIODevice::Append))
    {// 若文件打开正确
        this->m_pFile->seek(rq->m_nPostion);  // 首先将文件指针跳转到指定位置

        // 将数据写入到指定位置
        int64_t len = this->m_pFile->write(rq->m_szFileContent,rq->m_nBlockLen);
        emit SIG_UpdateDialogInfo(len + rq->m_nPostion);

        if(rq->m_nPostion + len >= this->m_nFileSize)
        {// 若当前文件写入完成
            // 停止发送回复报
            this->m_pFile->close(); // 关闭文件流
            return;
        }
        // 若没有完成写入
        // 继续发送回复报
        STRU_STABLEDOWNLOADATA_RS* rs = new STRU_STABLEDOWNLOADATA_RS;
        rs->m_nFileID = rq->m_nFileID;
        rs->m_nResult = 1;
        rs->m_nPostion = rq->m_nPostion;
        rs->m_nBlockLen = len;
        // 继续发送回复报
        EnterCriticalSection(&this->cs_Lock);
        if(this->m_bDownLoadFlag)
        {// 若当前为运行状态,则继续下载
            emit SIG_SendDownLoadRs((char*)rs,sizeof(STRU_STABLEDOWNLOADATA_RS)); //
        }
        else
        {// 若当前为暂停状态,记录当前完成的写入position以及完成写入的len
            this->m_nPausePosition = rq->m_nPostion;
            this->m_nPauseLen = len;
        }
        LeaveCriticalSection(&this->cs_Lock);

        this->m_pFile->close(); // 关闭文件流
    }
}

void DownLoadDialog::slot_UpdateDialogInfo(qint64 nLen)
{
    int fProcecss = nLen* 100/this->m_nFileSize ;
    EnterCriticalSection(&this->cs_Lock);
    this->ui->probar_download->setValue(fProcecss);// 设置进度条
    LeaveCriticalSection(&this->cs_Lock);
    QTime timeNow = QTime::currentTime();
    // 获取当前的下载时间
    int pts = this->m_Lasttime.msecsTo(timeNow);
    //计算当前速度:当前得到下载字节数 / 下载时间
    float fSpeed = nLen*1000 / ((float)pts * 1024.0);
    QString strSpeed = QString::number(fSpeed,'f',1);

    EnterCriticalSection(&this->cs_Lock);
    if(this->m_bDownLoadFlag)
        this->ui->lb_speed->setText("当前速度:"+strSpeed + " KB/s");
    else
        this->ui->lb_speed->setText("当前速度:--- KB/s");
    LeaveCriticalSection(&this->cs_Lock);

    int64_t nDuration = this->m_nFileSize - nLen;
    // 剩余字节数在当前网速下需要下载的时间
    int64_t nRemainTime = nDuration / (1024 * fSpeed);
    QString strHour = QString("00%1").arg(nRemainTime/3600);
    QString strMinium = QString("00%1").arg((nRemainTime%3600)/60);
    QString strSecond = QString("00%1").arg(nRemainTime%60);

    QString strRemainTime = strHour.right(2) + ":" + strMinium.right(2) + ":" + strSecond.right(2);

    EnterCriticalSection(&this->cs_Lock);
    if(this->m_bDownLoadFlag)
        this->ui->lb_time->setText("预计时间:" + strRemainTime);
    else
        this->ui->lb_time->setText("预计时间:--:--:--");
    LeaveCriticalSection(&this->cs_Lock);
    this->repaint();
}
