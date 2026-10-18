/**
 * @file        mainwindow.h
 * @brief       主窗口类声明
 * @note        主界面负责整合表盘时钟、温湿度、音乐、天气、地图和倒车影像
 *              等模块，并通过按钮在各功能子窗口之间切换。
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include <QVBoxLayout>
#include <QParallelAnimationGroup>
#include "Music/musicplayer.h"
#include "Weather/weather.h"
#include "clock.h"
#include "dht11.h"
#include "settingwindow.h"
#include "onevideo.h"
#include "Map/baidumap.h"
#include "Monitor/monitor.h"

namespace Ui {
class MainWindow;
}

/**
 * @brief 程序主窗口：承载并调度各功能子窗口
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    /* 各功能按钮的点击槽函数：隐藏主界面并打开对应子窗口 */
    void on_Weather_Button_clicked();

    void on_Music_Button_clicked();

    void on_Map_Button_clicked();

    void on_Camera_Button_clicked();

    /* 定时刷新时间日期显示 */
    void on_timer_updateTime();
    /* 刷新温湿度显示 */
    void on_update_humidity_temp(QString, QString);

    /* 打开设置窗口 */
    void on_pBtn_Setting_clicked();

private:
    Ui::MainWindow *ui;
    Clock clock;                     /* 表盘时钟控件 */
    MusicPlayer *musicPlayer;        /* 音乐播放器窗口 */
    Weather *weather;                /* 天气窗口 */
    QVector<QMainWindow *> windows;  /* 所有子窗口集合 */
    QTimer *time;                    /* 时间刷新定时器 */
    Dht11 *dht11;                    /* DHT11 温湿度采集线程 */
    SettingWindow settingWindow;     /* 设置窗口 */
    OneVideo oneVideo;               /* 视频播放控件对象 */
    BaiduMap *baiduMap;              /* 百度地图窗口 */
    Monitor *monitor;                /* 倒车影像监控窗口 */
};

#endif // MAINWINDOW_H
