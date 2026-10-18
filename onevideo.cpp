/**
 * @file        onevideo.cpp
 * @brief       视频播放控件类实现
 */
#include "onevideo.h"
#include "ui_onevideo.h"
#include<QDebug>

/* 构造函数 */
OneVideo::OneVideo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OneVideo)
{
    ui->setupUi(this);

    /* 创建媒体播放器和视频显示组件 */
    m_pPlayer = new QMediaPlayer;
    m_pPlayerWidget = new QVideoWidget;
    m_pPlayer->setVideoOutput(m_pPlayerWidget);
    ui->verticalLayout->addWidget(m_pPlayerWidget);

    /* 设置视频组件自动填充背景，保持播放时的视觉效果 */
    m_pPlayerWidget->setAutoFillBackground(true);

    /* 界面美化：背景设置为黑色 */
    QPalette qplte;
    qplte.setColor(QPalette::Window, QColor(0,0,0));
    m_pPlayerWidget->setPalette(qplte);

    /* 设置要播放的本地视频文件路径 */
    QString path = QCoreApplication::applicationDirPath() + "/Video/show.mp4";
    m_pPlayer->setMedia(QUrl::fromLocalFile(path));
}

/* 析构函数 */
OneVideo::~OneVideo()
{
    delete m_pPlayer;
    delete m_pPlayerWidget;
    delete ui;
}

/* 开始播放视频 */
void OneVideo::PlayVideo()
{
    m_pPlayer->play();
}
