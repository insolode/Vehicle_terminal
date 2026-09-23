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
/* 数据保存位置 */
static int saveData_location=0;

extern int gps_fd;
extern char* gps_device;
extern int gps_valid;


int create_gps_thread();

/* 初始化 GPS（配置串口并创建解析线程） */
int gps_init();
//int set_gps_async_update();
/* 分析原始数据，解析经纬度 */
int analyseRawData();
/* 获取 GPS 经纬度数据（供外部模块调用） */
int getGpsData(char* N_S_Flag,char *E_W_Flag,double *jingdu,double *weidu);
#ifdef __cplusplus
}
#endif

#endif
