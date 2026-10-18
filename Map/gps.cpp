/**
 * @file        gps.cpp
 * @brief       GPS 数据解析模块实现
 */
#define _GNU_SOURCE
#include "gps.h"
#include "uart.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <linux/types.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <QDebug>

int gps_fd = -1;
char* gps_device = (char*)"/dev/ttyS7";
int gps_valid = 0;

pthread_rwlock_t gps_rwlock;
static char N_S_Flag = 'N';
static char E_W_Flag = 'E';
static double jingdu = 116.4074;    // 默认经纬度为北京
static double weidu = 39.9042;

pthread_t threat_id;

/**
 * @brief 初始化 GPS 模块
 * @return 成功返回 0，失败返回 -1
 */
int gps_init()
{
    /* 防止重复初始化 */
    if (gps_fd >= 0) {
        return 0;   // 已经打开过，直接返回
    }

    gps_device = (char*)"/dev/ttyS7";
    qDebug() << "gps模块开始初始化";

#if defined(__arm__) || defined(__aarch64__)
    /* 配置串口参数：9600 波特率、8 数据位、无校验、1 停止位 */
    uart_cfg_t cfg = {0};
    gps_valid = 0;
    pthread_rwlock_init(&gps_rwlock, NULL);
    cfg.parity = 'N';
    cfg.dbit = 8;
    cfg.sbit = 1;
    cfg.baudrate = 9600;

    /* 打开并配置串口设备 */
    gps_fd = uart_init(gps_device);
    if (0 > gps_fd)
    {
        printf("uart init error \n");
        return -1;  // 打开失败直接退出
    }

    if (uart_cfg(&cfg, gps_fd)) {
        close(gps_fd);
        gps_fd = -1;    // 配置失败，重置文件描述符
        printf("uart cfg error \n");
        return -1;
    }
#endif

    saveData_location = 0;
    OneFramStart = 0;
    create_gps_thread();
    return 0;
}

/**
 * @brief  获取第 uc_cnt 个逗号之后的偏移位置（用于拆分 NMEA 字段）
 * @param  buf    原始数据缓冲区
 * @param  uc_cnt 要定位的逗号序号
 * @return 该逗号之后的缓冲区偏移
 */
int get_pos_dot(unsigned char *buf, unsigned char uc_cnt )
{
    unsigned char *buf_x;
    buf_x = buf;
    while(uc_cnt)
    {
        if(*buf_x==',')
            uc_cnt--;
        buf_x++;
    }
    return buf_x-buf;
}

/**
 * @brief  供外部模块获取解析出的经纬度数据
 * @param  N_S_Flag_p 输出：纬度南北半球标志
 * @param  E_W_Flag_p 输出：经度东西半球标志
 * @param  jingdu_p   输出：经度
 * @param  weidu_p    输出：纬度
 * @return 成功返回 0
 */
int getGpsData(char *N_S_Flag_p,char *E_W_Flag_p,double *jingdu_p,double *weidu_p)
{
    pthread_rwlock_rdlock(&gps_rwlock);
    *N_S_Flag_p = N_S_Flag;
    *E_W_Flag_p = E_W_Flag;
    *jingdu_p = jingdu;
    *weidu_p = weidu;
    pthread_rwlock_unlock(&gps_rwlock);
    return 0;
}

/**
 * @brief  分析原始 NMEA 数据，解析出经纬度
 * @return 成功返回 0，数据无效返回 -1
 */
int analyseRawData()
{
    char N_S_Flag_local;
    char E_W_Flag_local;
    double jingdu_local;
    double weidu_local;
    unsigned char uc_pos_x = 0;
    /* 先判断语句类型，长度不足说明不是 GGA 语句，直接丢弃 */
    uc_pos_x = get_pos_dot(dataBuf,3);
    char num_buf[10];
    if(17>=uc_pos_x)
    {
        gps_valid = 0;
        return -1;
    }
    /* 依次解析纬度、南北标志、经度、东西标志 */
    uc_pos_x = get_pos_dot(dataBuf,2);
    memcpy(num_buf,&dataBuf[uc_pos_x],9);
    num_buf[9]='\0';
    weidu_local = strtod(num_buf,NULL);
    weidu_local/=100;
    uc_pos_x = get_pos_dot(dataBuf,3);
    N_S_Flag_local = dataBuf[uc_pos_x];
    uc_pos_x = get_pos_dot(dataBuf,4);
    memcpy(num_buf,&dataBuf[uc_pos_x],9);
    num_buf[9]='\0';
    jingdu_local = strtod(num_buf,NULL);
    jingdu_local/=100;
    uc_pos_x = get_pos_dot(dataBuf,5);
    E_W_Flag_local = dataBuf[uc_pos_x];

    /* 写锁保护，更新全局经纬度数据 */
    pthread_rwlock_wrlock(&gps_rwlock);
    N_S_Flag = N_S_Flag_local;
    E_W_Flag = E_W_Flag_local;
    jingdu = jingdu_local;
    weidu = weidu_local;
    gps_valid  = 1;
    pthread_rwlock_unlock(&gps_rwlock);
    return 0;
}

/**
 * @brief GPS 解析线程执行体：不断从串口读取并解析数据
 */
static void * gps_thread_func(void* param)
{
    char c;
    while(1)
    {
#if defined(__arm__) || defined(__aarch64__)
        int n = read(gps_fd, &c, 1);
        if (n == 1) {
            if (c == '$') {
                /* 收到 '$'：一帧开始，清空缓冲区 */
                OneFramStart = 1;
                saveData_location = 0;
                memset(dataBuf, 0, sizeof(dataBuf));
            }
            else if (OneFramStart) {
                if (c == '\n') {
                    /* 收到换行：一帧结束，收尾并解析 */
                    OneFramStart = 0;
                    dataBuf[saveData_location] = '\0';
                    if (dataBuf[3] == 'G') {   /* 只解析 GGA 语句 */
                        analyseRawData();
                    }
                }
                else if (saveData_location < sizeof(dataBuf) - 1) {
                    dataBuf[saveData_location++] = c;
                }
            }
        }
        usleep(10000);   /* 没数据时短暂等待，避免空转 */
#else
        /* 非 ARM 平台直接置为无效 */
        gps_valid = 0;
        usleep(50000);
#endif
    }
}

/**
 * @brief 创建 GPS 解析线程
 * @return 成功返回 0，失败返回 -1
 */
int create_gps_thread()
{
    int err;
    err = pthread_create(&threat_id,NULL,gps_thread_func,NULL);
    if(err!=0)
    {
        printf("Can't create gps thread\n");
        return -1;
    }
    return 0;
}
