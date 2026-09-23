#-------------------------------------------------
#
# Project created by QtCreator 2026-09-01T17:31:51
#
#-------------------------------------------------

QT       += core gui multimedia
QT += multimediawidgets #使用QVideoWidget视频显示组件
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# DEFINES += QT_NO_SSL

target.path = /home/orangepi/QT_projects/app
INSTALLS += target

INCLUDEPATH += Monitor

TARGET = Vehicle_terminal
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    Map/baidumap.cpp \
    Map/gps.cpp \
    Map/uart.cpp \
    Monitor/ap3216.cpp \
    Monitor/capture_thread.cpp \
    Monitor/videowidget.cpp \
    dht11.cpp \
        main.cpp \
        mainwindow.cpp \
    Music/musicplayer.cpp \
    Music/searchmusic.cpp \
    Weather/weather.cpp \
    clock.cpp \
    Monitor/monitor.cpp \
    onevideo.cpp \
    settingwindow.cpp

HEADERS += \
    Map/baidumap.h \
    Map/gps.h \
    Map/uart.h \
    Monitor/ap3216.h \
    Monitor/capture_thread.h \
    Monitor/videowidget.h \
    dht11.h \
        mainwindow.h \
    Music/musicplayer.h \
    Music/searchmusic.h \
    Weather/weather.h \
    clock.h \
    Monitor/monitor.h \
    onevideo.h \
    settingwindow.h

FORMS += \
    Map/baidumap.ui \
        mainwindow.ui \
    Music/musicplayer.ui \
    Music/searchmusic.ui \
    Weather/weather.ui \
    clock.ui \
    Monitor/monitor.ui \
    onevideo.ui \
    settingwindow.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    resources/mianwindow.qss \
    Music/myMusic/7paste - 我的阿勒泰_298669206.mp3 \
