/**
 * @file        weather.h
 * @brief       天气窗口类声明
 * @note        通过网络请求获取天气数据，解析后显示未来几天的天气信息。
 */
#ifndef WEATHER_H
#define WEATHER_H

#include <QMainWindow>
#include <QTimer>
#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QDebug>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVector>
#include <QFile>
#include <QDate>

namespace Ui {
class Weather;
}

/**
 * @brief 天气窗口类：获取并展示天气预报信息
 */
class Weather : public QMainWindow
{
    Q_OBJECT

public:
    explicit Weather(QWidget *parent = nullptr);
    ~Weather();

    /* 将日期转换为星期 */
    QString TransDataToWeek(QString Data);
    /* 将 "xx-xx" 转换为 "xx月xx日" */
    QString TransDataToMyData(QString Data);
    /* 拼接显示昼夜温度 */
    QString TransTempToStr(QString tmpDay, QString tmpNight);
    /* 根据天气描述选择对应图片 */
    QString SelectWeatherImg(QString WeatherDes);
    /* 重新请求并更新天气信息 */
    void updateInfo();

private slots:
    /* 处理天气数据的网络响应 */
    void getWeatherInfo(QNetworkReply *reply);
    /* 刷新按钮 */
    void on_pushButton_clicked();
    /* 返回按钮 */
    void on_pushButton_2_clicked();

private:
    Ui::Weather *ui;
    QNetworkAccessManager netManager;   /* 网络管理器 */
    QNetworkRequest request;            /* 网络请求对象 */
    QList<QString> weather_data;        /* 日期列表 */
    QList<QString> weather_wea;         /* 天气描述列表 */
    QList<QString> weather_wea_img;     /* 天气图标列表 */
    QList<QString> weather_tem_day;     /* 白天温度列表 */
    QList<QString> weather_tem_night;   /* 夜间温度列表 */
    QList<QString> weather_win;         /* 风向列表 */
    QList<QString> weather_win_speed;   /* 风力列表 */
    QString cityName;                   /* 城市名 */
    QString UpdateTime;                 /* 更新时间 */
};

#endif // WEATHER_H
