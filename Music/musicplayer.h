#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

//#include <Music/searchmusic.h>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QDir>
#include <QFile>
#include <QUrl>
#include <QVector>
#include <QNetworkReply>
#include <QMessageBox>
#include <QLocale>
#include <QSet>
#include <QIcon>

/* 音乐模块命令定义（用于语音控制） */
#define MUSIC_COMMAND_CLOSE 0
#define MUSIC_COMMAND_SHOW 1
#define MUSIC_COMMAND_CLOSE 0
#define MUSIC_COMMAND_PRE 2
#define MUSIC_COMMAND_NEXT 3
#define MUSIC_COMMAND_PLAY 4
#define MUSIC_COMMAND_PAUSE 5

namespace Ui {
class MusicPlayer;
}

/* 媒体信息结构体 */
struct MediaObjectInfo {
    /* 用于保存歌曲文件名 */
    QString fileName;
    /* 用于保存歌曲文件路径 */
    QString filePath;
    /* 是否为喜欢（收藏）歌曲 */
    bool isFavorite = false;
};

/* 音乐播放器窗口类 */
class MusicPlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit MusicPlayer(QWidget *parent = nullptr);
    ~MusicPlayer();
    QString LocalSongsPath = "/myMusic";  /* 本地音乐存放的相对路径 */
    /* 扫描本地音乐并加入播放列表 */
    void ScanLocalSongs();
    /* 初始化音乐播放器 */
    void mediaPlayerInit();

private slots:

    /* 打开在线搜索音乐窗口 */
//    void on_pBtn_OpenSearchWin_clicked();
    /* 根据 URL 添加在线音乐 */
//    void AddMusicFromUrl(QString name,QString UrlPath);

    /* 音乐列表当前行改变：切换播放对应歌曲 */
    void on_listWidget_currentRowChanged(int currentRow);

    /* 上一首 */
    void on_pBtn_Pre_clicked();

    /* 下一首 */
    void on_pBtn_Next_clicked();

    /* 播放/暂停 */
    void on_pBtn_Pause_clicked();

    /* 增加音量 */
    void on_pBtn_Loud_clicked();

    /* 降低音量 */
    void on_pBtn_Low_clicked();

    /* 拖动进度条释放时：跳转到对应播放位置 */
    void on_horizontalSlider_sliderReleased();

    /* 音乐总时长改变 */
    void on_musicPlayer_DurationChanged(qint64);
    /* 音乐当前播放位置改变 */
    void on_musicPlayer_CurPostionChanged(qint64);

    /* 循环播放按钮 */
    void on_pBtn_mod_clicked();

    /* 喜欢（收藏）当前歌曲按钮 */
    void on_pBtn_SetLove_clicked();

    /* 处理获取歌曲真实下载地址的网络响应 */
    void on_GetSongTrueUrl(QNetworkReply *reply);
    /* 下载歌曲到本地 */
    void on_DownSong(QNetworkReply *reply);

    /* 退出按钮 */
    void on_pBtn_Back_clicked();
public slots:
    /* 处理语音控制传来的指令 */
    void on_handleCommand(int);
private:
    class SearchMusic *searchMusicWin; /* 在线搜索音乐窗口 */
    Ui::MusicPlayer *ui;
    QMediaPlayer *musicPlayer;         /* 媒体播放器 */
    QMediaPlaylist *musicPlayList;     /* 播放列表 */
    QString CurrentSaveSongFileName;   /* 当前保存的歌曲文件名 */
    QVector<MediaObjectInfo> SongInfoVector; /* 歌曲信息列表 */

    QSet<QString> favoriteNames;    /* 收藏歌曲标题集合（键为 fileName） */
    QString favoritesFilePath;      /* 收藏列表持久化文件路径 */
    bool m_syncingList = false;     /* 程序化同步列表时抑制 currentRowChanged 信号 */

    /* 从本地文件加载收藏列表 */
    void loadFavorites();
    /* 将收藏列表写入本地文件 */
    void saveFavorites();
    /* 把列表高亮同步到播放器当前歌曲（不触发 currentRowChanged 处理） */
    void syncListToPlayer();
    /* 刷新当前歌曲名与喜欢按钮图标 */
    void updateNowPlayingUi();

    QNetworkAccessManager First_netManager; /* 网络管理器：获取真实下载地址 */
    QNetworkRequest First_request;
    QNetworkAccessManager Second_netManager;/* 网络管理器：下载歌曲 */
    QNetworkRequest Second_request;

};

#endif // MUSICPLAYER_H
