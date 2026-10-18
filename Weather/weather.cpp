/**
 * @file        weather.cpp
 * @brief       天气窗口类实现
 */
#include "weather.h"
#include "ui_weather.h"

Weather::Weather(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Weather)
{
    ui->setupUi(this);

    /* 读取并应用天气窗口样式表 */
    QFile file(":/resources/weather.qss");   // ":/" 表示从 Qt 资源文件读取
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString style = stream.readAll();
        file.close();
        this->setStyleSheet(style);
    }

    /* 发起天气数据网络请求（appid/appsecret 需替换为你自己的心知天气密钥） */
    request.setUrl(QUrl("http://v1.yiketianqi.com/free/week?appid=YOUR_APPID&appsecret=YOUR_APPSECRET&unescape=1"));
    netManager.get(request);
    connect(&netManager,SIGNAL(finished(QNetworkReply *)),this,SLOT(getWeatherInfo(QNetworkReply *)));

    ui->groupBox_Total->setObjectName("MainGroupBox");
//    ui->groupBox_Total->setStyleSheet("#MainGroupBox{background-image:url(':/weather/Weather/images/UI2.png');}");
}

Weather::~Weather()
{
    delete ui;
}

/* 将日期转换为星期 */
QString Weather::TransDataToWeek(QString Data)
{
    QDate date;
    date =  QDate::fromString(Data,"yyyy-MM-dd");
    int week = date.dayOfWeek();
    switch (week) {
    case 1:
        return "星期一";
    case 2:
        return "星期二";
    case 3:
        return "星期三";
    case 4:
        return "星期四";
    case 5:
        return "星期五";
    case 6:
        return "星期六";
    case 7:
        return "星期七";
    default:
        return "星期零";
    }
}

/* 将 "xx-xx" 转换为 "xx月xx日" */
QString Weather::TransDataToMyData(QString Data)
{
    QString MM = Data.split("-").at(1);
    QString DD = Data.split("-").at(2);
    return QString("%1月%2日").arg(MM).arg(DD);
}

/* 拼接显示昼夜温度 */
QString Weather::TransTempToStr(QString tmpDay, QString tmpNight)
{
    return QString("%1~%2 ℃").arg(tmpDay).arg(tmpNight);
}

/* 根据天气描述选择对应图片 */
QString Weather::SelectWeatherImg(QString WeatherDes)
{
    return QString(":/Weather/images/%1.png").arg(WeatherDes);
}

/* 重新请求并更新天气信息 */
void Weather::updateInfo()
{
    netManager.get(request);
}

