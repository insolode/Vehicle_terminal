#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFile file(":/resources/mianwindow.qss"); // ":/" 是Qt资源的固定前缀[reference:12]
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString style = stream.readAll();
        file.close();

        // 2. 只给当前 this（即 LoginDialog 本身及其子控件）设置样式
        this->setStyleSheet(style);
    }

    ui->videoVLayout->addWidget(&clock);
    musicPlayer = new MusicPlayer(this);
    weather = new Weather(this);
    baiduMap = new BaiduMap(this);

    time = new QTimer;
    time->setInterval(500);
    time->start();
    dht11 = new Dht11;
    dht11->start();
    monitor = Monitor::getInstance();
    connect(monitor, SIGNAL(closed()), this, SLOT(show()));


    windows.append(baiduMap);
    windows.append(monitor);
    windows.append(weather);
    windows.append(musicPlayer);

    connect(time,SIGNAL(timeout()),this,SLOT(on_timer_updateTime()));
    connect(dht11,SIGNAL(updateDht11Data(QString ,QString )),this,SLOT(on_update_humidity_temp(QString, QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Weather_Button_clicked()
{
    this->hide();          // 隐藏当前主窗口
    weather->show();
}

void MainWindow::on_Music_Button_clicked()
{
    this->hide();          // 隐藏当前主窗口
    musicPlayer->show();
}

void MainWindow::on_Map_Button_clicked()
{
    this->hide();          // 隐藏当前主窗口
    baiduMap->show();
}

void MainWindow::on_Camera_Button_clicked()
{
    this->hide();          // 隐藏当前主窗口
    monitor->show();
    monitor->myStart();
}

void MainWindow::on_update_humidity_temp(QString humidity, QString temp)
{
    ui->label_humi->setText(humidity);
    ui->label_temp->setText(temp);
}


void MainWindow::on_timer_updateTime()
{
    QString time = QTime::currentTime().toString("hh:mm:ss");
    QString date = QDate::currentDate().toString("yyyy-MM-dd");
    ui->label_Time->setText(time);
    ui->label_Date->setText(date);
}



void MainWindow::on_pBtn_Setting_clicked()
{
    settingWindow.ScanGif();
    settingWindow.show();
}

