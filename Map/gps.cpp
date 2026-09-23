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
static double jingdu = 113.3800;
static double weidu = 34.3500;

pthread_t threat_id;

int gps_init()
{
    // 防止重复初始化
    if (gps_fd >= 0) {
        return 0; // 已经打开过了，直接返回
    }

    gps_device = (char*)"/dev/ttyS7";
    qDebug() << "gps模块开始初始化";

#if defined(__arm__) || defined(__aarch64__)
    uart_cfg_t cfg = {0};
    gps_valid = 0;
    pthread_rwlock_init(&gps_rwlock, NULL);
    cfg.parity = 'N';
    cfg.dbit = 8;
    cfg.sbit = 1;
    cfg.baudrate = 9600;

    gps_fd = uart_init(gps_device);
    if (0 > gps_fd)
    {
        printf("uart init error \n");
        return -1; // 打开失败直接退出！不要循环重试
    }

    if (uart_cfg(&cfg, gps_fd)) {
        close(gps_fd);
        gps_fd = -1; // 重置文件描述符
        printf("uart cfg error \n");
        return -1;
    }
#endif

    saveData_location = 0;
    OneFramStart = 0;
    create_gps_thread();
    return 0;
}

/*  被分析原始数据 函数调用，用于获取下一个逗号的位置  */
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
/*  被外界访问 获取经纬度数据  */
int getGpsData(char *N_S_Flag_p,char *E_W_Flag_p,double *jingdu_p,double *weidu_p)
{
    pthread_rwlock_rdlock(&gps_rwlock);
    *N_S_Flag_p = N_S_Flag;
    *E_W_Flag_p = E_W_Flag;
    *jingdu_p = jingdu;
    *weidu_p = weidu;
    pthread_rwlock_unlock(&gps_rwlock);
}

/*  被回调函数调用 ，用于分析原始数据，得到经纬度数据 */
int analyseRawData()
{
    char N_S_Flag_local;
    char E_W_Flag_local;
    double jingdu_local;
    double weidu_local;
    unsigned char uc_pos_x = 0;
    uc_pos_x = get_pos_dot(dataBuf,3);
    char num_buf[10];
    if(17>=uc_pos_x)
    {
        gps_valid = 0;
        return -1;
    }
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

    pthread_rwlock_wrlock(&gps_rwlock);
    N_S_Flag = N_S_Flag_local;
    E_W_Flag = E_W_Flag_local;
    jingdu = jingdu_local;
    weidu = weidu_local;
    gps_valid  = 1;
    pthread_rwlock_unlock(&gps_rwlock);
    return 0;
}

/*   线程回调函数  ，不断刷新获取GPS数据  */
static void * gps_thread_func(void* param)
{
    char c;
    while(1)
    {
#if defined(__arm__) || defined(__aarch64__)
        int n = read(gps_fd, &c, 1);
        if (n == 1) {
            if (c == '$') {
                /* 一帧开始：清空缓冲区 */
                OneFramStart = 1;
                saveData_location = 0;
                memset(dataBuf, 0, sizeof(dataBuf));
            } else if (OneFramStart) {
                if (c == '\n') {
                    /* 一帧结束：收尾并解析 */
                    OneFramStart = 0;
                    dataBuf[saveData_location] = '\0';
                    if (dataBuf[3] == 'G') {   /* 只解析 GGA 语句 */
                        analyseRawData();
                    }
                } else if (saveData_location < sizeof(dataBuf) - 1) {
                    dataBuf[saveData_location++] = c;
                }
            }
        }
        usleep(10000);   /* 没数据时短暂等待，别太长 */
#else
        gps_valid = 0;
        usleep(50000);
#endif
    }
}


/*  创建一个线程，实时更新GPS数据 **/
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


//static void read_data_handle(int sig,siginfo_t *info, void *context)
//{
//    char c;
//    int ret;
//    if(SIGRTMIN != sig)
//        return;
//    if (POLL_IN == info->si_code) {
//        ret = read(gps_fd, &c, 1);
//        if(1==OneFramStart)
//        {
//            if(0x0a==c)
//            {
//                OneFramStart = 0;
//                dataBuf[saveData_location++]='\0';
//                //printf("Read One Frame:%s\n",dataBuf);
//                goto handle_one_frame_data;
//            }
//            dataBuf[saveData_location]=c;
//            saveData_location++;
//            return ;
//        }
//        if('$'==c)
//        {
//            OneFramStart = 1;
//            memset(dataBuf, 0, sizeof(dataBuf));
//            saveData_location = 0;
//            return ;
//        }
//    }
//handle_one_frame_data:
//    if('G' == dataBuf[3])
//    {
//        analyseRawData();
//        return ;
//    }
//}


//int set_gps_async_update()
//{
//    struct sigaction sigatn;
//    int flag;


//    flag = fcntl(gps_fd, F_GETFL);
//    flag |= O_ASYNC;
//    fcntl(gps_fd, F_SETFL, flag);


//    fcntl(gps_fd, F_SETOWN, getpid());


//    fcntl(gps_fd, F_SETSIG, SIGRTMIN);


//    sigatn.sa_sigaction = read_data_handle;
//    sigatn.sa_flags = SA_SIGINFO;
//    sigemptyset(&sigatn.sa_mask);
//    sigaction(SIGRTMIN, &sigatn, NULL);
//}

//int main()
//{
//	gps_init();
//	while(1)
//	{
//		printf("********************************\n");
//		printf("%c-%4.4f | %c-%4.4f\n",N_S_Flag,weidu,E_W_Flag,jingdu);
//		printf("********************************\n");
//		usleep(1000000);
//	}
//}

