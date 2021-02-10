#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QHeaderView>
#include <QDir>
#include <QTextBlock>
#include <QScrollBar>
#include <QDialog>
#include <QStandardPaths>
#include <QFontDialog>
#include <QColorDialog>
#include <QFileDialog>
#include <QSettings>
#include <QPushButton>
#include <QShortcut>
#include <QBitmap>
#include <QDesktopServices>
#include <QStackedWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    settings(QApplication::organizationName(), QApplication::applicationName())
{
    setWindowIcon(QIcon(":/icon/KuGou.svg"));
    setWindowFlags(Qt::FramelessWindowHint);
    resize(1000,700);

    move((QApplication::desktop()->width() - width()) / 2, (QApplication::desktop()->height() - height()) / 2);
    setStyleSheet(//"QListWidget { background:white; }"
                  //"QTableWidget { background:white; }"
                  //"QHeaderView::section { background-color:white; }"
                  //"QTableCornerButton::section { background:white; }"
                  "QPushButton:hover { background:rgba(0,131,221,50); }"
                  "QTabWidget { background:rgb(82,146,254); }"
                  "QTabWidget::pane { border:0px; }"    //内部边框
                  "QTabWidget::tab-bar { background:rgb(82,146,254); }"
                  //"QTabBar { background:rgb(82,146,254); }"
                  "QTabBar:tab { width:155px; height:30px; }"
                  "QTabBar::tab:selected { color:rgb(82,146,254); background:rgb(82,146,254); border-image: url(:/icon/tab.svg); }"
                  "QTabBar::tab:!selected { color:black; background:rgb(82,146,254); border-image: url(:/icon/tab1.svg); }"
                  );
    connect(new QShortcut(QKeySequence(Qt::Key_Escape),this), SIGNAL(activated()), this, SLOT(exitFullscreen()));
    connect(new QShortcut(QKeySequence(Qt::Key_Space),this), SIGNAL(activated()), this, SLOT(playPause()));

    QWidget *widget = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setSpacing(0);
    vbox->setContentsMargins(0,0,0,0);
    widget->setLayout(vbox);
    setCentralWidget(widget);

    titleBar = new TitleBar;
    connect(titleBar->action_search, SIGNAL(triggered(bool)), this, SLOT(preSearch()));
    connect(titleBar->lineEdit_search, SIGNAL(returnPressed()), this, SLOT(preSearch()));
    connect(titleBar->lineEdit_page, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(titleBar->pushButton_lastPage, SIGNAL(clicked(bool)), this, SLOT(lastPage()));
    connect(titleBar->pushButton_nextPage, SIGNAL(clicked(bool)), this, SLOT(nextPage()));
    connect(titleBar->pushButton_minimize, SIGNAL(clicked(bool)), this, SLOT(showMinimized()));
    connect(titleBar->pushButton_maximize, SIGNAL(clicked(bool)), this, SLOT(showNormalMaximize()));
    connect(titleBar->pushButton_close, SIGNAL(clicked(bool)), qApp, SLOT(quit()));
    connect(titleBar->action_set, SIGNAL(triggered()), this, SLOT(dialogSet()));
    connect(titleBar, SIGNAL(moveMainWindow(QPoint)), this, SLOT(moveMe(QPoint)));
    vbox->addWidget(titleBar);

    tabWidget = new QTabWidget;
    tabWidget->setTabShape(QTabWidget::Triangular);
    tabWidget->setAutoFillBackground(true);
    vbox->addWidget(tabWidget);

    controlBar = new ControlBar;
    connect(controlBar->pushButton_albumPic, &QPushButton::pressed, [=]{
        tabWidget->setCurrentWidget(textBrowser);
    });
    connect(controlBar->pushButton_last, SIGNAL(pressed()), this, SLOT(playLast()));
    connect(controlBar->pushButton_play, SIGNAL(pressed()), this, SLOT(playPause()));
    connect(controlBar->pushButton_next, SIGNAL(pressed()),this, SLOT(playNext()));
    connect(controlBar->pushButton_mute, SIGNAL(pressed()), this, SLOT(mute()));
    connect(controlBar->pushButton_lyric, &QPushButton::toggled, [=](bool b){
        if (b) {
            lyricWidget->show();
        } else {
            lyricWidget->hide();
        }
        settings.setValue("isShowLyric", b);
    });
    connect(controlBar->pushButton_download, SIGNAL(pressed()), this, SLOT(dialogDownload()));
    connect(controlBar->pushButton_fullscreen, SIGNAL(pressed()), this, SLOT(enterFullscreen()));
    connect(controlBar->slider_progress, SIGNAL(sliderReleased()), this, SLOT(setMPPosition()));
    connect(controlBar->slider_volume, SIGNAL(sliderReleased()), this, SLOT(setVolume()));
    //connect(controlBar->comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeQuality(QString)));
    vbox->addWidget(controlBar);

    //我的音乐页面
    widget_mymusic = new QWidget;
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->setContentsMargins(0,0,0,0);
    hbox->setSpacing(0);
    widget_mymusic->setLayout(hbox);

    listWidget_mymusic = new QListWidget;
    listWidget_mymusic->setFixedWidth(155);
    listWidget_mymusic->setSpacing(2);
    QListWidgetItem *LWI;
    QSize size(130,35);
    LWI = new QListWidgetItem("我的音乐");
    LWI->setFlags(Qt::NoItemFlags);
    LWI->setSizeHint(size);
    listWidget_mymusic->addItem(LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/radio1.svg"), "我的电台");
    LWI->setSizeHint(size);
    listWidget_mymusic->addItem(LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/computer.svg"), "本地与下载");
    LWI->setSizeHint(size);
    listWidget_mymusic->addItem(LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/recent.svg"), "最近播放");
    LWI->setSizeHint(size);
    listWidget_mymusic->addItem(LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/playlist1.svg"), "默认列表");
    LWI->setSizeHint(size);
    listWidget_mymusic->addItem(LWI);
    LWI = new QListWidgetItem("自建歌单");
    LWI->setFlags(Qt::NoItemFlags);
    LWI->setSizeHint(size);
    listWidget_mymusic->addItem(LWI);
    hbox->addWidget(listWidget_mymusic);
    hbox->addStretch();

    tabWidget->addTab(widget_mymusic, QIcon(":/icon/man.svg"), "我的音乐");

    //发现页面
    widget_discovery = new QWidget;
    hbox = new QHBoxLayout;
    hbox->setSpacing(0);
    hbox->setContentsMargins(0,0,0,0);
    widget_discovery->setLayout(hbox);

    listWidget_discovery = new QListWidget;
    listWidget_discovery->setFixedWidth(155);
    listWidget_discovery->setSpacing(10);
    LWI = new QListWidgetItem(QIcon(":/icon/music.svg"), "推荐");
    //LWI->setTextAlignment(Qt::AlignCenter);
    LWI->setSizeHint(size);
    listWidget_discovery->addItem(LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/songlist1.svg"), "歌单");
    LWI->setSizeHint(size);
    listWidget_discovery->addItem(LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/radio.svg"), "电台");
    LWI->setSizeHint(size);
    listWidget_discovery->addItem(LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/music.svg"), "听书");
    LWI->setSizeHint(size);
    listWidget_discovery->addItem(LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/rank.svg"), "排行");
    LWI->setSizeHint(size);
    listWidget_discovery->addItem(LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/man.svg"), "歌手");
    LWI->setSizeHint(size);
    listWidget_discovery->addItem(LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/category.svg"), "分类");
    LWI->setSizeHint(size);
    listWidget_discovery->addItem(LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/video.svg"), "视频");
    LWI->setSizeHint(size);
    listWidget_discovery->addItem(LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/star.svg"), "直播");
    LWI->setSizeHint(size);
    listWidget_discovery->addItem(LWI);
    listWidget_discovery->setCurrentRow(4);
    hbox->addWidget(listWidget_discovery);
    connect(listWidget_discovery, &QListWidget::currentRowChanged, [=](int row){
        switch (row) {
        case 1:
            stackedWidget->setCurrentWidget(tableWidget_songlist);
            break;
        }
    });
    connect(listWidget_discovery, &QListWidget::itemClicked, [=](QListWidgetItem *item){
        int row = listWidget_discovery->row(item);
        switch (row) {
        case 4:
            stackedWidget->setCurrentWidget(listWidget_rank);
            break;
        }
    });

    listWidget_rank = new QListWidget;
    listWidget_rank->setViewMode(QListView::IconMode);
    listWidget_rank->setGridSize(QSize(200,200));
    listWidget_rank->setIconSize(QSize(150,150));
    listWidget_rank->setResizeMode(QListView::Adjust);
    connect(listWidget_rank, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(rankListItemClick(QListWidgetItem*)));

    stackedWidget = new QStackedWidget;
    stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    stackedWidget->addWidget(listWidget_rank);
    hbox->addWidget(stackedWidget);

    tabWidget->addTab(widget_discovery, QIcon(":/icon/music.svg"), "发现");

    widget_songrank = new QWidget;
    vbox = new QVBoxLayout;
    vbox->setContentsMargins(0,0,0,0);
    hbox = new QHBoxLayout;
    label_rankimg = new QLabel;
    label_rankimg->setFixedSize(100,100);
    hbox->addWidget(label_rankimg);
    label_rankname = new QLabel;
    QFont font;
    font.setPointSize(15);
    font.setBold(true);
    label_rankname->setFont(font);
    hbox->addWidget(label_rankname);
    vbox->addLayout(hbox);

    tableWidget_songlist_rank = new QTableWidget;
    tableWidget_songlist_rank->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget_songlist_rank->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget_songlist_rank->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget_songlist_rank->setColumnCount(6);
    QStringList headerRank;
    headerRank << "歌名" << "时长" << "hash" << "mvhash" << "album_id" << "MV";
    tableWidget_songlist_rank->setHorizontalHeaderLabels(headerRank);
    tableWidget_songlist_rank->setColumnHidden(2,true);
    tableWidget_songlist_rank->setColumnHidden(3,true);
    tableWidget_songlist_rank->setColumnHidden(4,true);
    connect(tableWidget_songlist_rank, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(playSongRank(int,int)));
    vbox->addWidget(tableWidget_songlist_rank);
    widget_songrank->setLayout(vbox);
    stackedWidget->addWidget(widget_songrank);

    tableWidget_songlist = new QTableWidget;
    tableWidget_songlist->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget_songlist->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget_songlist->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget_songlist->setColumnCount(8);
    tableWidget_songlist->setColumnHidden(4,true);
    tableWidget_songlist->setColumnHidden(5,true);
    tableWidget_songlist->setColumnHidden(6,true);
    QStringList header;
    header << "歌名" << "歌手" << "专辑" << "时长" << "hash" << "mvhash" << "album_id" << "MV";
    tableWidget_songlist->setHorizontalHeaderLabels(header);
//    tableWidget_songlist->horizontalHeader()->setStyleSheet("QHeaderView::section { color:white; background-color:#232326; }");
//    tableWidget_songlist->verticalHeader()->setStyleSheet("QHeaderView::section { color:white; background-color:#232326; }");
//    tableWidget_songlist->setStyleSheet("QTableView { color:white; selection-background-color:#e6e6e6; }");
    connect(tableWidget_songlist, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(playSong(int,int)));
    stackedWidget->addWidget(tableWidget_songlist);

    videoWidget = new QVideoWidget;
    tabWidget->addTab(videoWidget, "MV");    

    textBrowser = new QTextBrowser;
    textBrowser->zoomIn(10);
    tabWidget->addTab(textBrowser, "歌词");

    player = new QMediaPlayer;
    player->setVideoOutput(videoWidget);
    connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChange(qint64)));
    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChange(qint64)));
    connect(player, SIGNAL(volumeChanged(int)), this, SLOT(volumeChange(int)));
    //connect(player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(errorHandle(QMediaPlayer::Error)));
    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), SLOT(stateChange(QMediaPlayer::State)));
    connect(new QShortcut(QKeySequence(Qt::Key_Left),this), SIGNAL(activated()), this, SLOT(seekBack()));
    connect(new QShortcut(QKeySequence(Qt::Key_Right),this), SIGNAL(activated()), this, SLOT(seekForward()));

    lyricWidget = new LyricWidget;
    connect(lyricWidget->pushButton_set, SIGNAL(pressed()), this, SLOT(dialogSet()));
    connect(lyricWidget->pushButton_close, &QPushButton::pressed, [=]{
        controlBar->pushButton_lyric->toggled(false);
        settings.setValue("isShowLyric", false);
    });
    int lx = settings.value("LyricX").toInt();
    int ly = settings.value("LyricY").toInt();
    //qDebug() << lx << ly;
    if (lx==0 || ly==0 || lx>QApplication::desktop()->width() || ly>QApplication::desktop()->height()) {
        lyricWidget->move((QApplication::desktop()->width()-lyricWidget->width())/2, QApplication::desktop()->height()-lyricWidget->height());
    } else {
        lyricWidget->move(lx, ly);
    }
    QColor color(settings.value("LyricFontColor").toString());
    QPalette plt;
    plt.setColor(QPalette::WindowText, color);
    lyricWidget->label_lyric->setPalette(plt);
    QString sfont = settings.value("LyricFont").toString();
    if (sfont != "") {
        QStringList SLFont = sfont.split(",");
        lyricWidget->label_lyric->setFont(QFont(SLFont.at(0), SLFont.at(1).toInt(), SLFont.at(2).toInt(), SLFont.at(3).toInt()));
    }
    lyricWidget->show();

    bool isShowLyric = settings.value("isShowLyric").toBool();
    if (isShowLyric) {
        controlBar->pushButton_lyric->setChecked(true);
    } else {
        lyricWidget->hide();
    }

    tabWidget->setCurrentWidget(widget_discovery);
    genRankList();

    //tabWidget->dumpObjectTree(); //显示控件树

    // Cookie的kg_mid算法：https://blog.csdn.net/ychgyyn/article/details/90110296
    QString key = genKey(4);
    qWarning() << key;
    //QString转MD5：https://blog.csdn.net/qq_33266987/article/details/80497595
    kg_mid = QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Md5).toHex();
    qWarning() << kg_mid;

    dfid = "3KsyWK16LQQx15O2S526TQXX";
}

MainWindow::~MainWindow()
{
}

void MainWindow::moveMe(QPoint point)
{
    move(point);
}

QByteArray MainWindow::getReply(QString surl)
{
    QNetworkAccessManager *NAM = new QNetworkAccessManager;
    QNetworkRequest request;
    request.setUrl(QUrl(surl));
    //request.setRawHeader("Cookie", "kg_mid=" + kg_mid);
    QNetworkReply *reply = NAM->get(request);
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    reply->deleteLater();
    return reply->readAll();
}

QByteArray MainWindow::postReply(QString surl, QString spost)
{
    QNetworkAccessManager *NAM = new QNetworkAccessManager;
    QNetworkRequest request;
    request.setUrl(QUrl(surl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QByteArray BA_post;
    BA_post.append(spost);
    QNetworkReply *reply = NAM->post(request, BA_post);
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    reply->deleteLater();
    return reply->readAll();
}

void MainWindow::showNormalMaximize()
{
    if (isMaximized()) {
        showNormal();
        titleBar->pushButton_maximize->setIcon(QIcon(":/icon/maximize.svg"));
    } else {
        showMaximized();
        titleBar->pushButton_maximize->setIcon(QIcon(":/icon/normal.svg"));
    }
}

void MainWindow::playSong(int row, int column)
{
    Q_UNUSED(column);
    controlBar->pushButton_albumPic->setIcon(QIcon(":/icon/album.svg"));
    controlBar->pushButton_love->toggled(false);
    QString hash = tableWidget_songlist->item(row,4)->text();
    QString album_id = tableWidget_songlist->item(row,6)->text();
    //QString surl = "http://www.kugou.com/yy/index.php?r=play/getdata&hash=" + hash;
    QString surl = "https://wwwapi.kugou.com/yy/index.php?r=play/getdata&hash=" + hash + "&mid=" + kg_mid + "&dfid=" + dfid + "&album_id=" + album_id;
    qWarning() << surl;
    QJsonDocument JD = QJsonDocument::fromJson(getReply(surl));
    QString songurl = JD.object().value("data").toObject().value("play_url").toString();
    player->setMedia(QUrl(songurl));
    player->play();
    QString songname = tableWidget_songlist->item(row,0)->text() + " - " + tableWidget_songlist->item(row,1)->text();
    controlBar->pushButton_songname->setText(songname);
    controlBar->pushButton_songname->setToolTip(songname);
    lyricWidget->label_lyric->setText(songname);
    setLyric(JD.object().value("data").toObject().value("lyrics").toString());
    QString imgurl = JD.object().value("data").toObject().value("img").toString();
    if (imgurl != "") {
        QPixmap pixmap;
        pixmap.loadFromData(getReply(imgurl));
        controlBar->pushButton_albumPic->setIcon(QIcon(pixmap.scaled(50,50,Qt::KeepAspectRatio,Qt::SmoothTransformation)));
        pixmap.save(QDir::currentPath() + "/cover.jpg");
        textBrowser->setStyleSheet("QTextBrowser{color:white; border-image:url(cover.jpg);}");
    }
}

void MainWindow::playSongRank(int row, int column)
{
    Q_UNUSED(column);
    controlBar->pushButton_albumPic->setIcon(QIcon(":/icon/album.svg"));
    controlBar->pushButton_love->toggled(false);
    QString hash = tableWidget_songlist_rank->item(row,2)->text();
    QString album_id = tableWidget_songlist_rank->item(row,4)->text();
    //QString surl = "http://www.kugou.com/yy/index.php?r=play/getdata&hash=" + hash;
    //请求新增mid和dfid参数：https://my.oschina.net/u/4409146/blog/3306849
    QString surl = "https://wwwapi.kugou.com/yy/index.php?r=play/getdata&hash=" + hash + "&mid=" + kg_mid + "&dfid=" + dfid + "&album_id=" + album_id;
    qWarning() << surl;
    QJsonDocument JD = QJsonDocument::fromJson(getReply(surl));
    QString songurl = JD.object().value("data").toObject().value("play_url").toString();
    qWarning() << songurl;
    player->setMedia(QUrl(songurl));
    player->play();
    QString songname = tableWidget_songlist_rank->item(row,0)->text();
    //navWidget->label_songname->setText(songname);
    controlBar->pushButton_songname->setText(songname);
    controlBar->pushButton_songname->setToolTip(songname);
    lyricWidget->label_lyric->setText(songname);
    setLyric(JD.object().value("data").toObject().value("lyrics").toString());
    QString imgurl = JD.object().value("data").toObject().value("img").toString();
    if (imgurl != "") {
        QPixmap pixmap;
        pixmap.loadFromData(getReply(imgurl));
        controlBar->pushButton_albumPic->setIcon(QIcon(pixmap.scaled(50,50,Qt::KeepAspectRatio,Qt::SmoothTransformation)));
        pixmap.save(QDir::currentPath() + "/cover.jpg");
        textBrowser->setStyleSheet("QTextBrowser{color:white; border-image:url(cover.jpg);}");
    }
}

void MainWindow::durationChange(qint64 d)
{    
    int id = static_cast<int>(d);
    controlBar->slider_progress->setMaximum(id);
    QTime t(0,0,0);
    t = t.addMSecs(id);
    if (t < QTime(1,0,0)) {
        controlBar->label_song_duration->setText(t.toString("mm:ss"));
    } else {
        controlBar->label_song_duration->setText(t.toString("H:mm:ss"));
    }
}

void MainWindow::positionChange(qint64 p)
{
    //qDebug() << "position =" << p;
    int ip = static_cast<int>(p);
    controlBar->slider_progress->setValue(ip);
    QTime t(0,0,0);
    t = t.addMSecs(ip);
    controlBar->label_song_timeNow->setText(t.toString("mm:ss"));

    // 歌词选行
    int hl=0;
    // 非最后一句
    for(int i=0; i<lyrics.size()-1; i++){
        if(t>lyrics.at(i).time && t<lyrics.at(i+1).time){
            lyricWidget->label_lyric->setText(lyrics.at(i).sentence);
            hl=i;
            break;
        }
    }
    //最后一句
    if(lyrics.size()>0){
        int j = lyrics.size()-1;
        if(t>lyrics.at(j).time){
            lyricWidget->label_lyric->setText(lyrics.at(j).sentence);
            hl=j;
        }
    }

    // 歌词文本着色
    for (int a=0; a<lyrics.size(); a++) {
        QTextCursor cursor(textBrowser->document()->findBlockByLineNumber(a));
        QTextBlockFormat TBF = cursor.blockFormat();
        TBF.setForeground(QBrush(Qt::white));
        //TBF.setBackground(QBrush(Qt::transparent));
        TBF.clearBackground();
        cursor.setBlockFormat(TBF);
    }
    if(lyrics.size()>0){
        QTextCursor cursor1(textBrowser->document()->findBlockByLineNumber(hl));
        QTextBlockFormat TBF1 = cursor1.blockFormat();
        TBF1.setForeground(QBrush(Qt::green));
        TBF1.setBackground(QBrush(QColor(255,255,255,80)));
        cursor1.setBlockFormat(TBF1);
        //textBrowser->setTextCursor(cursor1);
        QScrollBar *scrollBar = textBrowser->verticalScrollBar();
        //qDebug() << "scrollBar" << scrollBar->maximum() << scrollBar->maximum()*hl/(lyrics.size()) ;
        scrollBar->setSliderPosition(scrollBar->maximum()*hl/(lyrics.size()));
    }
}

void MainWindow::volumeChange(int v)
{
    controlBar->slider_volume->setValue(v);
    controlBar->slider_volume->setToolTip(QString::number(v));
}

void MainWindow::stateChange(QMediaPlayer::State state)
{
    //qDebug() << state;
    if(state == QMediaPlayer::PlayingState){
        controlBar->pushButton_play->setIcon(QIcon(":icon/pause.svg"));
    }
    if(state == QMediaPlayer::PausedState){
        controlBar->pushButton_play->setIcon(QIcon(":/icon/play.svg"));
    }
    if(state == QMediaPlayer::StoppedState){
        controlBar->pushButton_play->setIcon(QIcon(":/icon/play.svg"));
    }
}

void MainWindow::playPause()
{
    //qDebug() << "state=" << player->state();
    if (player->state() == QMediaPlayer::PlayingState) {
        player->pause();
    } else if (player->state() == QMediaPlayer::PausedState) {
        player->play();
    } else if (player->state() == QMediaPlayer::StoppedState) {
        player->play();
    }
}

void MainWindow::setMPPosition()
{
    player->setPosition(controlBar->slider_progress->value());
}


void MainWindow::setVolume()
{
    player->setVolume(controlBar->slider_volume->value());
}

void MainWindow::mute()
{
    if(player->isMuted()){
        player->setMuted(false);
        controlBar->pushButton_mute->setIcon(QIcon(":/icon/volume.svg"));
        controlBar->slider_volume->setValue(volume);
    }else{
        volume = player->volume();
        player->setMuted(true);
        controlBar->pushButton_mute->setIcon(QIcon(":/icon/mute.svg"));
        controlBar->slider_volume->setValue(0);
    }
}

void MainWindow::preSearch()
{
    titleBar->lineEdit_page->setText("1");
    search();
}

void MainWindow::search()
{
    if (titleBar->lineEdit_search->text() != "") {
        tabWidget->setCurrentWidget(widget_discovery);
        listWidget_discovery->setCurrentRow(1);
        stackedWidget->setCurrentWidget(tableWidget_songlist);
        int pagesize = 20;
        QString surl = "http://mobilecdn.kugou.com/api/v3/search/song?format=json&keyword=" + titleBar->lineEdit_search->text() + "&page=" + QString::number(titleBar->lineEdit_page->text().toInt()) + "&pagesize=" + QString::number(pagesize) + "&showtype=1";
        qWarning() << surl;
        tableWidget_songlist->setRowCount(0);
        QJsonDocument json = QJsonDocument::fromJson(getReply(surl));
        QJsonArray songs = json.object().value("data").toObject().value("info").toArray();
        //qDebug() << songs;
        for(int i=0; i<songs.size(); i++){
            tableWidget_songlist->insertRow(i);
            tableWidget_songlist->setItem(i,0,new QTableWidgetItem(songs[i].toObject().value("songname").toString()));
            tableWidget_songlist->setItem(i,1,new QTableWidgetItem(songs[i].toObject().value("singername").toString()));
            tableWidget_songlist->setItem(i,2,new QTableWidgetItem(songs[i].toObject().value("album_name").toString()));
            int ds = songs[i].toObject().value("duration").toInt();
            QTableWidgetItem *TWI = new QTableWidgetItem(QString("%1:%2").arg(ds/60,2,10,QLatin1Char(' ')).arg(ds%60,2,10,QLatin1Char('0')));
            TWI->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
            tableWidget_songlist->setItem(i,3,TWI);
            tableWidget_songlist->setItem(i,4,new QTableWidgetItem(songs[i].toObject().value("hash").toString()));
            QString mvhash = songs[i].toObject().value("mvhash").toString();
            tableWidget_songlist->setItem(i,5,new QTableWidgetItem(mvhash));
            QString album_id = songs[i].toObject().value("album_id").toString();
            tableWidget_songlist->setItem(i,6,new QTableWidgetItem(album_id));
            if (mvhash != "") {
                QPushButton *pushButton_MV = new QPushButton;
                pushButton_MV->setFixedSize(24,24);
                pushButton_MV->setIcon(QIcon(":/icon/video.svg"));
                pushButton_MV->setIconSize(QSize(24,24));
                //pushButton_MV->setFocusPolicy(Qt::NoFocus);
                pushButton_MV->setFlat(true);
                pushButton_MV->setCursor(QCursor(Qt::PointingHandCursor));
                connect(pushButton_MV, SIGNAL(clicked()), this, SLOT(pushButtonMVClicked()));
                tableWidget_songlist->setCellWidget(i,7,pushButton_MV);
            }
        }
        tableWidget_songlist->resizeColumnsToContents();
    }
}

void MainWindow::lastPage()
{
    int page = titleBar->lineEdit_page->text().toInt();
    if(page > 1){
        titleBar->lineEdit_page->setText(QString::number(page-1));
        search();
    }
}

void MainWindow::nextPage()
{
    int page = titleBar->lineEdit_page->text().toInt();
    if(page < 99){
        titleBar->lineEdit_page->setText(QString::number(page+1));
        search();
    }
}

void MainWindow::setLyric(QString s)
{
    textBrowser->setText("");
    lyrics.clear();
    QStringList line = s.split("\n");
    for(int i=0; i<line.size(); i++){
        if(line.at(i).contains("]")){
            QStringList strlist = line.at(i).split("]");
            //qDebug() << strlist.at(0).mid(1);
            Lyric lyric;
            QString stime = strlist.at(0).mid(1);
            //qDebug() << stime.length() << stime.indexOf(".");
            if((stime.length()-stime.indexOf("."))==3) stime += "0";
            lyric.time = QTime::fromString(stime, "mm:ss.zzz");
            lyric.sentence = strlist.at(1);
            lyrics.append(lyric);
        }
    }
    for(int i=0; i<lyrics.size(); i++){
        textBrowser->insertPlainText(lyrics.at(i).sentence + "\n");
    }
    textBrowser->selectAll();
    textBrowser->setAlignment(Qt::AlignCenter);
    QTextCursor cursor = textBrowser->textCursor();
    cursor.setPosition(0, QTextCursor::MoveAnchor);
    textBrowser->setTextCursor(cursor);
}

void MainWindow::dialogSet()
{
    QDialog *dialog_set = new QDialog(this);
    dialog_set->setWindowTitle("设置");
    dialog_set->setFixedSize(300,200);
    dialog_set->setStyleSheet("QLineEdit{border:1px solid black;}");
    QVBoxLayout *vbox = new QVBoxLayout;
    QHBoxLayout *hbox = new QHBoxLayout;
    QLabel *label = new QLabel("歌词");
    hbox->addWidget(label);
    pushButton_font = new QPushButton;
    QString sfont = lyricWidget->label_lyric->font().family() + "," + QString::number(lyricWidget->label_lyric->font().pointSize()) + "," + QString::number(lyricWidget->label_lyric->font().weight()) + "," + lyricWidget->label_lyric->font().italic();
    pushButton_font->setText(sfont);
    pushButton_font->setFocusPolicy(Qt::NoFocus);
    //pushButton_font->setFlat(true);
    connect(pushButton_font, SIGNAL(pressed()), this, SLOT(chooseFont()));
    hbox->addWidget(pushButton_font);
    pushButton_fontcolor = new QPushButton;
    pushButton_fontcolor->setText("■");
    pushButton_fontcolor->setFocusPolicy(Qt::NoFocus);
    //pushButton_fontcolor->setFlat(true);
    QPalette plt = lyricWidget->label_lyric->palette();
    QBrush brush = plt.color(QPalette::WindowText);
    plt.setColor(QPalette::ButtonText, brush.color());
    pushButton_fontcolor->setPalette(plt);
    connect(pushButton_fontcolor, SIGNAL(pressed()), this, SLOT(chooseFontColor()));
    hbox->addWidget(pushButton_fontcolor);
    vbox->addLayout(hbox);

    hbox = new QHBoxLayout;
    downloadPath = settings.value("DownloadPath").toString();
    QPushButton *pushbutton_openpath = new QPushButton("保存路径");
    pushbutton_openpath->setStyleSheet("border:none;");
    connect(pushbutton_openpath, &QPushButton::clicked, [=](){
         QDesktopServices::openUrl(QUrl::fromLocalFile(downloadPath));
    });
    hbox->addWidget(pushbutton_openpath);

    QLineEdit *lineEdit_downloadPath = new QLineEdit;
    if (downloadPath == "")
        lineEdit_downloadPath->setText(QStandardPaths::writableLocation(QStandardPaths::MusicLocation));
    else
        lineEdit_downloadPath->setText(downloadPath);    
    QAction *action_chooseDownloadPath = new QAction(this);
    action_chooseDownloadPath->setIcon(QIcon::fromTheme("folder"));
    connect(action_chooseDownloadPath, &QAction::triggered, [=](){
        downloadPath = QFileDialog::getExistingDirectory(this, "保存路径", downloadPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if(downloadPath != ""){
            lineEdit_downloadPath->setText(downloadPath);
            settings.setValue("DownloadPath", downloadPath);
        }
    });
    lineEdit_downloadPath->addAction(action_chooseDownloadPath, QLineEdit::TrailingPosition);
    hbox->addWidget(lineEdit_downloadPath);
    vbox->addLayout(hbox);
    dialog_set->setLayout(vbox);
    dialog_set->show();
}

void MainWindow::chooseFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, lyricWidget->label_lyric->font(), this, "选择字体");
    if(ok){
       lyricWidget->label_lyric->setFont(font);
       QString sfont = font.family() + "," + QString::number(font.pointSize()) + "," + QString::number(font.weight()) + "," + font.italic();
       pushButton_font->setText(sfont);
       settings.setValue("LyricFont", sfont);
       lyricWidget->label_lyric->adjustSize();
       //qDebug() << "label_after" << desktopLyric->ui->label_lyric->size();
       lyricWidget->resize(lyricWidget->label_lyric->size());
       //qDebug() << "window" << desktopLyric->size();
    }
}

void MainWindow::chooseFontColor()
{
    QPalette plt = lyricWidget->label_lyric->palette();
    QBrush brush = plt.color(QPalette::WindowText);
    QColor color = QColorDialog::getColor(brush.color(), this);
    if(color.isValid()){
        plt.setColor(QPalette::WindowText, color);
        lyricWidget->label_lyric->setPalette(plt);
        plt.setColor(QPalette::ButtonText, color);
        pushButton_fontcolor->setPalette(plt);
        settings.setValue("LyricFontColor", color.name());
    }
}

void MainWindow::playLast()
{
    if(tabWidget->currentIndex()==0){
        int row = tableWidget_songlist_rank->currentRow();
        qDebug() << row;
        if (row != -1) {
            if (row > 0) {
                row--;
                playSongRank(row,0);
                tableWidget_songlist_rank->setCurrentCell(row,0);
            }
        }
    }
    if(tabWidget->currentIndex()==2){
        int row = tableWidget_songlist->currentRow();
        qDebug() << row;
        if (row != -1) {
            if (row > 0) {
                row--;
                playSong(row,0);
                tableWidget_songlist->setCurrentCell(row,0);
            }
        }
    }
}

void MainWindow::playNext()
{
    if (tabWidget->currentIndex() == 0) {
        int row = tableWidget_songlist_rank->currentRow();
        int rc = tableWidget_songlist_rank->rowCount();
        qDebug() << row << rc;
        if (row != -1) {
            if (row < rc-1) {
                row++;
                playSongRank(row,0);
                tableWidget_songlist_rank->setCurrentCell(row,0);
            }
        }
    }
    if (tabWidget->currentIndex() == 2) {
        int row = tableWidget_songlist->currentRow();
        int rc = tableWidget_songlist->rowCount();
        qDebug() << row << rc;
        if (row != -1) {
            if (row < rc-1) {
                row++;
                playSong(row,0);
                tableWidget_songlist->setCurrentCell(row,0);
            }
        }
    }
}

void MainWindow::pushButtonMVClicked()
{
    controlBar->pushButton_albumPic->setIcon(QIcon(":/icon/album.svg"));
    textBrowser->setText("");
    lyrics.clear();
    QPushButton *pushButton = qobject_cast<QPushButton*>(sender());
    if (pushButton == nullptr) {
        return;
    }
    QModelIndex index = tableWidget_songlist->indexAt(QPoint(pushButton->frameGeometry().x(), pushButton->frameGeometry().y()));
    int row = index.row();
    tableWidget_songlist->setCurrentCell(row,0);
    QString songname = tableWidget_songlist->item(row,0)->text() + " - " + tableWidget_songlist->item(row,1)->text();
    controlBar->pushButton_songname->setText(songname);
    controlBar->pushButton_songname->setToolTip(songname);
    QString mvhash = tableWidget_songlist->item(row,5)->text();
    QString surl = "http://m.kugou.com/app/i/mv.php?cmd=100&ismp3=1&ext=mp4&hash=" + mvhash;
    qWarning() << surl;
    QJsonDocument JD = QJsonDocument::fromJson(getReply(surl));
    QString mvurl = JD.object().value("mvdata").toObject().value("rq").toObject().value("downurl").toString();
    if (mvurl == "") {
        mvurl = JD.object().value("mvdata").toObject().value("sq").toObject().value("downurl").toString();
    }
    if (mvurl == "") {
        mvurl = JD.object().value("mvdata").toObject().value("le").toObject().value("downurl").toString();
    }
    qWarning() << mvurl;
    controlBar->pushButton_lyric->toggled(false);
    tabWidget->setCurrentWidget(videoWidget);
    player->setMedia(QUrl(mvurl));
    player->play();
    QString mvicon = JD.object().value("mvicon").toString().replace("/{size}/","/");
    qWarning() << mvicon;
    if (mvicon != "") {
        QPixmap pixmap;
        pixmap.loadFromData(getReply(mvicon));
        controlBar->pushButton_albumPic->setIcon(QIcon(pixmap.scaled(50,50,Qt::KeepAspectRatio,Qt::SmoothTransformation)));
        pixmap.save(QDir::currentPath() + "/cover.jpg");
        textBrowser->setStyleSheet("QTextBrowser{color:white; border-image:url(cover.jpg);}");
    }
}

void MainWindow::rankPushButtonMVClicked()
{
    textBrowser->setText("");
    lyrics.clear();
    QPushButton *pushButton = qobject_cast<QPushButton*>(sender());
    if (pushButton == nullptr) {
        return;
    }
    QModelIndex index = tableWidget_songlist_rank->indexAt(QPoint(pushButton->frameGeometry().x(), pushButton->frameGeometry().y()));
    int row = index.row();
    tableWidget_songlist_rank->setCurrentCell(row, 0);
    QString songname = tableWidget_songlist_rank->item(row, 0)->text();
    controlBar->pushButton_songname->setText(songname);
    controlBar->pushButton_songname->setToolTip(songname);
    QString mvhash = tableWidget_songlist_rank->item(row, 3)->text();
    QString surl = "http://m.kugou.com/app/i/mv.php?cmd=100&ismp3=1&ext=mp4&hash=" + mvhash;
    qDebug() << surl;
    QJsonDocument JD = QJsonDocument::fromJson(getReply(surl));
    QString mvurl = JD.object().value("mvdata").toObject().value("rq").toObject().value("downurl").toString();
    if (mvurl == "") {
        mvurl = JD.object().value("mvdata").toObject().value("sq").toObject().value("downurl").toString();
    }
    if (mvurl == "") {
        mvurl = JD.object().value("mvdata").toObject().value("le").toObject().value("downurl").toString();
    }
    qDebug() << mvurl;
    controlBar->pushButton_lyric->toggled(false);
    tabWidget->setCurrentWidget(videoWidget);
    player->setMedia(QUrl(mvurl));
    player->play();
    QString mvicon = JD.object().value("mvicon").toString().replace("/{size}/","/");
    qWarning() << mvicon;
    if (mvicon != "") {
        QPixmap pixmap;
        pixmap.loadFromData(getReply(mvicon));
        controlBar->pushButton_albumPic->setIcon(QIcon(pixmap.scaled(50,50,Qt::KeepAspectRatio,Qt::SmoothTransformation)));
        pixmap.save(QDir::currentPath() + "/cover.jpg");
        textBrowser->setStyleSheet("QTextBrowser{color:white; border-image:url(cover.jpg);}");
    }
}

void MainWindow::enterFullscreen()
{
    if (tabWidget->currentWidget() == videoWidget || tabWidget->currentWidget() == textBrowser) {
        showFullScreen();
        titleBar->hide();
        controlBar->hide();
        tabWidget->tabBar()->hide();
        lyricWidget->hide();
    }
}

void MainWindow::exitFullscreen()
{
    if (isFullScreen()) {
        showNormal();
        titleBar->show();
        controlBar->show();
        tabWidget->tabBar()->show();
        if (controlBar->pushButton_lyric->isChecked())
            lyricWidget->show();
    }
}

void MainWindow::dialogDownload()
{
    QDialog *dialog = new QDialog(this);
    dialog->setFixedSize(300,200);
    dialog->setWindowFlags(dialog->windowFlags() &~ Qt::WindowMinimizeButtonHint);//去掉最小化无效
    dialog->setWindowTitle("下载");
    QGridLayout *gridLayout = new QGridLayout;
    QLabel *label = new QLabel("歌名");
    gridLayout->addWidget(label,0,0,1,1);
    QLineEdit *lineEdit_songname = new QLineEdit;
    lineEdit_songname->setText(controlBar->pushButton_songname->text());
    gridLayout->addWidget(lineEdit_songname,0,1,1,1);
    label = new QLabel("下载地址");
    gridLayout->addWidget(label,1,0,1,1);
    QLineEdit *lineEdit_url = new QLineEdit;
    lineEdit_url->setText(player->media().canonicalUrl().toString());
    gridLayout->addWidget(lineEdit_url,1,1,1,1);
    QPushButton *pushbutton_openpath = new QPushButton("保存路径");
    pushbutton_openpath->setStyleSheet("border:none;");
    gridLayout->addWidget(pushbutton_openpath,2,0,1,1);
//    pushButton_path = new QPushButton;
//    pushButton_path->setObjectName("DownloadDialogPath");
//    pushButton_path->setFocusPolicy(Qt::NoFocus);
//    downloadPath = settings.value("DownloadPath").toString();
//    pushButton_path->setText(downloadPath);
//    pushButton_path->setToolTip(downloadPath);
//    connect(pushButton_path, SIGNAL(pressed()), this, SLOT(chooseDownloadPath()));
//    gridLayout->addWidget(pushButton_path,2,1,1,1);

    QLineEdit *lineEdit_downloadPath1 = new QLineEdit;
    downloadPath = settings.value("DownloadPath").toString();
    if (downloadPath == "") {
        lineEdit_downloadPath1->setText(QStandardPaths::writableLocation(QStandardPaths::MusicLocation));
    } else {
        lineEdit_downloadPath1->setText(downloadPath);
    }
    QAction *action_chooseDownloadPath = new QAction(this);
    action_chooseDownloadPath->setIcon(QIcon::fromTheme("folder"));
    connect(action_chooseDownloadPath, &QAction::triggered, [=](){
        downloadPath = QFileDialog::getExistingDirectory(this, "保存路径", downloadPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (downloadPath != "") {
            lineEdit_downloadPath1->setText(downloadPath);
            settings.setValue("DownloadPath", downloadPath);
        }
    });
    lineEdit_downloadPath1->addAction(action_chooseDownloadPath, QLineEdit::TrailingPosition);
    gridLayout->addWidget(lineEdit_downloadPath1,2,1,1,1);

    connect(pushbutton_openpath, &QPushButton::clicked, [=](){
         QDesktopServices::openUrl(QUrl::fromLocalFile(downloadPath));
    });
    dialog->setLayout(gridLayout);
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addStretch();
    QPushButton *pushButton_confirm = new QPushButton("确定");
    connect(pushButton_confirm, SIGNAL(clicked()), dialog, SLOT(accept()));
    hbox->addWidget(pushButton_confirm);
    QPushButton *pushButton_cancel = new QPushButton("取消");
    connect(pushButton_cancel, SIGNAL(clicked()), dialog, SLOT(reject()));
    hbox->addWidget(pushButton_cancel);
    hbox->addStretch();
    gridLayout->addLayout(hbox,3,0,1,2);
    int result = dialog->exec();
    if (result == QDialog::Accepted) {
        QString filepath = lineEdit_downloadPath1->text() + "/" + lineEdit_songname->text() + "." + QFileInfo(lineEdit_url->text()).suffix();
        qDebug() << lineEdit_url->text() << filepath;
        download(lineEdit_url->text(), filepath);
    } else if (result == QDialog::Rejected) {
        dialog->close();
    }
}

void MainWindow::download(QString surl, QString filepath)
{
    //controlBar->pushButton_download->setEnabled(false);//禁用按钮背景色不显示
    qDebug() <<  "download -> " << surl << "->" << filepath;
    QUrl url = QString(surl);
    QNetworkAccessManager *NAM = new QNetworkAccessManager;
    QNetworkRequest request(url);
    QNetworkReply *reply = NAM->get(request);
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)));
    connect(reply, &QNetworkReply::finished, [=]() {
        qDebug() << reply->errorString();
        switch (reply->error()) {
        case QNetworkReply::NoError :{
            QFile file(filepath);
            file.open(QIODevice::WriteOnly);
            file.write(reply->readAll());
            file.flush();
            file.close();
            break;}
        default:
            break;
        }
        //controlBar->pushButton_download->setEnabled(true);
        reply->deleteLater();
    });
}

void MainWindow::updateProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    //ui->pushButton_download->setText(QString("%1%").arg(bytesReceived*100/bytesTotal));
    double p = static_cast<double>(bytesReceived)/bytesTotal;
    controlBar->pushButton_download->setStyleSheet(QString("QPushButton { background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0,"
                                                   "stop:0 rgba(48, 194, 124, 255), stop:%1 rgba(48, 194, 124, 255),"
                                                   "stop:%2 rgba(255, 255, 255, 255), stop:1 rgba(255, 255, 255, 255)); }")
                                      .arg(p-0.001)
                                      .arg(p));
    qDebug() << p << controlBar->pushButton_download->styleSheet();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    //圆角 https://jingyan.baidu.com/article/219f4bf799ccccde442d381d.html
    QBitmap bitmap(size());
    bitmap.fill();
    QPainter painter(&bitmap);
    painter.setRenderHint(QPainter::Antialiasing, true);  //无效
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    if (isFullScreen())
        painter.drawRect(bitmap.rect());
    else
        painter.drawRoundedRect(bitmap.rect(),10,10);
    setMask(bitmap);
}

void MainWindow::seekBack()
{
    player->setPosition(player->position() - 5000);
}

void MainWindow::seekForward()
{
    player->setPosition(player->position() + 5000);
}

void MainWindow::genRankList()
{
    QString surl = "http://m.kugou.com/rank/list&json=true";
    qDebug() << surl;
    QJsonDocument JD = QJsonDocument::fromJson(getReply(surl));
    QJsonArray JA_list = JD.object().value("rank").toObject().value("list").toArray();
    for (int i=0; i<JA_list.size(); i++) {
        int rankid = JA_list[i].toObject().value("rankid").toInt();
        QString rankname = JA_list[i].toObject().value("rankname").toString();
        QString imgurl = JA_list[i].toObject().value("imgurl").toString().replace("/{size}/","/");
        QListWidgetItem *LWI = new QListWidgetItem(QIcon(":/icon/KU.svg"), "");        
        LWI->setData(RANK_ID, rankid);
        LWI->setData(RANK_NAME, rankname);
        LWI->setData(RANK_IMGURL, imgurl);
        listWidget_rank->addItem(LWI);
        getRankImage(LWI);
    }    
}

void MainWindow::rankListItemClick(QListWidgetItem *LWI)
{
    listWidget_discovery->setCurrentRow(-1);
    stackedWidget->setCurrentWidget(widget_songrank);
    tableWidget_songlist_rank->setRowCount(0);
    QString rankname = LWI->data(RANK_NAME).toString();
    label_rankname->setText(rankname);
    label_rankimg->setPixmap(LWI->icon().pixmap(label_rankimg->size()));
    int rankid = LWI->data(RANK_ID).toInt();
    QString surl = QString("http://m.kugou.com/rank/info/?rankid=%1&page=1&json=true").arg(rankid);
    qWarning() << surl;
    QJsonDocument JD = QJsonDocument::fromJson(getReply(surl));
    QJsonArray JA_list = JD.object().value("songs").toObject().value("list").toArray();
    for(int i=0; i<JA_list.size(); i++){
        tableWidget_songlist_rank->insertRow(i);
        tableWidget_songlist_rank->setItem(i, 0, new QTableWidgetItem(JA_list[i].toObject().value("filename").toString()));
        int ds = JA_list[i].toObject().value("duration").toInt();
        QTableWidgetItem *TWI = new QTableWidgetItem(QString("%1:%2").arg(ds/60,2,10,QLatin1Char(' ')).arg(ds%60,2,10,QLatin1Char('0')));
        TWI->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tableWidget_songlist_rank->setItem(i, 1, TWI);
        tableWidget_songlist_rank->setItem(i, 2, new QTableWidgetItem(JA_list[i].toObject().value("hash").toString()));
        QString mvhash = JA_list[i].toObject().value("mvhash").toString();
        tableWidget_songlist_rank->setItem(i, 3, new QTableWidgetItem(mvhash));
        QString album_id = JA_list[i].toObject().value("album_id").toString();
        tableWidget_songlist_rank->setItem(i, 4, new QTableWidgetItem(album_id));
        if (mvhash != "") {
            QPushButton *pushButton_MV = new QPushButton;
            pushButton_MV->setFixedSize(24,24);
            pushButton_MV->setIcon(QIcon(":/icon/video.svg"));
            pushButton_MV->setIconSize(QSize(24,24));
            pushButton_MV->setFocusPolicy(Qt::NoFocus);
            pushButton_MV->setFlat(true);
            pushButton_MV->setCursor(QCursor(Qt::PointingHandCursor));
            connect(pushButton_MV, SIGNAL(clicked()), this, SLOT(rankPushButtonMVClicked()));
            tableWidget_songlist_rank->setCellWidget(i, 5, pushButton_MV);
        }
    }
    tableWidget_songlist_rank->resizeColumnsToContents();
}

QString MainWindow::genKey(int count)
{
    char s[] = {"abcdefghijklmnopqrstuvwxyz0123456789"};
    QString key = "";
    qsrand(static_cast<uint>(clock()));
    for(int i=0; i<count; i++){
        int n = qrand() % static_cast<int>(strlen(s));
        key += QString(s[n]);
    }
    return key;
}

void MainWindow::getRankImage(QListWidgetItem *LWI)
{
    QString surl = LWI->data(RANK_IMGURL).toString();
    //qWarning() << surl;
    QNetworkAccessManager *NAM = new QNetworkAccessManager;
    QNetworkRequest request;
    request.setUrl(QUrl(surl));
    QNetworkReply *reply = NAM->get(request);
    connect(reply, &QNetworkReply::finished, [=]{
        QPixmap pixmap;
        pixmap.loadFromData(reply->readAll());
        pixmap = pixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        if (LWI)
            LWI->setIcon(QIcon(pixmap));
    });
}