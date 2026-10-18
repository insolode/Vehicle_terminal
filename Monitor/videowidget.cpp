/**
 * @file        videowidget.cpp
 * @brief       视频显示控件类实现
 */
#include "videowidget.h"
#include <QPainter>

/* 构造函数 */
VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent)
{
}

/* 设置要显示的图像帧并触发重绘 */
void VideoWidget::setFrame(const QImage &frame)
{
    m_frame = frame;
    update();
}

/* 清空画面 */
void VideoWidget::clearFrame()
{
    m_frame = QImage();
    update();
}

/* 设置宽高比模式 */
void VideoWidget::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    if (m_mode == mode)
        return;
    m_mode = mode;
    update();
}

/* 获取当前宽高比模式 */
Qt::AspectRatioMode VideoWidget::aspectRatioMode() const
{
    return m_mode;
}

/* 重绘事件：绘制当前图像帧 */
void VideoWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    /* 没有图像时显示提示 */
    if (m_frame.isNull()) {
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, tr("未获取到图像数据"));
        return;
    }

    /* 按比例缩放则居中绘制，否则拉伸填满整个控件 */
    if (m_mode == Qt::KeepAspectRatio) {
        QImage scaled = m_frame.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        int x = (width() - scaled.width()) / 2;
        int y = (height() - scaled.height()) / 2;
        painter.drawImage(x, y, scaled);
    } else {
        painter.drawImage(rect(), m_frame);
    }
}
