/**
 * @file        onevideo.h
 * @brief       视频播放控件类声明
 * @note        封装 QMediaPlayer 与 QVideoWidget，用于播放本地视频。
 */
#ifndef ONEVIDEO_H
#define ONEVIDEO_H

#include <QtMultimedia>
#include <QtMultimediaWidgets/QVideoWidget>

#include <QWidget>

namespace Ui {
class OneVideo;
}

/**
 * @brief 视频播放控件类
 */
class OneVideo : public QWidget
{
    Q_OBJECT

public:
    explicit OneVideo(QWidget *parent = nullptr);
    ~OneVideo();

private:
    Ui::OneVideo *ui;
    QVideoWidget *m_pPlayerWidget;   /* 视频显示组件 */
    QMediaPlayer *m_pPlayer;         /* 媒体播放器 */

public slots:
    /* 开始播放视频 */
    void PlayVideo();
};

#endif // ONEVIDEO_H
