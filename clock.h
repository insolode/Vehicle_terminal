#ifndef CLOCK_H
#define CLOCK_H

#include <QMainWindow>
#include<math.h>
#include<QResizeEvent>
#include<QGroupBox>
#include<QTimer>
#include<QTime>
#include<QPainter>

namespace Ui {
class Clock;
}

class Clock : public QMainWindow
{
    Q_OBJECT

public:
    explicit Clock(QWidget *parent = 0);
    ~Clock();

private:
    Ui::Clock *ui;
    QTime time;
    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // CLOCK_H
