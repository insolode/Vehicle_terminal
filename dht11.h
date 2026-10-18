/**
 * @file        dht11.h
 * @brief       DHT11 温湿度传感器线程类声明
 * @note        在独立线程中周期读取 DHT11 数据，并通过信号上报温湿度。
 */
#ifndef DHT11_H
#define DHT11_H

#include <QObject>
#include <QThread>
#include <QFile>
#include <QDebug>

/**
 * @brief DHT11 温湿度采集线程类
 */
class Dht11 : public QThread
{
    Q_OBJECT

public:
    Dht11();

protected:
    /* 线程执行体：周期读取温湿度数据并发出信号 */
    void run();

signals:
    /* 温湿度数据更新信号 */
    void updateDht11Data(QString humi, QString temp);

private:
    QFile file;
};

#endif // DHT11_H
