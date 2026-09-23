# 车载多媒体终端

基于 Qt 5 的嵌入式 Linux 车载多媒体终端，运行于全志 A733（OrangePi 4 Pro）开发板。

## 功能模块

- **监控（Monitor）**：V4L2 + QThread 摄像头视频采集与实时显示，mmap 多缓冲轮转取帧
- **地图（Map）**：百度地图静态图展示 + GPS 定位
- **音乐（Music）**：本地/在线音乐播放
- **天气（Weather）**：心知天气 API 天气查询与展示
- **设置（Setting）**：Wi-Fi 等系统设置
- **传感器（DHT11）**：温湿度采集

## 技术栈

- 语言：C / C++
- GUI：Qt 5（QWidget），信号槽作为统一命令路由
- 平台：嵌入式 Linux（全志 A733 / OrangePi 4 Pro）
- 接口：V4L2、UART、I2C、GPIO

## 构建

用 Qt Creator 打开 `Vehicle_terminal.pro`，选择对应交叉编译 Kit 后 qmake 构建即可。目标板需提前部署 Qt 5.15 运行库。

## 说明

- 天气、地图模块调用的第三方 API 密钥（心知天气 appid/appsecret、百度地图 AK）已从源码中脱敏，使用前请在对应文件中填入自己的密钥：
  - `Weather/weather.cpp`：心知天气 `appid` / `appsecret`
  - `Map/baidumap.h`：百度地图静态图 `ak`
- `Music/myMusic/` 下的本地音乐文件未入库，请自行放入。
