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

class SettingWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingWindow(QWidget *parent = nullptr);
    ~SettingWindow();

    void ScanGif();

    /* 读取当前系统时间，显示到 时/分/秒 spinBox */
    void loadCurrentTime();

protected:
    /* 窗口显示时刷新 spinBox 为当前系统时间 */
    void showEvent(QShowEvent *event) override;

private slots:
    void on_pBtn_ModifyTime_clicked();

    void on_pBtn_ModifyDate_clicked();

    void on_pBtn_SwitchGif_clicked();

    void on_pushButton_8_clicked();

    void on_btn_connect_wifi_clicked();

    void on_btn_wifi_on_clicked();

    void on_btn_scan_network_clicked();

    void on_timer_updateTime();


    void on_pBtn_PauseGif_clicked(bool checked);


private:
    Ui::SettingWindow *ui;
    QString LocalGifPath="/MyGif";  /* 存放 GIF 文件的相对路径 */
    QMovie *movie = new QMovie;     /* 用于播放 GIF 动画 */
    QVector<QString> gif_Files;     /* 保存扫描到的 GIF 文件路径列表 */
    int currentGifIndex=0;          /* 当前播放的 GIF 索引 */
    int GifSum=0;                   /* GIF 文件总数 */
    QTimer *timer;                  /* 定时刷新日期时间的定时器 */
    QProcess process;               /* 用于执行系统命令（date/hwclock/wifi 等） */
};

#endif // SETTINGWINDOW_H
