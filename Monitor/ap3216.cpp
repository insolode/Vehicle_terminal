/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 1990-2030. All rights reserved.
* @projectName   sensor
* @brief         ap3216c.cpp  AP3216C 传感器类实现
* @author        Deng Zhimao
* @email         1252699831@qq.com
* @date          2020-07-10
*******************************************************************/
#include "ap3216.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <QDebug>

/* AP3216C 构造函数 */
Ap3216c::Ap3216c(QObject *parent) : QObject (parent)
{

}

/* AP3216C 析构函数 */
Ap3216c::~Ap3216c()
{

}

/* 读取红外（IR）传感器数据 */
QString Ap3216c::readIrData()
{
#if defined(__arm__) || defined(__aarch64__)
    /* 红外数据对应的 sysfs 节点路径 */
    char const *filename = "/sys/class/misc/ap3216c/ir";
    int err = 0;
    int fd;
    char buf[10];

    /* 只读方式打开节点 */
    fd = open(filename, O_RDONLY);
    if(fd < 0){
        close(fd);
        return "open file error!";
    }

    /* 读取数据 */
    err = read(fd, buf, sizeof(buf));
    if (err < 0){
        close(fd);
        return "read data error!";
    }
    close(fd);

    /* 将读取的字符串按换行分割，取第一行 */
    QString adcValue = buf;
    QStringList list = adcValue.split("\n");
    return list[0];
#else
    /* 非 ARM 平台返回 "No" */
    return "No";
#endif
}

/* 读取接近（PS）传感器数据 */
QString Ap3216c::readPsData()
{
#if defined(__arm__) || defined(__aarch64__)
    /* 接近数据对应的 sysfs 节点路径 */
    char const *filename = "/sys/class/misc/ap3216c/ps";
    int err = 0;
    int fd;
    char buf[10];

    fd = open(filename, O_RDONLY);
    if(fd < 0){
        close(fd);
        return "open file error!";
    }

    err = read(fd, buf, sizeof(buf));
    if (err < 0){
        close(fd);
        return "read data error!";
    }
    close(fd);

    QString adcValue = buf;
    QStringList list = adcValue.split("\n");
    return list[0];
#else
    return "No";
#endif
}

/* 读取环境光强度（ALS）传感器数据 */
QString Ap3216c::readAlsData()
{
#if defined(__arm__) || defined(__aarch64__)
    /* 环境光数据对应的 sysfs 节点路径 */
    char const *filename = "/sys/class/misc/ap3216c/als";
    int err = 0;
    int fd;
    char buf[10];

    fd = open(filename, O_RDONLY);
    if(fd < 0){
        close(fd);
        return "open file error!";
    }

    err = read(fd, buf, sizeof(buf));
    if (err < 0){
        close(fd);
        return "read data error!";
    }
    close(fd);

    QString adcValue = buf;
    QStringList list = adcValue.split("\n");
    return list[0];
#else
    return "No";
#endif
}

/* 返回环境光强度数据成员变量 */
QString Ap3216c::alsData()
{
    return alsdata;
}

/* 返回红外数据成员变量 */
QString Ap3216c::irData()
{
    return irdata;
}

/* 返回接近数据成员变量 */
QString Ap3216c::psData()
{
    return psdata;
}
