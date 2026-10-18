/**
 * @file        settingwindow.h
 * @brief       设置窗口类声明
 * @note        提供系统时间/日期修改、GIF 屏保切换以及 WiFi 连接等功能。
 */
#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include <QMainWindow>
#include <QMovie>
#include <QDir>
#include <QFile>
#include <QMediaObject>
#include <QTimer>
#include <QTime>
#include <QDate>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>
#include <QThread>
#include <QListWidgetItem>
#include <QShowEvent>

namespace Ui {
class SettingWindow;
}

/**
 * @brief 设置窗口类
 */
class SettingWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingWindow(QWidget *parent = nullptr);
    ~SettingWindow();

    /* 扫描本地 GIF 文件并加载播放 */
    void ScanGif();

    /* 读取当前系统时间，显示到 时/分/秒 spinBox */
    void loadCurrentTime();

protected:
    /* 窗口显示时刷新 spinBox 为当前系统时间 */
    void showEvent(QShowEvent *event) override;

private slots:
    /* 修改系统时间 */
    void on_pBtn_ModifyTime_clicked();

    /* 修改系统日期 */
    void on_pBtn_ModifyDate_clicked();

    /* 切换 GIF 屏保 */
    void on_pBtn_SwitchGif_clicked();

    /* 返回按钮 */
    void on_pushButton_8_clicked();

    /* 连接 WiFi */
    void on_btn_connect_wifi_clicked();

    /* 开启 WiFi */
    void on_btn_wifi_on_clicked();

    /* 扫描 WiFi 网络 */
    void on_btn_scan_network_clicked();

    /* 定时刷新日期时间显示 */
    void on_timer_updateTime();

    /* 暂停/播放 GIF 屏保 */
    void on_pBtn_PauseGif_clicked(bool checked);

private:
    Ui::SettingWindow *ui;
    QString LocalGifPath = "/MyGif";   /* 存放 GIF 文件的相对路径 */
    QMovie *movie = new QMovie;        /* 用于播放 GIF 动画 */
    QVector<QString> gif_Files;        /* 保存扫描到的 GIF 文件路径列表 */
    int currentGifIndex = 0;           /* 当前播放的 GIF 索引 */
    int GifSum = 0;                    /* GIF 文件总数 */
    QTimer *timer;                     /* 定时刷新日期时间的定时器 */
    QProcess process;                  /* 用于执行系统命令（date/hwclock/wifi 等） */
};

#endif // SETTINGWINDOW_H
