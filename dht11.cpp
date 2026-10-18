/**
 * @file        dht11.cpp
 * @brief       DHT11 温湿度传感器线程类实现
 */
#include "dht11.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* 构造函数 */
Dht11::Dht11()
{

}

/* 线程执行体：周期读取 DHT11 温湿度数据 */
void Dht11::run()
{
    unsigned char date[5]={0};
    QString humidity ,temp;
    unsigned char tmp=0 ;
    int ret;

#if defined(__arm__) || defined(__aarch64__)
    /* 打开 DHT11 设备节点 */
    int fd = open("/dev/dht11",O_RDWR);
    if(fd<0)
    {
        qDebug()<<"can't open /dev/dht11";
        return;
    }

    while (1) {
        /* 读取 5 字节数据（湿度整数/小数、温度整数/小数、校验和） */
        if((ret = read(fd, date, sizeof(date))) != sizeof(date))
        {
            continue;
        }
        /* 校验：前 4 字节之和应等于第 5 字节 */
        tmp = 0;
        tmp+=date[0];
        tmp+=date[1];
        tmp+=date[2];
        tmp+=date[3];
        if(tmp==date[4])
        {
            humidity = QString("%1.%2").arg(date[0]).arg(date[1]);
            temp = QString("%1.%2").arg(date[2]).arg(date[3]);
        }
        else
        {
            humidity = "error";
            temp = "error";
        }
        emit updateDht11Data(humidity, temp);
        usleep(500000);
    }
#else
    /* 非 ARM 平台：返回固定的模拟数据便于调试 */
    while(1)
    {
        humidity = "23.1";
        temp = "32.5";
        emit updateDht11Data(humidity, temp);
        usleep(500000);
    }
#endif
}
