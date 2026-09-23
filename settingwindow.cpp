#include "settingwindow.h"
#include "ui_settingwindow.h"


SettingWindow::SettingWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SettingWindow)
{
    ui->setupUi(this);
    QFile file(":/resources/setting.qss"); // ":/" 是Qt资源的固定前缀[reference:12]
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString style = stream.readAll();
        file.close();

        // 2. 只给当前 this（即 LoginDialog 本身及其子控件）设置样式
        this->setStyleSheet(style);
    }

    /* 配置 GIF 显示标签的大小和缩放 */
    ui->label_ShowGif->setFixedSize(500,300);
    ui->label_ShowGif->setScaledContents(true);
    ui->label_setting_date->setFixedWidth(225);

    /* 创建定时器，每 500ms 刷新一次日期时间显示 */
    timer = new QTimer;
    timer->setInterval(500);
    timer->start();
    connect(timer,SIGNAL(timeout()),this,SLOT(on_timer_updateTime()));
    /* 扫描并加载本地 GIF 文件 */
    ScanGif();
}

SettingWindow::~SettingWindow()
{
    delete ui;
}

/* 遍历本地gif文件  */
void SettingWindow::ScanGif()
{
    /* 定位到应用程序目录下的 /MyGif 文件夹 */
    QDir dir(QCoreApplication::applicationDirPath() + LocalGifPath);
    QDir dirAbsolutePath(dir.absolutePath());
    if(dirAbsolutePath.exists())
    {
        /* 过滤出 *.gif 文件 */
        QStringList filter;
        filter<<"*.gif";
        QFileInfoList files = dirAbsolutePath.entryInfoList(filter,QDir::Files);
        GifSum = files.count();
        gif_Files.clear();
        /* 把所有 GIF 文件的绝对路径存入列表 */
        for(int i=0;i<files.count();i++)
        {
            gif_Files.append(files.at(i).absoluteFilePath());
        }
    }
    /* 目录下没有 GIF 文件时给出提示 */
    if(0==GifSum)
    {
        ui->label_ShowGif->setText("./MyGif 目录下没有可用的Gif文件");
        return;
    }
    /* 加载第一个 GIF 并开始播放 */
    currentGifIndex = 0;
    movie->setFileName(gif_Files.at(0));
    ui->label_ShowGif->setMovie(movie);
    movie->start();
}


/* 读取当前系统时间并显示到 时/分/秒 spinBox */
void SettingWindow::loadCurrentTime()
{
    QTime now = QTime::currentTime();
    ui->spinBox_hour->setValue(now.hour());
    ui->spinBox_Min->setValue(now.minute());
    ui->spinBox_Sec->setValue(now.second());
}

/* 窗口每次显示时，把 spinBox 同步为当前系统时间 */
void SettingWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    loadCurrentTime();
}

void SettingWindow::on_pBtn_ModifyTime_clicked()
{
    /* 补零拼接成 hh:mm:ss，避免 "9:5:3" 这类格式 */
    QString string = QString("%1:%2:%3")
                         .arg(ui->spinBox_hour->value(), 2, 10, QLatin1Char('0'))
                         .arg(ui->spinBox_Min->value(), 2, 10, QLatin1Char('0'))
                         .arg(ui->spinBox_Sec->value(), 2, 10, QLatin1Char('0'));
    qDebug() << string;

    /* 调用 date 命令设置系统时间，等待其执行完成（不能和 hwclock 连续启动） */
    process.start("date", QStringList() << "-s" << string);
    if (!process.waitForFinished(3000)) {
        QMessageBox::warning(this, "错误", "date 命令执行超时");
        return;
    }

    /* 再用 hwclock 把系统时间同步到硬件时钟（RTC） */
    process.start("hwclock", QStringList() << "-w");
    process.waitForFinished(3000);

    process.close();
    QMessageBox::information(this, "提示", "系统时间设置成功");
}


void SettingWindow::on_pBtn_ModifyDate_clicked()
{
    /* 获取界面选择的日期并拼接当前时间 */
    QString dateStr = ui->dateEdit->date().toString("yyyy-MM-dd");
    QString timeStr = QTime::currentTime().toString("hh:mm:ss");
    QString string = QString("%1 %2").arg(dateStr).arg(timeStr);
    qDebug()<<string;
    /* 调用 date 命令设置系统时间，再用 hwclock 同步到硬件时钟 */
    process.start("date",QStringList()<<"-s"<<string);
    process.waitForFinished();
    process.start("hwclock",QStringList()<<"-w");
    process.waitForFinished();
    qDebug()<<process.readAllStandardOutput();
    process.close();
}

void SettingWindow::on_pBtn_SwitchGif_clicked()
{
    if(GifSum==0)return;
    /* 随机选择一个 GIF */
    int id = qrand()%GifSum;
    movie->stop();
    movie->setFileName(gif_Files.at(id));

    /* 如果处于未选中状态，则将按钮设置为选中 */
    if(ui->pBtn_PauseGif->isChecked())
    {
        ui->pBtn_PauseGif->setChecked(true);
        ui->pBtn_PauseGif->setText("暂停");
    }

    movie->start();
}


void SettingWindow::on_pushButton_8_clicked()
{

    this->hide();
}


