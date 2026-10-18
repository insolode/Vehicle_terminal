/**
 * @file        mainwindow.cpp
 * @brief       主窗口类实现
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* 读取并应用主界面样式表 */
    QFile file(":/resources/mianwindow.qss");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString style = stream.readAll();
        file.close();
        this->setStyleSheet(style);
    }

    /* 将表盘时钟控件加入主界面布局 */
    ui->videoVLayout->addWidget(&clock);

    /* 创建音乐、天气、地图等子窗口 */
    musicPlayer = new MusicPlayer(this);
    weather = new Weather(this);
    baiduMap = new BaiduMap(this);

    /* 创建时间刷新定时器，每 500ms 触发一次 */
    time = new QTimer;
    time->setInterval(500);
    time->start();

    /* 启动 DHT11 温湿度采集线程 */
    dht11 = new Dht11;
    dht11->start();

    /* 获取倒车影像单例，监控窗口关闭时恢复主界面 */
    monitor = Monitor::getInstance();
    connect(monitor, SIGNAL(closed()), this, SLOT(show()));

    /* 收集所有子窗口 */
    windows.append(baiduMap);
    windows.append(monitor);
    windows.append(weather);
    windows.append(musicPlayer);

    /* 连接定时器与温湿度数据刷新的信号槽 */
    connect(time,SIGNAL(timeout()),this,SLOT(on_timer_updateTime()));
    connect(dht11,SIGNAL(updateDht11Data(QString ,QString )),this,SLOT(on_update_humidity_temp(QString, QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* 点击天气按钮：隐藏主界面并显示天气窗口 */
void MainWindow::on_Weather_Button_clicked()
{
    this->hide();
    weather->show();
}

/* 点击音乐按钮：隐藏主界面并显示音乐播放器 */
void MainWindow::on_Music_Button_clicked()
{
    this->hide();
    musicPlayer->show();
}

/* 点击地图按钮：隐藏主界面并显示地图窗口 */
void MainWindow::on_Map_Button_clicked()
{
    this->hide();
    baiduMap->show();
}

/* 点击摄像头按钮：隐藏主界面并启动倒车影像 */
void MainWindow::on_Camera_Button_clicked()
{
    this->hide();
    monitor->show();
    monitor->myStart();
}

/* 刷新主界面上的温湿度显示 */
void MainWindow::on_update_humidity_temp(QString humidity, QString temp)
{
    ui->label_humi->setText(humidity);
    ui->label_temp->setText(temp);
}

/* 定时刷新主界面上的时间和日期显示 */
void MainWindow::on_timer_updateTime()
{
    QString time = QTime::currentTime().toString("hh:mm:ss");
    QString date = QDate::currentDate().toString("yyyy-MM-dd");
    ui->label_Time->setText(time);
    ui->label_Date->setText(date);
}

/* 点击设置按钮：打开设置窗口 */
void MainWindow::on_pBtn_Setting_clicked()
{
    settingWindow.ScanGif();
    settingWindow.show();
}
