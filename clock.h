/**
 * @file        clock.h
 * @brief       表盘时钟控件类声明
 * @note        自绘一个模拟表盘时钟，显示在车载终端主界面上。
 */
#ifndef CLOCK_H
#define CLOCK_H

#include <QMainWindow>
#include<math.h>
#include<QResizeEvent>
#include<QGroupBox>
#include<QTimer>
#include<QTime>
#include<QPainter>

namespace Ui {
class Clock;
}

/**
 * @brief 表盘时钟控件类
 */
class Clock : public QMainWindow
{
    Q_OBJECT

public:
    explicit Clock(QWidget *parent = 0);
    ~Clock();

protected:
    /* 重绘事件：绘制表盘与指针 */
    void paintEvent(QPaintEvent *event);

private:
    Ui::Clock *ui;
    QTime time;   /* 当前时间 */
};

#endif // CLOCK_H
