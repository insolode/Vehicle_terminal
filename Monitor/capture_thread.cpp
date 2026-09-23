/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 2021-2030. All rights reserved.
* @projectName   video_server
* @brief         capture_thread.cpp  摄像头采集线程类实现
* @author        Deng Zhimao
* @email         dengzhimao@alientek.com
* @link          www.openedv.com
* @date          2021-11-19
*******************************************************************/
#include "capture_thread.h"

/* 摄像头采集线程执行体 */
void CaptureThread::run()
{
#ifdef __linux__
#if !defined(__arm__) && !defined(__aarch64__)
    return;
#endif
    int video_fd = -1;      /* 摄像头设备文件描述符 */
    struct v4l2_format fmt; /* 视频格式 */
    struct v4l2_requestbuffers req_bufs; /* 请求缓冲区 */
    static struct v4l2_buffer buf;       /* 视频缓冲区 */
    int n_buf;
    struct buffer_info bufs_info[VIDEO_BUFFER_COUNT]; /* 缓冲区信息数组 */
    enum v4l2_buf_type type;

    /* 打开摄像头设备 */
    video_fd = open(VIDEO_DEV, O_RDWR);
    if (0 > video_fd) {
        printf("ERROR: failed to open video device %s\n", VIDEO_DEV);
        return ;
    }

    /* 设置视频采集格式：USB 摄像头(Q8 HD) 用 MJPEG，1080p */
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 1920;
    fmt.fmt.pix.height = 1080;
    fmt.fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;

    if (0 > ioctl(video_fd, VIDIOC_S_FMT, &fmt)) {
        printf("ERROR: failed to VIDIOC_S_FMT\n");
        close(video_fd);
        return ;
    }

    /* 申请视频缓冲区（使用内存映射 mmap 方式） */
    req_bufs.count = VIDEO_BUFFER_COUNT;
    req_bufs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req_bufs.memory = V4L2_MEMORY_MMAP;

    if (0 > ioctl(video_fd, VIDIOC_REQBUFS, &req_bufs)) {
        printf("ERROR: failed to VIDIOC_REQBUFS\n");
        return ;
    }

    /* 查询并映射缓冲区 */
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    for (n_buf = 0; n_buf < VIDEO_BUFFER_COUNT; n_buf++) {

        buf.index = n_buf;
        if (0 > ioctl(video_fd, VIDIOC_QUERYBUF, &buf)) {
            printf("ERROR: failed to VIDIOC_QUERYBUF\n");
            return ;
        }

        bufs_info[n_buf].length = buf.length;
        bufs_info[n_buf].start = mmap(NULL, buf.length,
                                      PROT_READ | PROT_WRITE, MAP_SHARED,
                                      video_fd, buf.m.offset);
        if (MAP_FAILED == bufs_info[n_buf].start) {
            printf("ERROR: failed to mmap video buffer, size 0x%x\n", buf.length);
            return ;
        }
    }

    /* 将缓冲区放入采集队列 */
    for (n_buf = 0; n_buf < VIDEO_BUFFER_COUNT; n_buf++) {

        buf.index = n_buf;
        if (0 > ioctl(video_fd, VIDIOC_QBUF, &buf)) {
            printf("ERROR: failed to VIDIOC_QBUF\n");
            return ;
        }
    }

    /* 开始视频流采集 */
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (0 > ioctl(video_fd, VIDIOC_STREAMON, &type)) {
        printf("ERROR: failed to VIDIOC_STREAMON\n");
        return ;
    }

    /* 循环采集图像 */
    while (startFlag) {

        /* 从队列中取出采集到的一帧图像（buf.index 由驱动返回实际索引） */
        if (0 > ioctl(video_fd, VIDIOC_DQBUF, &buf)) {
            printf("ERROR: failed to VIDIOC_DQBUF\n");
            break;
        }

        /* MJPEG 数据按实际帧长 bytesused 解码成 QImage */
        QImage qImage = QImage::fromData(
            (const unsigned char*)bufs_info[buf.index].start, (int)buf.bytesused, "JPG");

        /* 是否开启本地显示 */
        if (startLocalDisplay && !qImage.isNull())
            emit imageReady(qImage);

        /* 将缓冲区重新放回队列 */
        if (0 > ioctl(video_fd, VIDIOC_QBUF, &buf)) {
            printf("ERROR: failed to VIDIOC_QBUF\n");
            break;
        }
    }

    msleep(800);//at lease 650
    /* 解除缓冲区映射 */
    for (int i = 0; i < VIDEO_BUFFER_COUNT; i++) {
        munmap(bufs_info[i].start, buf.length);
    }

    /* 关闭摄像头设备 */
    close(video_fd);
#endif
}
