/**
 * @file        uart.h
 * @brief       串口操作模块头文件
 * @note        封装 Linux 串口的初始化与配置操作。
 */
#ifndef UART_H
#define UART_H
#ifdef __cplusplus
extern "C"{
#endif



/* 串口硬件配置结构体 */
typedef struct uart_hardware_cfg {
    unsigned int baudrate;      /* 波特率     */
    unsigned char dbit;         /* 数据位     */
    char parity;                /* 奇偶校验 */
    unsigned char sbit;         /* 停止位 */
} uart_cfg_t;

/* 打开并初始化串口设备，返回文件描述符 */
int uart_init(const char *device);
/* 配置串口参数（波特率、数据位、校验位、停止位） */
int uart_cfg(const uart_cfg_t *cfg,int fd);
#ifdef __cplusplus
}
#endif
#endif
