/**
 * @file        main.cpp
 * @brief       程序入口：创建 Qt 应用并启动主窗口
 */
#include "mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /* 创建并显示主窗口 */
    MainWindow w;
    w.show();

    /* 进入 Qt 事件循环 */
    return a.exec();
}
