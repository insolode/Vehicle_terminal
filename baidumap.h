#ifndef BAIDUMAP_H
#define BAIDUMAP_H

#include <QMainWindow>

namespace Ui {
class BaiduMap;
}

class BaiduMap : public QMainWindow
{
    Q_OBJECT

public:
    explicit BaiduMap(QWidget *parent = nullptr);
    ~BaiduMap();

private:
    Ui::BaiduMap *ui;
};

#endif // BAIDUMAP_H