void SettingWindow::on_btn_connect_wifi_clicked()
{
    // 1. 检查选中项
    QList<QListWidgetItem*> items = ui->listWidget->selectedItems();
    if (items.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择一个 WiFi 网络");
        return;
    }
    QString wifiName = items.first()->text();
    QString password = ui->lineEdit_password->text();
    if (password.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入 WiFi 密码");
        return;
    }

    // 2. 添加一个新网络，获取网络 ID
    process.start("wpa_cli", QStringList() << "-i" << "wlan0" << "add_network");
    process.waitForFinished(3000);
    QString netIdStr = QString::fromLocal8Bit(process.readAllStandardOutput()).trimmed();
    process.close();

    bool ok;
    int netId = netIdStr.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "错误", "无法创建 WiFi 配置，请检查 wpa_supplicant 是否运行。");
        return;
    }

    // 3. 设置 SSID 和密码（注意 shell 转义）
    QString setSsidCmd = QString("set_network %1 ssid '\"%2\"'").arg(netId).arg(wifiName);
    QString setPskCmd  = QString("set_network %1 psk '\"%2\"'").arg(netId).arg(password);

    // 通过 bash -c 执行，确保引号被正确解析
    process.start("bash", QStringList() << "-c" << QString("wpa_cli -i wlan0 %1").arg(setSsidCmd));
    process.waitForFinished(3000); process.close();

    process.start("bash", QStringList() << "-c" << QString("wpa_cli -i wlan0 %1").arg(setPskCmd));
    process.waitForFinished(3000); process.close();

    // 4. 启用网络
    process.start("wpa_cli", QStringList() << "-i" << "wlan0" << "enable_network" << QString::number(netId));
    process.waitForFinished(5000);
    process.close();

    // 5. 保存配置（写入 /etc/wpa_supplicant/wpa_supplicant.conf）
    process.start("wpa_cli", QStringList() << "-i" << "wlan0" << "save_config");
    process.waitForFinished(3000);
    process.close();

    // 6. 等待连接建立并获取 IP（需要 root 权限运行 dhclient）
    QThread::msleep(3000);
    process.start("dhclient", QStringList() << "wlan0");
    process.waitForFinished(10000);

    QString error = QString::fromLocal8Bit(process.readAllStandardError());
    process.close();

    if (!error.isEmpty()) {
        QMessageBox::warning(this, "连接可能失败", "获取 IP 地址失败，请检查密码或网络。\n" + error);
        return;
    }

    QMessageBox::information(this, "成功", "WiFi 连接成功！");
}


void SettingWindow::on_btn_wifi_on_clicked()
{
    // 使用 ip 命令启用 wlan0（替代 ifconfig）
    process.start("ip", QStringList() << "link" << "set" << "wlan0" << "up");
    process.waitForFinished(5000);

    QString error = QString::fromLocal8Bit(process.readAllStandardError());
    process.close();

    if (!error.isEmpty()) {
        // 很可能是权限不足，提示用户需要 root 或 sudo
        QMessageBox::warning(this, "错误", "启用 WiFi 失败，请确保程序以 root 权限运行。\n" + error);
        return;
    }
    QMessageBox::information(this, "提示", "WiFi 已开启");
}


void SettingWindow::on_btn_scan_network_clicked()
{
    ui->listWidget->clear();

    // 1. 确保 wlan0 已启用
    process.start("ip", QStringList() << "link" << "set" << "wlan0" << "up");
    process.waitForFinished(3000);
    process.close();

    // 2. 触发扫描
    process.start("wpa_cli", QStringList() << "-i" << "wlan0" << "scan");
    process.waitForFinished(3000);
    process.close();

    // 3. 等待扫描完成（wpa_cli scan 只是触发，结果需要稍后获取）
    QThread::msleep(2000);

    // 4. 获取扫描结果
    process.start("wpa_cli", QStringList() << "-i" << "wlan0" << "scan_results");
    if (!process.waitForFinished(5000)) {
        QMessageBox::warning(this, "错误", "获取 WiFi 列表超时");
        process.kill();
        return;
    }

    QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
    process.close();

    // 5. 解析输出（wpa_cli 输出以 Tab 分隔，SSID 在最后一列）
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    QSet<QString> added;
    for (int i = 1; i < lines.count(); ++i) { // 跳过标题行
        QStringList cols = lines.at(i).split('\t');
        if (cols.size() < 5) continue;

        QString ssid = cols.last().trimmed();
        // 过滤空 SSID 和重复项
        if (ssid.isEmpty() || added.contains(ssid)) continue;

        added.insert(ssid);
        ui->listWidget->addItem(ssid);
    }

    if (ui->listWidget->count() == 0) {
        QMessageBox::information(this, "提示", "未扫描到可用 WiFi");
    }
}

/* 更新当前窗口的时间显示 */
void SettingWindow::on_timer_updateTime()
{
    ui->label_setting_date->setText(QDate::currentDate().toString("yyyy-MM-dd"));
    ui->label_setting_time->setText(QTime::currentTime().toString("hh:mm:ss"));
}


void SettingWindow::on_pBtn_PauseGif_clicked(bool checked)
{
    if(0==GifSum)return;
    if(!checked)
    {
        /* 按钮未选中时表示“暂停”，恢复播放 */
        ui->pBtn_PauseGif->setText("暂停");
        movie->setPaused(false);
    }
    else
    {
        /* 按钮选中时表示“播放”，停止播放 */
        ui->pBtn_PauseGif->setText("播放");
        movie->setPaused(true);
    }
}


