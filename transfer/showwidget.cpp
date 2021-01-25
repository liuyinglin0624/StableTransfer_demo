#include "showwidget.h"
#include "ui_showwidget.h"
#include"Packdef.h"
#include<QMessageBox>
#include<QDir>
#include<QDebug>
#include<QToolButton>

void DownLoadWorker::doWork(char* buf)
{
    // 输出当前的ID
    qDebug() << "DownLoadWorker" << QThread::currentThreadId();
    ShowWidget::m_pWidget->DownLoadFileBlock(buf);
    delete [] buf;
}

ShowWidget* ShowWidget::m_pWidget = NULL;
ShowWidget::ShowWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ShowWidget)
{
    ui->setupUi(this);
    this->m_pWidget = this; // 初始化自身对象的指针
    this->m_pClient = new QMyTcpClient;
    if(!this->m_pClient->InitNetWork(SERVER_IP,SERVER_PORT))
    {
        QMessageBox::information(this,"Net init","网络连接失败");
    }
    connect(this->m_pClient,SIGNAL(SIG_ReadyData(char*,int) ),this,SLOT(slot_ReadyDate(char* ,int )));

    // 利用多线程的方法，实现文件下载
    this->m_pWorker = new DownLoadWorker;
    this->m_pDownLoadThread = new QThread(this);
    m_pWorker->moveToThread(m_pDownLoadThread);
    connect(this,SIGNAL(SIG_DownLoadFileBlock(char*) ),m_pWorker,SLOT(doWork(char* )) );
    m_pDownLoadThread->start(); // 启动线程



}

ShowWidget::~ShowWidget()
{
    Release();
    delete ui;
}
// 接收到服务器的数据报文
void ShowWidget::slot_ReadyDate(char* szBuf, int nLen)
{
    char* temp = szBuf;
    switch(*(int*)temp)
    {
    case(DEF_PACK_REGISTER_RS):
        deal_RegisterRs(szBuf,nLen);
        break;
    case(DEF_PACK_UPDATEPICTURE_RS):
        deal_UpdatePicture(szBuf,nLen);
        break;
    case(DEF_PACK_DOWNLOADFILEBLOCK_RQ):
    {
        deal_DownLoadFileBlock(szBuf,nLen);
        return ;
    }
        break;
    case(DEF_PACK_STABLEDOWNLOAD_RS): // 稳定下载回复,带头
        deal_StableDownLoadRs(szBuf,nLen);
        break;
    case(DEF_PACK_STABLEDOWNLOAD_DATA_RQ):
        {
            deal_DownLoadDataRq(szBuf,nLen);
            return;
        }
        break;
    }
    delete[] szBuf;
}
 // 处理登录信息的回复
void ShowWidget::deal_RegisterRs(char *szBuf, int nLen)
{
    QMessageBox::information(this,"提示","登录成功");
}

// 对堆区空间的释放
void ShowWidget::Release()
{
    delete this->m_pClient;
    this->m_pClient = 0;
    // 对于QThread对象的释放，先退出，再等待
    this->m_pDownLoadThread->terminate();
    this->m_pDownLoadThread->wait();
    delete m_pDownLoadThread;
    delete m_pWorker;
}

// 点击下载图片
void ShowWidget::on_pb_reflush_clicked()
{
    STRU_UPDATEPICTURE_RQ rq;
    this->m_pClient->SendData((char*)&rq,sizeof(rq));
}

// 接收服务器返回的文件头
void ShowWidget::deal_UpdatePicture(char* szBuf, int nLen)
{
    STRU_UPDATEPICTURE_RS* rs = (STRU_UPDATEPICTURE_RS*)szBuf;

    // 将服务器发送过来的文件信息，保存在FileInfo结构体中
    FileInfo* info = new FileInfo;
    info->m_nFileID = rs->m_nFileID;
    info->m_nFilePos = 0;
    info->m_nFileSize = rs->m_nFileSize;
    info->m_nVideoID = rs->m_nVideoID;
    info->m_strRtmp = rs->m_szRtmp;
    info->m_strFileName = rs->m_szFileName;
    info->m_strFilePath = QDir::currentPath();  // 返回当前的路径
    info->m_strFilePath += "/temp";
    QDir dir;
    dir.setPath(info->m_strFilePath);
    if(!dir.exists())
    {
        dir.mkdir(info->m_strFilePath);
    }
    //申请文件指针
    info->pFile = new QFile(info->m_strFilePath +"/"+ info->m_strFileName);
    if(info->pFile->open(QIODevice::WriteOnly))
    {
        m_FileIDtoFileInfo[rs->m_nFileID] = info;
    }

}

