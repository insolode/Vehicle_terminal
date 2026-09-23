#include "Music/musicplayer.h"
#include "ui_musicplayer.h"

#include <QMouseEvent>

MusicPlayer::MusicPlayer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MusicPlayer)
{
    ui->setupUi(this);
    //设置样式表
    QFile file(":/resources/music.qss"); // ":/" 是Qt资源的固定前缀[reference:12]
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString style = stream.readAll();
        file.close();

        // 2. 只给当前 this（即 LoginDialog 本身及其子控件）设置样式
        this->setStyleSheet(style);
    }

    /* 创建在线搜索音乐窗口 */
//    searchMusicWin = new SearchMusic(this);
    /* 初始化播放器并扫描本地音乐 */
    mediaPlayerInit();
    ScanLocalSongs();
    loadFavorites();

    /* 喜欢按钮：非 checkable，图标完全由收藏状态驱动 */
    ui->pBtn_SetLove->setCheckable(false);
    ui->pBtn_SetLove->setIconSize(QSize(50, 50));
    ui->pBtn_SetLove->setIcon(QIcon(":/Music/Ui/收藏.png"));

    /* 循环按钮：非 checkable，图标按初始播放模式（随机）设置 */
    ui->pBtn_mod->setCheckable(false);
    ui->pBtn_mod->setStyleSheet("border-image: url(:/Music/Ui/随机播放.png);");

    /* 若已有当前歌曲，刷新歌曲名与喜欢图标 */
    updateNowPlayingUi();

    #ifndef QT_NO_SSL
    /* 配置 SSL，忽略证书校验（方便访问 HTTPS 资源） */
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    Second_request.setSslConfiguration(config);
    #endif

//    connect(searchMusicWin,&SearchMusic::AddUrlMusic,this,&MusicPlayer::AddMusicFromUrl);
    connect(musicPlayer,SIGNAL(durationChanged(qint64)),this,SLOT(on_musicPlayer_DurationChanged(qint64)));
    connect(musicPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(on_musicPlayer_CurPostionChanged(qint64)));
    connect(&First_netManager,SIGNAL(finished(QNetworkReply* )),this,SLOT(on_GetSongTrueUrl(QNetworkReply* )));
    connect(&Second_netManager,SIGNAL(finished(QNetworkReply* )),this,SLOT(on_DownSong(QNetworkReply* )));

}


MusicPlayer::~MusicPlayer()
{
    delete ui;
}

/*  扫描本地音乐  */
void MusicPlayer::ScanLocalSongs()
{
    /* 定位本地音乐目录 /myMusic */
    QDir dir(QCoreApplication::applicationDirPath() + LocalSongsPath);
    qDebug() << "程序实际查找的目录：" << dir.absolutePath();
    QDir dirAbsolutePath(dir.absolutePath());
    if(dirAbsolutePath.exists())
    {
        /* 过滤出 *.mp3 文件 */
        QStringList filter;
        filter<<"*.mp3";
        QFileInfoList files = dirAbsolutePath.entryInfoList(filter,QDir::Files);
        for(int i=0;i<files.count();i++)
        {
            MediaObjectInfo info;
            /* 去掉 .mp3 后缀作为歌曲名 */
            QString fileName = QString::fromUtf8(files.at(i).fileName().replace(".mp3","").toUtf8().data());
            info.fileName = fileName;
            info.filePath = QString::fromUtf8(files.at(i).filePath().toUtf8().data());
            /* 将歌曲加入播放列表 */
            if(musicPlayList->addMedia(QUrl::fromLocalFile(info.filePath)))
            {
                SongInfoVector.append(info);
                ui->listWidget->addItem(info.fileName);
            }
            else
            {
                qDebug()<<musicPlayList->errorString()<<endl;
                qDebug()<<"Error number:"<<musicPlayList->error()<<endl;
            }
        }
    }

}

/* 初始化音乐播放器  */
void MusicPlayer::mediaPlayerInit()
{
    musicPlayer = new QMediaPlayer(this);
    musicPlayList = new QMediaPlaylist(this);
    musicPlayList->clear();
    musicPlayer->setPlaylist(musicPlayList);
    /* 设置播放模式 循环、随机、顺序、当前循环、当前一次 */
    musicPlayList->setPlaybackMode(QMediaPlaylist::Random);
}

