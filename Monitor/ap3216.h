/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 1990-2030. All rights reserved.
* @projectName   sensor
* @brief         ap3216c.h  AP3216C 传感器类声明
* @author        Deng Zhimao
* @email         1252699831@qq.com
* @date          2020-07-10
*******************************************************************/
#ifndef AP3216_H
#define AP3216_H

#include <QObject>
#include <QTimer>

/* AP3216C 传感器类：通过读取 sysfs 节点获取 ALS/PS/IR 传感器数据 */
class Ap3216c : public QObject
{
    Q_OBJECT

public:
    explicit Ap3216c(QObject *parent = 0);
    ~Ap3216c();

private:
    QString alsdata;    /* 环境光强度数据 */
    QString psdata;     /* 接近/距离数据 */
    QString irdata;     /* 红外数据 */
public:
    /* 以下为各传感器数据的 getter 接口 */
    QString alsData();
    QString psData();
    QString irData();

    /* 以下为直接从 sysfs 节点读取数据的接口 */
    QString readAlsData();
    QString readPsData();
    QString readIrData();


};

#endif // AP3216_H
