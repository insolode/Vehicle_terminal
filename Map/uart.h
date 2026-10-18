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
    unsigned int baudrate;      /* 波特率   */
    unsigned char dbit;         /* 数据位   */
    char parity;                /* 奇偶校验 */
    unsigned char sbit;         /* 停止位   */
} uart_cfg_t;

/**
 * @brief  打开串口设备
 * @param  device 串口设备节点路径
 * @return 成功返回文件描述符，失败返回 -1
 */
int uart_init(const char *device);

/**
 * @brief  配置串口参数（波特率、数据位、校验位、停止位）
 * @param  cfg 串口配置结构体指针
 * @param  fd  已打开的串口文件描述符
 * @return 成功返回 0，失败返回 -1
 */
int uart_cfg(const uart_cfg_t *cfg, int fd);

#ifdef __cplusplus
}
#endif

#endif
