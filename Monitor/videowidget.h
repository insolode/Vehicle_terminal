/**
 * @file        videowidget.h
 * @brief       视频显示控件类声明
 * @note        保存最新一帧图像并自绘显示，为后续叠加（倒车辅助线/水印等）预留扩展点。
 */
#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QImage>

/**
 * @brief 视频显示控件：保存最新一帧图像并自绘
 */
class VideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr);

    /* 设置要显示的一帧图像并触发重绘 */
    void setFrame(const QImage &frame);
    /* 清空画面，显示"未获取到图像数据" */
    void clearFrame();

    /* 设置宽高比模式 */
    void setAspectRatioMode(Qt::AspectRatioMode mode);
    /* 获取当前宽高比模式 */
    Qt::AspectRatioMode aspectRatioMode() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage m_frame;                                   /* 当前显示的一帧图像 */
    Qt::AspectRatioMode m_mode = Qt::IgnoreAspectRatio; /* 宽高比模式：默认拉伸填满 */
};

#endif // VIDEOWIDGET_H
