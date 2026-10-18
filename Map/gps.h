/**
 * @file        gps.h
 * @brief       GPS 数据解析模块头文件
 * @note        通过串口读取 GPS 模块的 NMEA 数据，解析出经纬度信息，
 *              供地图定位使用。
 */
#ifndef GPS_H
#define GPS_H

#ifdef __cplusplus
extern "C"{
#endif

/* 原始数据缓冲区 */
static unsigned char dataBuf[1024];
/* 一帧数据接收标志 */
static int OneFramFlag;
/* 一帧数据接收开始标志（收到 '$' 后置 1） */
static int OneFramStart = 0;
/* 当前数据保存位置 */
static int saveData_location = 0;

/* GPS 串口文件描述符 */
extern int gps_fd;
/* GPS 串口设备节点 */
extern char *gps_device;
/* GPS 定位有效标志 */
extern int gps_valid;

/* 创建 GPS 解析线程 */
int create_gps_thread();

/**
 * @brief  初始化 GPS 模块（配置串口并创建解析线程）
 * @return 成功返回 0，失败返回 -1
 */
int gps_init();

/**
 * @brief  分析原始 NMEA 数据，解析出经纬度
 * @return 成功返回 0，失败返回 -1
 */
int analyseRawData();

/**
 * @brief  获取解析出的经纬度数据（供外部模块调用）
 * @param  N_S_Flag 输出：纬度南北半球标志（N/S）
 * @param  E_W_Flag 输出：经度东西半球标志（E/W）
 * @param  jingdu   输出：经度
 * @param  weidu    输出：纬度
 * @return 成功返回 0
 */
int getGpsData(char *N_S_Flag, char *E_W_Flag, double *jingdu, double *weidu);

#ifdef __cplusplus
}
#endif

#endif
