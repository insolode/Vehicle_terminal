/**
 * @file        monitor.h
 * @brief       倒车影像监控窗口类声明
 * @note        通过 V4L2 摄像头采集图像显示在界面上，并读取 AP3216C 传感器
 *              测量前方距离，实现倒车监控功能。
 */
#ifndef MONITOR_H
#define MONITOR_H

#include <QMainWindow>
#include <QImage>
#include <QTimer>

#include "capture_thread.h"
#include "ap3216.h"

/* 监控模块命令定义（用于语音控制） */
#define MONITOR_COMMAND_CLOSE 0   /* 关闭监控 */
#define MONITOR_COMMAND_SHOW 1    /* 显示监控 */
namespace Ui {
class Monitor;
}

/* 倒车影像监控窗口类（单例模式） */
class Monitor : public QMainWindow
{
    Q_OBJECT
public:
    /* 获取单例对象 */
    static Monitor* getInstance();
private:
    /* 私有构造函数（单例） */
    Monitor(QWidget *parent = nullptr);
    static Monitor* monitor;   /* 单例对象指针 */
    ~Monitor();

private:
    Ui::Monitor *ui;
    QTimer *DisUpdate_Timer;      /* 定时更新距离显示的定时器 */
    CaptureThread *captureThread; /* 摄像头采集线程 */
    Ap3216c *ap3216;              /* AP3216C 距离/光照传感器 */

public:
    /* 启动监控（开始采集） */
    void myStart();
    /* 停止监控 */
    void myStop();
signals:
    /* 监控窗口关闭，通知主界面恢复显示 */
    void closed();
public slots:
    /* 显示采集到的图像 */
    void showImage(QImage);
    /* 定时器超时更新距离显示 */
    void on_timer_timeout();
    /* 处理语音控制传来的指令 */
    void on_handleCommand(int);
private slots:
    /* 点击关闭按钮：停止采集、关闭窗口并回到主界面 */
    void on_pBtn_Close_clicked();
};

#endif // MONITOR_H
