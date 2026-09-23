/**
 * @file        baidumap.cpp
 * @brief       百度地图窗口类实现
 */
#include "baidumap.h"
#include "ui_baidumap.h"

/* 百度地图窗口构造函数 */
BaiduMap::BaiduMap(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BaiduMap)
{
    ui->setupUi(this);
    /* 调整按钮位置与大小 */
    ui->pBtn_ZoomUp->move(924,500);
    ui->pBtn_ZoomDown->move(824,500);
    ui->pBtn_Close->resize(50,50);
    ui->pBtn_Close->move(0,0);
    /* 连接地图图片网络请求完成的信号槽 */
    connect(&netManager,SIGNAL(finished(QNetworkReply *)),this,SLOT(ReceiveMapImg(QNetworkReply *)));
    /* 初始化 GPS 并刷新地图 */
    gps_init();
    updateMap();
}

/* 百度地图窗口析构函数 */
BaiduMap::~BaiduMap()
{
    delete ui;
}

/*  刷新地图  (重新访问百度地图API，发起网络请求)*/
void BaiduMap::updateMap()
{
    QString center;
    /* 如果 GPS 定位有效，则用经纬度作为地图中心，否则用默认城市 */
    if(gps_valid)
    {
        getGpsData(&N_S_Flag,&E_W_Flag,&jingdu,&weidu);
        center = QString("%1,%2").arg(QString::number(jingdu,'f',5)).arg(QString::number(weidu,'f',5));
    }
    else
    {
        center = "长沙";
    }
    /* 拼接地图图片 URL 并发起请求 */
    QString url = MapImgUrl.arg(center).arg(img_Zoom);
    qDebug()<<url;
    request.setUrl(QUrl(url));
    netManager.get(request);  //开始获取网络请求的返回结果
}


/*  处理网络返回结果，解析图像并显示图像  */
void BaiduMap::ReceiveMapImg(QNetworkReply *reply)
{
    //qDebug()<<"receive map img";
    QFile file;
    file.setFileName("map.png");
    QByteArray data = reply->readAll();
    if(data.isEmpty())
    {
        qDebug()<<"data is null";
        return ;
    }
    /* 将地图图片数据写入本地文件 map.png */
    file.open(QIODevice::Append | QIODevice::Truncate);
    file.write(data);
    file.close();
    /* 通过样式表将图片设置为窗口背景 */
    this->setStyleSheet("QWidget{border-image: url(./map.png);}");
}


/* 点击地图放大按钮 进行的操作 */
void BaiduMap::on_pBtn_ZoomUp_clicked()
{
    /* 达到最大缩放级别时不再放大 */
    if(img_Zoom==img_Zoom_Max)
        return;

    img_Zoom++;
    updateMap();
}


/* 点击地图缩小按钮 进行的操作 */
void BaiduMap::on_pBtn_ZoomDown_clicked()
{
    /* 达到最小缩放级别时不再缩小 */
    if(img_Zoom==img_Zoon_Min)
        return;
    img_Zoom--;
    updateMap();
}

/*  关闭地图界面  */
void BaiduMap::on_pBtn_Close_clicked()
{
    if (parentWidget()) {
        parentWidget()->show();
    }
    this->hide();
}


/* 用来处理语音识别传递过来的命令 */
void BaiduMap::on_handleCommand(int command)
{
    switch (command) {
    case MAP_COMMAND_SHOW:
        this->show();
        break;
    case MAP_COMMAND_CLOSE:
        this->close();
        break;
    case MAP_COMMAND_AMPLIFY:
        on_pBtn_ZoomUp_clicked();
        break;
    case MAP_COMMAND_SHRINK:
        on_pBtn_ZoomDown_clicked();
        break;
    default:
        break;
    }
}