/* 从本地文件加载收藏列表  */
void MusicPlayer::loadFavorites()
{
    favoritesFilePath = QCoreApplication::applicationDirPath() + LocalSongsPath + "/favorites.txt";
    QFile f(favoritesFilePath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString name = in.readLine().trimmed();
        if (!name.isEmpty())
            favoriteNames.insert(name);
    }
    f.close();

    /* 将收藏状态标记到歌曲信息，并更新列表项文本 */
    for (int i = 0; i < SongInfoVector.size(); ++i) {
        if (favoriteNames.contains(SongInfoVector[i].fileName)) {
            SongInfoVector[i].isFavorite = true;
            if (i < ui->listWidget->count())
                ui->listWidget->item(i)->setText(SongInfoVector[i].fileName + " ♥");
        }
    }
}

/* 将收藏列表写入本地文件  */
void MusicPlayer::saveFavorites()
{
    QFile f(favoritesFilePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return;
    QTextStream out(&f);
    for (const QString &name : favoriteNames)
        out << name << "\n";
    f.close();
}

/* 把列表高亮同步到播放器当前歌曲（抑制 currentRowChanged 处理）  */
void MusicPlayer::syncListToPlayer()
{
    int idx = musicPlayList->currentIndex();
    if (idx < 0)
        return;
    m_syncingList = true;
    ui->listWidget->setCurrentRow(idx);
    m_syncingList = false;
    updateNowPlayingUi();
}

/* 刷新当前歌曲名与喜欢按钮图标  */
void MusicPlayer::updateNowPlayingUi()
{
    int idx = musicPlayList->currentIndex();
    if (idx < 0 || idx >= SongInfoVector.size())
        return;
    ui->name_label->setText(SongInfoVector[idx].fileName);
    ui->pBtn_SetLove->setIcon(QIcon(SongInfoVector[idx].isFavorite
                                     ? ":/Music/Ui/收藏 (1).png"
                                     : ":/Music/Ui/收藏.png"));
}

///*  打开在线搜索音乐界面  */
//void MusicPlayer::on_pBtn_OpenSearchWin_clicked()
//{
//    searchMusicWin->show();
//}


///*  根据音乐url地址下载音乐 添加到本地  */
//void MusicPlayer::AddMusicFromUrl(QString name, QString UrlPath)
//{
//    qDebug()<<UrlPath;
//    /* 先通过 First_netManager 请求歌曲地址，获取真实下载地址 */
//    First_request.setUrl(QUrl(UrlPath));
//    First_netManager.get(First_request);
//    CurrentSaveSongFileName = name;
//}


/*  用户点击其他音乐进行播放  */
void MusicPlayer::on_listWidget_currentRowChanged(int currentRow)
{
    if (m_syncingList)      /* 程序化同步列表时忽略 */
        return;
    if (currentRow < 0)     /* 无选中项时忽略 */
        return;
    musicPlayer->stop();
    musicPlayList->setCurrentIndex(currentRow);
    musicPlayer->play();
    ui->pBtn_Pause->setChecked(false);   /* 同步“播放/暂停”按钮为播放中状态 */
    updateNowPlayingUi();
}

/* 点击切换上一首按钮 */
void MusicPlayer::on_pBtn_Pre_clicked()
{
    if (musicPlayList->isEmpty())
        return;
    musicPlayList->previous();
    syncListToPlayer();
    musicPlayer->play();
    ui->pBtn_Pause->setChecked(false);   /* 同步“播放/暂停”按钮为播放中状态 */
}

/* 点击切换下一首按钮 */
void MusicPlayer::on_pBtn_Next_clicked()
{
    if (musicPlayList->isEmpty())
        return;
    musicPlayList->next();
    syncListToPlayer();
    musicPlayer->play();
    ui->pBtn_Pause->setChecked(false);   /* 同步“播放/暂停”按钮为播放中状态 */
}


/* 暂停音乐 */
void MusicPlayer::on_pBtn_Pause_clicked()
{
    if(ui->pBtn_Pause->isChecked())
    {
        musicPlayer->stop();
    }
    else
        musicPlayer->play();
}


/*  增加音量  */
void MusicPlayer::on_pBtn_Loud_clicked()
{
    int curVolume = musicPlayer->volume();
    if(curVolume>99)
        return;
    else
        musicPlayer->setVolume(curVolume+10);
    qDebug()<<"Curr Volumn"<<QString::number(musicPlayer->volume()) <<endl;
}


/* 降低音量 */
void MusicPlayer::on_pBtn_Low_clicked()
{
    int curVolume = musicPlayer->volume();
    if(curVolume<1)
        return;
    else
        musicPlayer->setVolume(curVolume-10);
    qDebug()<<"Curr Volumn"<<QString::number(musicPlayer->volume()) <<endl;
}




/* 上下滑动音乐列表时 */
void MusicPlayer::on_horizontalSlider_sliderReleased()
{
    /* 拖动进度条释放时，跳转到对应的播放位置（毫秒） */
    musicPlayer->setPosition(ui->horizontalSlider->value()*1000);
}


/* 切换音乐时更新音乐总时长  */
void MusicPlayer::on_musicPlayer_DurationChanged(qint64 duration)
{
    ui->horizontalSlider->setMaximum(duration/1000);
    /* 将毫秒转换为 分:秒 格式 */
    int min = duration/1000/60;
    int sec = (duration/1000)%60;
    QString minStr = QString::number(min);
    QString secStr = QString::number(sec);
    if(min<10)minStr="0"+minStr;
    if(sec<10)secStr="0"+secStr;
    QString durationShow = QString("%1:%2").arg(minStr,secStr);
    ui->label_TotalTime->setText(durationShow);
}

/* 更新音乐当前播放时间显示 */
void MusicPlayer::on_musicPlayer_CurPostionChanged(qint64 value )
{
    ui->horizontalSlider->setValue(value/1000);
    int min = value/1000/60;
    int sec = (value/1000)%60;
    QString minStr = QString::number(min);
    QString secStr = QString::number(sec);
    if(min<10)minStr="0"+minStr;
    if(sec<10)secStr="0"+secStr;
    QString curTime = QString("%1:%2").arg(minStr,secStr);
    ui->label_CurTime->setText(curTime);
}


/* 点击循环播放按钮：在 顺序 -> 列表循环 -> 单曲循环 -> 随机 间循环切换 */
void MusicPlayer::on_pBtn_mod_clicked()
{
    QMediaPlaylist::PlaybackMode next;
    QString icon;
    switch (musicPlayList->playbackMode()) {
    case QMediaPlaylist::Sequential:
        next = QMediaPlaylist::Loop;
        icon = "列表循环.png";
        break;
    case QMediaPlaylist::Loop:
        next = QMediaPlaylist::CurrentItemInLoop;
        icon = "单曲循环.png";
        break;
    case QMediaPlaylist::CurrentItemInLoop:
        next = QMediaPlaylist::Random;
        icon = "随机播放.png";
        break;
    default:
        next = QMediaPlaylist::Sequential;
        icon = "顺序播放.png";
        break;
    }
    musicPlayList->setPlaybackMode(next);
    ui->pBtn_mod->setStyleSheet(QString("border-image: url(:/Music/Ui/%1);").arg(icon));
}

/* 点击喜欢按钮：切换当前歌曲的收藏状态  */
void MusicPlayer::on_pBtn_SetLove_clicked()
{
    int idx = musicPlayList->currentIndex();
    if (idx < 0 || idx >= SongInfoVector.size())
        return;

    SongInfoVector[idx].isFavorite = !SongInfoVector[idx].isFavorite;
    if (SongInfoVector[idx].isFavorite)
        favoriteNames.insert(SongInfoVector[idx].fileName);
    else
        favoriteNames.remove(SongInfoVector[idx].fileName);

    /* 更新列表项文本（加/去 ♥ 标记） */
    if (idx < ui->listWidget->count())
        ui->listWidget->item(idx)->setText(SongInfoVector[idx].fileName
                                           + (SongInfoVector[idx].isFavorite ? " ♥" : ""));

    /* 刷新喜欢按钮图标 */
    updateNowPlayingUi();

    /* 持久化到本地文件 */
    saveFavorites();
}


/* 根据音乐搜索网络请求，获取音乐的真实下载地址 */
void MusicPlayer::on_GetSongTrueUrl(QNetworkReply *reply)
{
    qDebug()<<"on_GetSongTrueUrl";
    qDebug()<<reply->rawHeaderList();
    qDebug()<<reply->rawHeaderPairs();
    qDebug()<<reply->readAll();
    int total = reply->rawHeaderPairs().length();
    qDebug()<<total;
    /* 遍历响应头，查找 Location 字段获取重定向的真实下载地址 */
    for(int i=0;i<total;i++)
        {
            QString first = QString(reply->rawHeaderPairs().at(i).first);
            if(first.compare(QString("Location"))==0)
            {
                QString urlDownload = QString(reply->rawHeaderPairs().at(i).second);
                if(urlDownload.endsWith("404"))
                {
                    QMessageBox::warning(this,"warning","该歌曲无法下载，请换一首");
                    return;
                }
                qDebug()<<urlDownload;
                /* 使用真实地址发起第二次请求下载歌曲 */
                Second_request.setUrl(QUrl(urlDownload));
                Second_netManager.get(Second_request);
            }
        }
}


/* 下载音乐到本地文件 */
void MusicPlayer::on_DownSong(QNetworkReply *reply)
{
    QFile file;
    QString FilePath = QCoreApplication::applicationDirPath()+LocalSongsPath+QString("/")+CurrentSaveSongFileName+QString(".mp3");
    file.setFileName(FilePath);
    /* 以覆盖写入的方式保存歌曲文件 */
    file.open(QIODevice::Append | QIODevice::Truncate);
    file.write(reply->readAll());
    file.close();
    qDebug()<<"Save Song <"<<CurrentSaveSongFileName<<"> "<<"Finished";
    /* 将下载的歌曲加入播放列表 */
    MediaObjectInfo info;
    info.fileName = CurrentSaveSongFileName;
    info.filePath = FilePath;
    if(musicPlayList->addMedia(QUrl::fromLocalFile(info.filePath)))
    {
        SongInfoVector.append(info);
        ui->listWidget->addItem(info.fileName);
    }
    else
    {
        qDebug()<<musicPlayList->errorString()<<endl;
        qDebug()<<"Error number:"<<musicPlayList->error()<<endl;
    }
}


/* 退出按钮  */
void MusicPlayer::on_pBtn_Back_clicked()
{
    if (parentWidget()) {
        parentWidget()->show();
    }
    this->hide();
}


/* 处理语音控制传来的指令 */
void MusicPlayer::on_handleCommand(int command)
{
    switch (command) {
    case MUSIC_COMMAND_SHOW:
        this->show();
        break;
    case MUSIC_COMMAND_CLOSE:
        this->hide();
        break;
    case MUSIC_COMMAND_PAUSE:
        musicPlayer->pause();
        break;
    case MUSIC_COMMAND_PLAY:
        if(musicPlayList->isEmpty())return;
        ui->listWidget->setCurrentRow(musicPlayList->previousIndex(1));
        musicPlayer->play();
        ui->pBtn_Pause->setChecked(false);   /* 同步“播放/暂停”按钮为播放中状态 */
        break;
    case MUSIC_COMMAND_PRE:
        if (!musicPlayList->isEmpty()) {
            musicPlayList->previous();
            syncListToPlayer();
            musicPlayer->play();
            ui->pBtn_Pause->setChecked(false);   /* 同步“播放/暂停”按钮为播放中状态 */
        }
        break;
    case MUSIC_COMMAND_NEXT:
        if (!musicPlayList->isEmpty()) {
            musicPlayList->next();
            syncListToPlayer();
            musicPlayer->play();
            ui->pBtn_Pause->setChecked(false);   /* 同步“播放/暂停”按钮为播放中状态 */
        }
        break;
    default:
        break;
    }
}


