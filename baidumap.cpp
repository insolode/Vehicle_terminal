#include "baidumap.h"
#include "ui_baidumap.h"

BaiduMap::BaiduMap(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BaiduMap)
{
    ui->setupUi(this);
}

BaiduMap::~BaiduMap()
{
    delete ui;
}
