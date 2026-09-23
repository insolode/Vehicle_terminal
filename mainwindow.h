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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_Weather_Button_clicked();

    void on_Music_Button_clicked();

    void on_Map_Button_clicked();

    void on_Camera_Button_clicked();

    void on_timer_updateTime();
    void on_update_humidity_temp(QString, QString);

    void on_pBtn_Setting_clicked();

private:
    Ui::MainWindow *ui;
    Clock clock;
    MusicPlayer *musicPlayer;
    Weather *weather;
    QVector<QMainWindow *> windows;
    QTimer *time;
    Dht11 *dht11;
    SettingWindow settingWindow;
    OneVideo oneVideo;          /* 视频播放控件对象 */
    BaiduMap *baiduMap;         /* 百度地图窗口 */
    Monitor *monitor;           /* 倒车影像监控窗口 */


};

#endif // MAINWINDOW_H