void ShowWidget::deal_DownLoadFileBlock(char* szBuf, int nLen)
{
    emit SIG_DownLoadFileBlock(szBuf);
}

void ShowWidget::DownLoadFileBlock(char* szBuf)
{
    STRU_DOWNLOAD_FILEBLOCK_RQ* rq = (STRU_DOWNLOAD_FILEBLOCK_RQ*)szBuf;
    FileInfo* info = 0;
    if(this->m_FileIDtoFileInfo.find(rq->m_nFileID) != m_FileIDtoFileInfo.end())
    {// 找到了
        info = m_FileIDtoFileInfo[rq->m_nFileID];
        // 向文件指针中写入对象
        int64_t len = info->pFile->write(rq->m_szFileContent,rq->m_nBlockLen);
        info->m_nFilePos += len;
        // 若文件写入完成
        if(info->m_nFilePos >= info->m_nFileSize || rq->m_nBlockLen < MAX_CONTENT_LEN)
        {
            // 关闭文件
            info->pFile->close();
            delete info->pFile;
            info->pFile = 0;

            // 同时，将图片显示在控件上
            // 首先，拼接控件的名称,利用控件名字 搜素控件对象
            QString strButtonName = QString("pb_%1").arg(info->m_nFileID);

            QPushButton* button = ui->widget->findChild<QPushButton*>(strButtonName);
            //button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            button->setIcon(QIcon(info->m_strFilePath +"/" + info->m_strFileName));
            button->setText(QString("%1").arg(info->m_nFileID));
        }
    }

}

// 下载对应的文件1.
void ShowWidget::on_pb_1_clicked()
{
    // 向服务器发送稳定下载的请求
    STRU_STABLEDOWNLOAD_RQ rq;
    rq.m_nVideoID = 1;
    this->m_pClient->SendData((char*)&rq,sizeof(rq));
}

// 接受服务器发送的稳定下载回复
void ShowWidget::deal_StableDownLoadRs(char* szBuf, int nLen)
{
    STRU_STABLEDOWNLOAD_RS* rs = (STRU_STABLEDOWNLOAD_RS*)szBuf;
    if(!rs->m_nResult)
    {
        QMessageBox::information(this,"提示","文件不存在");
    }

    if(this->m_mapVideoIDtoDownLoadDig.find(rs->m_nVideoID) == m_mapVideoIDtoDownLoadDig.end())
    {// 若没有找到,说明文件正常下载
        DownLoadDialog* downloadDlg = new DownLoadDialog;
        downloadDlg->SetInfo(rs->m_szFileName,rs->m_nFileSize,rs->m_nVideoID,rs->m_nUserID);
        connect(downloadDlg,SIGNAL(SIG_SendDownLoadRs(char* ,int )),this,SLOT(slot_SendDownLoadRs(char* ,int)) );
        downloadDlg->show();
        m_mapVideoIDtoDownLoadDig[rs->m_nVideoID] = downloadDlg;
    }
    else
    {// 若找到了

    }
}

// 下载数据请求
void ShowWidget::deal_DownLoadDataRq(char* szBuf, int nLen)
{
    STRU_STABLEDOWNLOADATA_RQ* rq = (STRU_STABLEDOWNLOADATA_RQ*)szBuf;
    if(this->m_mapVideoIDtoDownLoadDig.find(rq->m_nFileID) != m_mapVideoIDtoDownLoadDig.end())
    {// 若找到了下载对话框
        DownLoadDialog* DownLoadDlg = m_mapVideoIDtoDownLoadDig[rq->m_nFileID];
        DownLoadDlg->DownLoadData(szBuf);
    }
}
// 发送下载文件请求
void ShowWidget::slot_SendDownLoadRs(char* szBuf,int nLen)
{
    STRU_STABLEDOWNLOADATA_RS* rs = (STRU_STABLEDOWNLOADATA_RS*)szBuf;
    this->m_pClient->SendData(szBuf,nLen);

    delete szBuf;
}
