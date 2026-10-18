/**
 * @file        ap3216.h
 * @brief       AP3216C 环境光/距离传感器类声明
 * @note        通过读取 sysfs 节点获取 ALS（环境光）、PS（距离）、IR（红外）数据。
 */
#ifndef AP3216_H
#define AP3216_H

#include <QObject>
#include <QTimer>

/**
 * @brief AP3216C 传感器类：读取 sysfs 节点获取 ALS/PS/IR 传感器数据
 */
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
    /* 各传感器数据的 getter 接口 */
    QString alsData();
    QString psData();
    QString irData();

    /* 直接从 sysfs 节点读取数据的接口 */
    QString readAlsData();
    QString readPsData();
    QString readIrData();
};

#endif // AP3216_H
