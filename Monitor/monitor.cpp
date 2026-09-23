/**
 * @file        monitor.cpp
 * @brief       倒车影像监控窗口类实现
 */
#include "monitor.h"
#include "ui_monitor.h"

Monitor * Monitor::monitor = nullptr;

/* 获取监控窗口单例对象 */
Monitor * Monitor::getInstance()
{
    if(monitor==nullptr)
        monitor = new Monitor();
    return monitor;
}

/* 监控窗口构造函数 */
Monitor::Monitor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Monitor)
{
    ui->setupUi(this);

    /* 创建距离传感器对象 */
    ap3216 = new Ap3216c(this);

    /* 创建摄像头采集线程并开启本地显示 */
    captureThread = new CaptureThread(this);
    captureThread->setLocalDisplay(true);

    /* 创建定时器，每 500ms 更新一次距离显示 */
    DisUpdate_Timer = new QTimer(this);
    DisUpdate_Timer->setInterval(500);

    /* 连接信号槽：图像就绪、定时刷新 */
    connect(captureThread, &CaptureThread::imageReady, this, &Monitor::showImage);
    connect(DisUpdate_Timer, &QTimer::timeout, this, &Monitor::on_timer_timeout);
}

/* 监控窗口析构函数 */
Monitor::~Monitor()
{
    delete ui;
}

/* 显示采集到的图像 */
void Monitor::showImage(QImage image)
{
    ui->videoWidget->setFrame(image);
}

/* 启动监控：开启定时器和摄像头采集线程 */
void Monitor::myStart()
{
    DisUpdate_Timer->start();
    captureThread->startThread();
}

/* 停止监控：停止定时器和摄像头采集线程 */
void Monitor::myStop()
{
    DisUpdate_Timer->stop();
    captureThread->stopThread();
}

/* 点击关闭按钮：停止采集、关闭窗口并回到主界面 */
void Monitor::on_pBtn_Close_clicked()
{
    myStop();
    emit closed();
    this->hide();
}

/* 定时器超时：读取距离传感器数据并更新显示 */
void Monitor::on_timer_timeout()
{
    ui->label_distance->setText(QString("距离: %1").arg(ap3216->readPsData()));
}

/* 处理语音控制传来的指令 */
void Monitor::on_handleCommand(int command)
{
    switch (command) {
    case MONITOR_COMMAND_SHOW:
        this->show();
        break;
    case MONITOR_COMMAND_CLOSE:
        on_pBtn_Close_clicked();
        break;
    default:
        break;
    }
}
