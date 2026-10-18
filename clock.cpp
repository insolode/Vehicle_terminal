/**
 * @file        clock.cpp
 * @brief       表盘时钟控件类实现
 */
#include "clock.h"
#include "ui_clock.h"

/* 构造函数 */
Clock::Clock(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Clock)
{
    ui->setupUi(this);
    setWindowTitle(tr("Clock"));
    ui->setupUi(this);
}

/* 析构函数 */
Clock::~Clock()
{
    delete ui;
}

/* 重绘事件：绘制表盘、刻度和指针 */
void Clock::paintEvent(QPaintEvent *event)
{
    /* 定义时针、分针、秒针以及长/短刻度的坐标 */
    static const QPoint hourHand[3] =
        {
            QPoint(5, 14),
            QPoint(-5, 14),
            QPoint(0, -50)
        };
    static const QPoint minuteHand[3] =
        {
            QPoint(5, 14),
            QPoint(-5,14),
            QPoint(0, -70)
        };
    static const QPoint secondHand[3] =
        {
            QPoint(5, 14),
            QPoint(-5, 14),
            QPoint(0, -80)
        };
    static const QPoint line_long[2]=
        {
            QPoint(0,100),
            QPoint(0,80)
        };
    static const QPoint line_short[2]={
        QPoint(0,100),
        QPoint(0,95)
    };

    /* 绘制的范围 */
    int side = qMin(width(), height());

    /* 获取当前时间 */
    time = QTime::currentTime();

    /* 创建画笔 */
    QPainter painter(this);
    /* 开启反锯齿 */
    painter.setRenderHint(QPainter::Antialiasing);
    /* 把坐标原点平移到窗口中央 */
    painter.translate(width() / 2, height() / 2);
    /* 按窗口边长缩放画布 */
    painter.scale(side / 200.0, side / 200.0);

    /* 绘制时针 */
    painter.setPen( Qt::black );
    painter.setBrush(Qt::black);
    painter.save();
    painter.rotate(30.0 * ((time.hour() + time.minute() / 60.0)));
    painter.drawConvexPolygon(hourHand, 3);
    painter.restore();

    /* 绘制 12 个小时刻度 */
    painter.setPen(Qt::blue);
    for (int i = 0; i < 12; ++i)
    {
        painter.drawLine(line_long[0],line_long[1]);
        painter.rotate(30.0);
    }

    /* 绘制分针转角、刻度 */
    painter.setPen(Qt::green);
    painter.setBrush(Qt::green);
    painter.save();
    painter.rotate(6.0 * (time.minute() + time.second() / 60.0));
    painter.drawConvexPolygon(minuteHand, 3);
    painter.restore();
    painter.setPen(Qt::green);
    for (int j = 0; j < 60; ++j)
    {
        if ((j % 5) != 0)
            painter.drawLine(line_short[0],line_short[1]);
        painter.rotate(6.0);
    }

    /* 绘制分针转角、刻度 */
    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    painter.save();
    painter.rotate(6.0 * (time.minute() + time.second() / 60.0));
    painter.drawConvexPolygon(minuteHand, 3);
    painter.restore();
    painter.setPen(Qt::black);
    for (int j = 0; j < 60; ++j)
    {
        if ((j % 5) != 0)
            painter.drawLine(line_short[0],line_short[1]);
        painter.rotate(6.0);
    }

    /* 绘制秒针转角 */
    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    painter.save();
    painter.rotate(6.0*time.second());
    painter.drawConvexPolygon(secondHand,3);
    painter.restore();
}
