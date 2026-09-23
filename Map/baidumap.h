/**
 * @file        baidumap.h
 * @brief       百度地图窗口类声明
 * @note        通过百度地图静态图 API 获取地图图片并显示，支持地图缩放、
 *              定位（GPS 经纬度或默认城市）以及语音控制指令。
 */
#ifndef BAIDUMAP_H
#define BAIDUMAP_H

#include "gps.h"
#include <QMainWindow>
#include <QTimer>
#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QDebug>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVector>
#include <QFile>
#include <QDate>
#include <QSslSocket>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QAudioRecorder>
#include <QPair>
#include <QPainter>
#include <QDebug>

/* 地图模块命令定义（用于语音控制） */
#define MAP_COMMAND_CLOSE 0     /* 关闭地图 */
#define MAP_COMMAND_SHOW 1      /* 显示地图 */
#define MAP_COMMAND_AMPLIFY 2   /* 地图放大 */
#define MAP_COMMAND_SHRINK 3    /* 地图缩小 */


namespace Ui {
class BaiduMap;
}

/* 百度地图窗口类 */
class BaiduMap : public QMainWindow
{
    Q_OBJECT

public:
    explicit BaiduMap(QWidget *parent = nullptr);
    ~BaiduMap();

private:
    Ui::BaiduMap *ui;
private:
    QNetworkAccessManager netManager; /* 网络管理器：请求地图图片 */
    QNetworkRequest request;          /* 网络请求对象 */
    QFile file;
    char N_S_Flag = 'N';    /* 纬度南北半球标志（N/S） */
    char E_W_Flag = 'E';    /* 经度东西半球标志（E/W） */
    double jingdu = 112.5620; /* 默认经度 */
    double weidu = 23.1341;   /* 默认纬度 */

    int img_Zoom=10;        /* 当前地图缩放级别 */
    int img_Zoon_Min=3;     /* 最小缩放级别 */
    int img_Zoom_Max=18;    /* 最大缩放级别 */
    int img_Width=1024;     /* 地图图片宽度 */
    int img_Height=600;     /* 地图图片高度 */
    /* 百度地图静态图 API 地址模板 */
    QString MapImgUrl="http://api.map.baidu.com/staticimage/v2?ak=xXbBdGaD59fIl4UmSjwSos2vwDSjGGx6"
                        "&width=800&height=480&scale=1&copyright=1&center=%1&markers=%1&zoom=%2&coor=wgs84ll";
private:
    /* 刷新地图（重新请求百度地图 API） */
    void updateMap();
public slots:
    /* 处理地图图片网络请求返回结果 */
    void ReceiveMapImg(QNetworkReply *reply);

    /* 点击放大按钮 */
    void on_pBtn_ZoomUp_clicked();
    /* 点击缩小按钮 */
    void on_pBtn_ZoomDown_clicked();
    /* 点击关闭按钮 */
    void on_pBtn_Close_clicked();
    /* 处理语音控制传来的指令 */
    void on_handleCommand(int);
};

#endif // BAIDUMAP_H