/* 处理天气数据网络响应：解析并显示 */
void Weather::getWeatherInfo(QNetworkReply *reply)
{
    QByteArray bytes = reply->readAll();
    int num = bytes.size();

    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if(!doc.isObject()){
        qDebug()<<"Netjson not an jsonObject!";
        QMessageBox::warning(this,"Warning","获取数据失败！请检查网络");
        this->hide();
        return;
    }
    if(num<60)
    {
        QMessageBox::warning(this,"Warning","天气API过期，请更新API");
        return;
    }

    QJsonObject obj = doc.object();
    cityName = obj.value("city").toString();
    UpdateTime = obj.value("update_time").toString();
    QJsonArray arrays = obj.value("data").toArray();

    /* 检查数组长度，数据不足时直接返回 */
    int arraySize = arrays.size();
    if(arraySize < 4) {
        QMessageBox::warning(this,"Warning","返回数据不足！");
        return;
    }

    /* 循环前先清空旧数据，避免重复追加 */
    weather_data.clear();
    weather_wea.clear();
    weather_tem_day.clear();
    weather_tem_night.clear();
    weather_win.clear();
    weather_win_speed.clear();
    weather_wea_img.clear();

    /* 根据实际数组长度解析，最多取 7 天 */
    int count = qMin(arraySize, 7);
    for(int i=0; i<count; i++)
    {
        weather_data.append(arrays[i].toObject().value("date").toString());
        weather_wea.append(arrays[i].toObject().value("wea").toString());
        weather_tem_day.append(arrays[i].toObject().value("tem_day").toString());
        weather_tem_night.append(arrays[i].toObject().value("tem_night").toString());
        weather_win.append(arrays[i].toObject().value("win").toString());
        weather_win_speed.append(arrays[i].toObject().value("win_speed").toString());
        weather_wea_img.append(arrays[i].toObject().value("wea_img").toString());
    }
    qDebug()<< "天气数据解析完成" <<endl;

    /* 确保有足够数据再更新界面 */
    if(count < 5) {
        QMessageBox::warning(this,"Warning","数据不足5天，显示可能不完整！");
    }

    if(weather_data.size() > 4)
    {
        qDebug()<< "天气数据写入界面中" <<endl;

        /* 逐天更新日期、星期、天气描述、风向、风力、温度显示 */
        ui->label_day1_data->setText(TransDataToMyData(weather_data.at(1)));
        ui->label_day2_data->setText(TransDataToMyData(weather_data.at(2)));
        ui->label_day3_data->setText(TransDataToMyData(weather_data.at(3)));
        ui->label_day4_data->setText(TransDataToMyData(weather_data.at(4)));

        ui->label_day1_week->setText(TransDataToWeek(weather_data.at(1)));
        ui->label_day2_week->setText(TransDataToWeek(weather_data.at(2)));
        ui->label_day3_week->setText(TransDataToWeek(weather_data.at(3)));
        ui->label_day4_week->setText(TransDataToWeek(weather_data.at(4)));

        ui->label_day1_des->setText(weather_wea.at(1));
        ui->label_day2_des->setText(weather_wea.at(2));
        ui->label_day3_des->setText(weather_wea.at(3));
        ui->label_day4_des->setText(weather_wea.at(4));

        ui->label_day1_win->setText(weather_win.at(1));
        ui->label_day2_win->setText(weather_win.at(2));
        ui->label_day3_win->setText(weather_win.at(3));
        ui->label_day4_win->setText(weather_win.at(4));

        ui->label_day1_winSpeed->setText(weather_win_speed.at(1));
        ui->label_day2_winSpeed->setText(weather_win_speed.at(2));
        ui->label_day3_winSpeed->setText(weather_win_speed.at(3));
        ui->label_day4_winSpeed->setText(weather_win_speed.at(4));

        ui->label_day1_tmp->setText(TransTempToStr(weather_tem_day.at(1),weather_tem_night.at(1)));
        ui->label_day2_tmp->setText(TransTempToStr(weather_tem_day.at(2),weather_tem_night.at(2)));
        ui->label_day3_tmp->setText(TransTempToStr(weather_tem_day.at(3),weather_tem_night.at(3)));
        ui->label_day4_tmp->setText(TransTempToStr(weather_tem_day.at(4),weather_tem_night.at(4)));

        /* 显示当前（今天）的天气详情 */
        QPixmap pixmap;
        ui->label_now_img->setScaledContents(true);
        pixmap.load(SelectWeatherImg(weather_wea_img.at(0)));
        ui->label_now_img->setPixmap(pixmap);
        ui->label_now_des->setText(weather_wea.at(0));
        ui->label_now_win->setText(weather_win.at(0));
        ui->label_now_winSpeed->setText(weather_win_speed.at(0));
        ui->label_now_tmp->setText(weather_tem_day.at(0));

        ui->label_Time->setText(UpdateTime);
        ui->label_location->setText(cityName);
        qDebug()<< "天气数据写入完成" <<endl;
    }
}

/* 点击刷新按钮：重新获取天气信息 */
void Weather::on_pushButton_clicked()
{
    updateInfo();
}

/* 点击返回按钮：关闭天气窗口并回到主界面 */
void Weather::on_pushButton_2_clicked()
{
    if (parentWidget()) {
        parentWidget()->show();
    }
    this->hide();
}
