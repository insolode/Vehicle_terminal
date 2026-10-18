/**
 * @file        capture_thread.h
 * @brief       摄像头采集线程类声明
 * @note        在后台线程中通过 V4L2 采集摄像头图像，供倒车影像显示。
 */
#ifndef CAPTURE_THREAD_H
#define CAPTURE_THREAD_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#ifdef __linux__
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <linux/input.h>
#endif

#include <QThread>
#include <QDebug>
#include <QPushButton>
#include <QImage>
#include <QByteArray>
#include <QBuffer>
#include <QTime>
#include <QUdpSocket>

/* V4L2 摄像头设备节点和帧缓冲设备节点 */
#define VIDEO_DEV           "/dev/video0"
#define FB_DEV              "/dev/fb0"
#define VIDEO_BUFFER_COUNT  3   /* 视频缓冲区数量 */

/* 缓冲区信息结构体：记录缓冲区的起始地址和长度 */
struct buffer_info {
    void *start;
    unsigned int length;
};

/**
 * @brief 摄像头采集线程类：在后台线程中通过 V4L2 采集摄像头图像
 */
class CaptureThread : public QThread
{
    Q_OBJECT

signals:
    /* 图像就绪信号，携带一帧图像 */
    void imageReady(QImage);
    void sendImage(QImage);

private:
    /* 线程运行标志 */
    bool startFlag = false;

    /* 本地显示标志 */
    bool startLocalDisplay = false;
    /* 线程执行体：采集图像并发送 */
    void run() override;

public:
    CaptureThread(QObject *parent = nullptr) {
        Q_UNUSED(parent);
    }

public:
    /* 启动线程（置开始标志并启动线程） */
    void startThread()
    {
        startFlag = true;
        if (!this->isRunning())
            this->start();
    }

    /* 停止线程（清除开始标志并等待线程退出） */
    void stopThread()
    {
        startFlag = false;
        this->wait(2000);   // 等待 run() 退出并完成 munmap/close，避免快速重启竞态
    }

public slots:
    /* 根据传入标志启动或停止线程 */
    void setThreadStart(bool start) {
        startFlag = start;
        if (start) {
            if (!this->isRunning())
                this->start();
        } else {
            this->quit();
        }
    }

    /* 设置是否在本地显示图像 */
    void setLocalDisplay(bool start) {
        startLocalDisplay = start;
    }
};

#endif // CAPTURE_THREAD_H
