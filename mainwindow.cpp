#include "mainwindow.h"
#include "toplistitem.h"
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    settings(QCoreApplication::organizationName(), QCoreApplication::applicationName())
{
    setWindowIcon(QIcon(":/icon/icon.svg"));
    setWindowFlags(Qt::FramelessWindowHint);
    resize(1000,700);

    move((QApplication::desktop()->width()-width())/2,(QApplication::desktop()->height()-height())/2);
    setStyleSheet("QPushButton:hover { background:rgba(0,131,221,50); }"
                  "QTabWidget::pane { border:0px; }"
                  "QTabWidget::tab-bar { alignment:center; }"
                  "QTabBar:tab { width:90px; height:40px; font-size:15px; border:0px; }"
                  "QTabBar::tab:selected { color:rgb(0,131,221); border-bottom:1px solid rgb(0,131,221); }");
    connect(new QShortcut(QKeySequence(Qt::Key_Escape),this), SIGNAL(activated()),this, SLOT(exitFullscreen()));

    QWidget *widget = new QWidget;
    setCentralWidget(widget);
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setSpacing(0);
    vbox->setContentsMargins(0,0,0,0);

    titleBar = new TitleBar;
    connect(titleBar->action_search, SIGNAL(triggered(bool)), this, SLOT(preSearch()));
    connect(titleBar->lineEdit_search, SIGNAL(returnPressed()), this, SLOT(preSearch()));
    connect(titleBar->lineEdit_page, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(titleBar->pushButton_lastPage, SIGNAL(clicked(bool)), this, SLOT(lastPage()));
    connect(titleBar->pushButton_nextPage, SIGNAL(clicked(bool)), this, SLOT(nextPage()));
    connect(titleBar->pushButton_minimize, SIGNAL(clicked(bool)), this, SLOT(showMinimized()));
    connect(titleBar->pushButton_maximize, SIGNAL(clicked(bool)), this, SLOT(showNormalMaximize()));
    connect(titleBar->pushButton_close, SIGNAL(clicked(bool)), qApp, SLOT(quit()));
    connect(titleBar->action_set, SIGNAL(triggered()),this,SLOT(showSetDialog()));
    connect(titleBar, SIGNAL(moveMainWindow(QPoint)), this, SLOT(moveMe(QPoint)));
    vbox->addWidget(titleBar);

    QHBoxLayout *hbox = new QHBoxLayout;
    navWidget = new NavWidget;
    connect(navWidget->pushButton_albumPic, SIGNAL(clicked(bool)), this, SLOT(swapLyric()));
    hbox->addWidget(navWidget);

    tabWidget = new QTabWidget;
    tabWidget->setObjectName("tabWidget");
    repertory = new QTabWidget;
    repertory->setObjectName("repertory");
    repertory->addTab(new QLabel(""),"推荐");

    QWidget *widget_rank = new QWidget;
    QHBoxLayout *hbox_rank = new QHBoxLayout;
    hbox_rank->setSpacing(0);
    QListWidget *rankList = new QListWidget;
    rankList->setFixedWidth(145);
    QListWidgetItem *LWI;
    LWI = new QListWidgetItem(QIcon(":/icon/KU.svg"), "酷音乐排行榜");
    rankList->insertItem(0, LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/N.svg"), "酷狗飙升榜");
    rankList->insertItem(1, LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/icon.svg"), "酷狗TOP500");
    rankList->insertItem(2, LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/fire.svg"), "网络红歌榜");
    rankList->insertItem(3, LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/DJ.svg"), "DJ热歌榜");
    rankList->insertItem(4, LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/华.svg"), "华语新歌榜");
    rankList->insertItem(5, LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/euro.svg"),"欧美新歌榜");
    rankList->insertItem(6, LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/SouthKorea.svg"), "韩国新歌榜");
    rankList->insertItem(7, LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/sakura.svg"), "日本新歌榜");
    rankList->insertItem(8, LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/歌.svg"), "歌手第二季榜");
    rankList->insertItem(9, LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/乐人.png"), "腾讯音乐人原创榜");
    rankList->insertItem(10, LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/粤.svg"), "粤语新歌榜");
    rankList->insertItem(11, LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/hand.png"), "酷狗分享榜");
    rankList->insertItem(12, LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/5sing.png"), "5sing音乐榜");
    rankList->insertItem(13, LWI);
    LWI = new QListWidgetItem(QIcon(":/icon/fanxing.png"), "繁星音乐榜");
    rankList->insertItem(14, LWI);
    connect(rankList,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(rankListItemClick(QListWidgetItem*)));
    hbox_rank->addWidget(rankList);

    tableWidget_songlistrank = new QTableWidget;
    tableWidget_songlistrank->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget_songlistrank->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget_songlistrank->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget_songlistrank->setColumnCount(4);
    QStringList headerRank;
    headerRank << "歌曲名" << "hash" << "mvhash" << "操作";
    tableWidget_songlistrank->setHorizontalHeaderLabels(headerRank);
    tableWidget_songlistrank->setColumnHidden(1,true);
    tableWidget_songlistrank->setColumnHidden(2,true);
    connect(tableWidget_songlistrank, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(playSongRank(int,int)));
    hbox_rank->addWidget(tableWidget_songlistrank);
    widget_rank->setLayout(hbox_rank);
    repertory->addTab(widget_rank,"排行榜");
    repertory->addTab(new QLabel(""),"歌手");
    repertory->addTab(new QLabel(""),"分类");
    repertory->addTab(new QLabel(""),"歌手第二季");
    tabWidget->addTab(repertory,"乐库");
    tabWidget->addTab(new QLabel(""),"电台");

    tableWidget_songlist = new QTableWidget;
    tableWidget_songlist->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget_songlist->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget_songlist->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget_songlist->setColumnCount(7);
    tableWidget_songlist->setColumnHidden(4,true);
    tableWidget_songlist->setColumnHidden(5,true);
    QStringList header;
    header << "歌名" << "歌手" << "专辑" << "时长" << "hash" << "mvhash" << "MV";
    tableWidget_songlist->setHorizontalHeaderLabels(header);
//    tableWidget_songlist->horizontalHeader()->setStyleSheet("QHeaderView::section { color:white; background-color:#232326; }");
//    tableWidget_songlist->verticalHeader()->setStyleSheet("QHeaderView::section { color:white; background-color:#232326; }");
//    tableWidget_songlist->setStyleSheet("QTableView { color:white; selection-background-color:#e6e6e6; }");
    connect(tableWidget_songlist,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(playSong(int,int)));
    tabWidget->addTab(tableWidget_songlist,"歌单");

    videoWidget = new QVideoWidget;
    tabWidget->addTab(videoWidget,"MV");
    //connect(videoWidget,SIGNAL(doubleClicked()),this,SLOT(EEFullscreen()));

    tabWidget->addTab(new QLabel(""),"直播");
    tabWidget->addTab(new QLabel(""),"KTV");
    textBrowser = new QTextBrowser;
    textBrowser->zoomIn(10);
    tabWidget->addTab(textBrowser,"歌词");
    hbox->addWidget(tabWidget);
    vbox->addLayout(hbox);

    controlBar = new ControlBar;
    connect(controlBar->pushButton_last, SIGNAL(pressed()), this, SLOT(playLast()));
    connect(controlBar->pushButton_play, SIGNAL(pressed()), this, SLOT(playPause()));
    connect(controlBar->pushButton_next, SIGNAL(pressed()),this, SLOT(playNext()));
    connect(controlBar->pushButton_songname, SIGNAL(pressed()), this, SLOT(swapLyric()));
    connect(controlBar->pushButton_mute, SIGNAL(pressed()), this, SLOT(mute()));
    connect(controlBar->pushButton_lyric, SIGNAL(clicked(bool)), this, SLOT(showHideLyric(bool)));
    connect(controlBar->pushButton_playlist, SIGNAL(clicked(bool)), this, SLOT(showHidePlayList(bool)));
    connect(controlBar->pushButton_download, SIGNAL(pressed()), this, SLOT(dialogDownload()));
    connect(controlBar->pushButton_fullscreen, SIGNAL(pressed()), this, SLOT(enterFullscreen()));
    connect(controlBar->slider_progress, SIGNAL(sliderReleased()), this, SLOT(setMPPosition()));
    connect(controlBar->slider_volume, SIGNAL(sliderReleased()), this, SLOT(setVolume()));
    //connect(controlBar->comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeQuality(QString)));
    vbox->addWidget(controlBar);
    widget->setLayout(vbox);

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
    connect(lyricWidget->pushButton_set, SIGNAL(pressed()), this, SLOT(showSetDialog()));
    connect(lyricWidget->pushButton_close, SIGNAL(pressed()), this, SLOT(hideLyric()));
    int lx = settings.value("LyricX").toInt();
    int ly = settings.value("LyricY").toInt();
    qDebug() << lx << ly;
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
    if(sfont != ""){
        QStringList SLFont = sfont.split(",");
        lyricWidget->label_lyric->setFont(QFont(SLFont.at(0),  SLFont.at(1).toInt(), SLFont.at(2).toInt(), SLFont.at(3).toInt()));
    }
    lyricWidget->show();

    rankChineseNew();
    rankList->setCurrentRow(5);
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
    // 播放音乐Cookie的kg_mid算法　https://blog.csdn.net/ychgyyn/article/details/90110296，随便写一个好像可以了。
    if (surl.startsWith("http://www.kugou.com/yy/index.php?r=play/getdata&hash=")) {
        request.setRawHeader("Cookie", "kg_mid=355721c2749fe30472161adf09b5748d");
    }
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
    //qDebug() << "isMaximized=" << isMaximized();
    if(isMaximized()){
        showNormal();
        titleBar->pushButton_maximize->setIcon(QIcon(":/icon/maximize.svg"));
    }else{
        showMaximized();
        titleBar->pushButton_maximize->setIcon(QIcon(":/icon/normal.svg"));
    }
}

void MainWindow::playSong(int row, int column)
{
    Q_UNUSED(column);
    QString hash = tableWidget_songlist->item(row,4)->text();
    QString surl = "http://www.kugou.com/yy/index.php?r=play/getdata&hash=" + hash;
    //QString surl = "https://wwwapi.kugou.com/yy/index.php?r=play/getdata&hash=" + hash;
    qDebug() << surl;
    QJsonDocument json = QJsonDocument::fromJson(getReply(surl));
    QString songurl = json.object().value("data").toObject().value("play_url").toString();
    player->setMedia(QUrl(songurl));
    player->play();
    QString songname = tableWidget_songlist->item(row,0)->text() + " - " + tableWidget_songlist->item(row,1)->text();
    navWidget->label_songname->setText(songname + "\n" + tableWidget_songlist->item(row,3)->text());
    controlBar->pushButton_songname->setText(songname);
    lyricWidget->label_lyric->setText(songname);
    setLyric(json.object().value("data").toObject().value("lyrics").toString());
    QString imgurl = json.object().value("data").toObject().value("img").toString();
    if(imgurl != ""){
        QPixmap pixmap;
        pixmap.loadFromData(getReply(imgurl));
        navWidget->pushButton_albumPic->setIcon(QIcon(pixmap));
        pixmap.save(QDir::currentPath() + "/cover.jpg");
        textBrowser->setStyleSheet("QTextBrowser{color:white; border-image:url(cover.jpg);}");
    }else{

    }
}

void MainWindow::playSongRank(int row, int column)
{
    Q_UNUSED(column);
    QString hash = tableWidget_songlistrank->item(row,1)->text();
    QString surl = "http://www.kugou.com/yy/index.php?r=play/getdata&hash=" + hash;
    //QString surl = "https://wwwapi.kugou.com/yy/index.php?r=play/getdata&hash=" + hash;
    qDebug() << surl;
    QJsonDocument json = QJsonDocument::fromJson(getReply(surl));
    QString songurl = json.object().value("data").toObject().value("play_url").toString();
    //qDebug() << json;
    player->setMedia(QUrl(songurl));
    player->play();
    QString songname = tableWidget_songlistrank->item(row,0)->text();
    navWidget->label_songname->setText(songname);
    controlBar->pushButton_songname->setText(songname);
    lyricWidget->label_lyric->setText(songname);
    setLyric(json.object().value("data").toObject().value("lyrics").toString());
    QString imgurl = json.object().value("data").toObject().value("img").toString();
    if(imgurl != ""){
        QPixmap pixmap;
        pixmap.loadFromData(getReply(imgurl));
        navWidget->pushButton_albumPic->setIcon(QIcon(pixmap));
        pixmap.save(QDir::currentPath() + "/cover.jpg");
        textBrowser->setStyleSheet("QTextBrowser{color:white; border-image:url(cover.jpg);}");
    }else{

    }
}

void MainWindow::durationChange(qint64 d)
{
    controlBar->slider_progress->setMaximum(d);
    QTime t(0,0,0);
    t = t.addMSecs(d);
    if(t<QTime(1,0,0)){
        controlBar->label_song_duration->setText("/" + t.toString("mm:ss"));
    }else{
        controlBar->label_song_duration->setText("/" + t.toString("H:mm:ss"));
    }
}

void MainWindow::positionChange(qint64 p)
{
    //qDebug() << "position =" << p;
    controlBar->slider_progress->setValue(p);
    QTime t(0,0,0);
    t = t.addMSecs(p);
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
    for(int a=0; a<lyrics.size(); a++){
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
    if(player->state() == QMediaPlayer::PlayingState){
        player->pause();
    }else if(player->state() == QMediaPlayer::PausedState){
        player->play();
    }else if(player->state() == QMediaPlayer::StoppedState){
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
    if(titleBar->lineEdit_search->text() != ""){
        tabWidget->setCurrentWidget(tableWidget_songlist);
        int pagesize = 20;
        QString surl = "http://mobilecdn.kugou.com/api/v3/search/song?format=json&keyword=" + titleBar->lineEdit_search->text() + "&page=" + QString::number(titleBar->lineEdit_page->text().toInt()) + "&pagesize=" + QString::number(pagesize) + "&showtype=1";
        qDebug() << surl;
        //qDebug() << postReply(surl,spost);
        //stackedWidget->setCurrentIndex(1);
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
            if(mvhash != ""){
                QPushButton *pushButton_MV = new QPushButton;
                pushButton_MV->setFixedSize(24,24);
                pushButton_MV->setIcon(QIcon(":/icon/video.svg"));
                pushButton_MV->setIconSize(QSize(24,24));
                //pushButton_MV->setFocusPolicy(Qt::NoFocus);
                pushButton_MV->setFlat(true);
                pushButton_MV->setCursor(QCursor(Qt::PointingHandCursor));
                connect(pushButton_MV, SIGNAL(clicked()), this, SLOT(pushButtonMVClicked()));
                tableWidget_songlist->setCellWidget(i,6,pushButton_MV);
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

void MainWindow::swapLyric()
{
    if (tabWidget->currentIndex() != 6) {
        tabIndex = tabWidget->currentIndex();
        tabWidget->setCurrentIndex(6);
    } else {
        tabWidget->setCurrentIndex(tabIndex);
    }
}

void MainWindow::hideLyric()
{
    lyricWidget->hide();
    controlBar->pushButton_lyric->setChecked(false);
}

void MainWindow::showHideLyric(bool b)
{
    if(b){
        lyricWidget->show();
        qDebug() << lyricWidget->x() << lyricWidget->y();
    }else{
        lyricWidget->hide();
    }
}

void MainWindow::showSetDialog()
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
    QString sfont = lyricWidget->label_lyric->font().family() + "," + QString::number(lyricWidget->label_lyric->font().pointSize()) + "," + lyricWidget->label_lyric->font().weight() + "," + lyricWidget->label_lyric->font().italic();
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
    label = new QLabel("保存路径");
    hbox->addWidget(label);
    lineEdit_downloadPath = new QLineEdit;
    downloadPath = settings.value("DownloadPath").toString();
    if(downloadPath == ""){
        lineEdit_downloadPath->setText(QStandardPaths::writableLocation(QStandardPaths::MusicLocation));
    }else{
        lineEdit_downloadPath->setText(downloadPath);
    }
    hbox->addWidget(lineEdit_downloadPath);
    QPushButton *pushButton_downloadPath = new QPushButton("选择路径");
    pushButton_downloadPath->setObjectName("SettingDialogChooseDownloadPath");
    pushButton_downloadPath->setFocusPolicy(Qt::NoFocus);
    //pushButton_downloadPath->setFlat(true);
    connect(pushButton_downloadPath,SIGNAL(pressed()),this,SLOT(chooseDownloadPath()));
    hbox->addWidget(pushButton_downloadPath);
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
       QString sfont = font.family() + "," + QString::number(font.pointSize()) + "," + font.weight() + "," + font.italic();
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

void MainWindow::chooseDownloadPath()
{
    downloadPath = QFileDialog::getExistingDirectory(this, "保存路径", downloadPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(downloadPath != ""){
        QObject *object = sender();
        qDebug() << object->objectName() << downloadPath;
        if(object->objectName() == "SettingDialogChooseDownloadPath"){
            lineEdit_downloadPath->setText(downloadPath);
        }
        if(object->objectName() == "DownloadDialogPath"){
            pushButton_path->setText(downloadPath);
            pushButton_path->setToolTip(downloadPath);
        }
        settings.setValue("DownloadPath", downloadPath);
    }
}

void MainWindow::playLast()
{
    if(tabWidget->currentIndex()==0){
        int row = tableWidget_songlistrank->currentRow();
        qDebug() << row;
        if (row != -1) {
            if (row > 0) {
                row--;
                playSongRank(row,0);
                tableWidget_songlistrank->setCurrentCell(row,0);
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
    if(tabWidget->currentIndex()==0){
        int row = tableWidget_songlistrank->currentRow();
        int rc = tableWidget_songlistrank->rowCount();
        qDebug() << row << rc;
        if (row != -1) {
            if (row < rc-1) {
                row++;
                playSongRank(row,0);
                tableWidget_songlistrank->setCurrentCell(row,0);
            }
        }
    }
    if(tabWidget->currentIndex()==2){
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
    QPushButton *senderObj = qobject_cast<QPushButton*>(sender());
    if (senderObj == nullptr) {
        return;
    }
    QModelIndex index = tableWidget_songlist->indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));
    int row = index.row();
    tableWidget_songlist->setCurrentCell(row,0);
    QString songname = tableWidget_songlist->item(row,0)->text() + " - " + tableWidget_songlist->item(row,1)->text();
    navWidget->label_songname->setText(songname + "\n" + tableWidget_songlist->item(row,3)->text());
    controlBar->pushButton_songname->setText(songname);
    QString mvhash = tableWidget_songlist->item(row,5)->text();
    QString surl = "http://m.kugou.com/app/i/mv.php?cmd=100&ismp3=1&ext=mp4&hash=" + mvhash;
    qDebug() << surl;
    QJsonDocument json = QJsonDocument::fromJson(getReply(surl));
    QString mvurl = json.object().value("mvdata").toObject().value("rq").toObject().value("downurl").toString();
    if(mvurl == ""){
        mvurl = json.object().value("mvdata").toObject().value("sq").toObject().value("downurl").toString();
    }
    if(mvurl == ""){
        mvurl = json.object().value("mvdata").toObject().value("le").toObject().value("downurl").toString();
    }
    qDebug() << mvurl;
    lyricWidget->hide();
    navWidget->hide();
    controlBar->pushButton_lyric->setChecked(false);
    controlBar->pushButton_playlist->setChecked(false);
    tabWidget->setCurrentWidget(videoWidget);
    player->setMedia(QUrl(mvurl));
    player->play();
}

void MainWindow::rankPushButtonMVClicked()
{
    QPushButton *senderObj = qobject_cast<QPushButton*>(sender());
    if (senderObj == nullptr) {
        return;
    }
    QModelIndex index = tableWidget_songlistrank->indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));
    int row = index.row();
    tableWidget_songlistrank->setCurrentCell(row,0);
    QString songname = tableWidget_songlistrank->item(row,0)->text();
    navWidget->label_songname->setText(songname);
    controlBar->pushButton_songname->setText(songname);
    QString mvhash = tableWidget_songlistrank->item(row,2)->text();
    QString surl = "http://m.kugou.com/app/i/mv.php?cmd=100&ismp3=1&ext=mp4&hash=" + mvhash;
    qDebug() << surl;
    QJsonDocument json = QJsonDocument::fromJson(getReply(surl));
    QString mvurl = json.object().value("mvdata").toObject().value("rq").toObject().value("downurl").toString();
    if(mvurl == ""){
        mvurl = json.object().value("mvdata").toObject().value("sq").toObject().value("downurl").toString();
    }
    if(mvurl == ""){
        mvurl = json.object().value("mvdata").toObject().value("le").toObject().value("downurl").toString();
    }
    qDebug() << mvurl;
    lyricWidget->hide();
    navWidget->hide();
    controlBar->pushButton_lyric->setChecked(false);
    controlBar->pushButton_playlist->setChecked(false);
    tabWidget->setCurrentWidget(videoWidget);
    player->setMedia(QUrl(mvurl));
    player->play();
}

void MainWindow::showHidePlayList(bool b)
{
    navWidget->setVisible(b);
}

void MainWindow::rankListItemClick(QListWidgetItem *item)
{
    Q_UNUSED(item);
}

void MainWindow::rankChineseNew()
{
    tableWidget_songlistrank->setRowCount(0);
    QString surl = "http://m.kugou.com/?json=true";
    qDebug() << surl;
    QJsonDocument json = QJsonDocument::fromJson(getReply(surl));
    QJsonArray songs = json.object().value("data").toArray();
    for(int i=0; i<songs.size(); i++){
        tableWidget_songlistrank->insertRow(i);
        tableWidget_songlistrank->setItem(i,0,new QTableWidgetItem(songs[i].toObject().value("filename").toString()));
        tableWidget_songlistrank->setItem(i,1,new QTableWidgetItem(songs[i].toObject().value("hash").toString()));
        QString mvhash = songs[i].toObject().value("mvhash").toString();
        tableWidget_songlistrank->setItem(i,2,new QTableWidgetItem(mvhash));
        if (mvhash != "") {
            QPushButton *pushButton_MV = new QPushButton;
            pushButton_MV->setFixedSize(24,24);
            pushButton_MV->setIcon(QIcon(":/icon/video.svg"));
            pushButton_MV->setIconSize(QSize(24,24));
            pushButton_MV->setFocusPolicy(Qt::NoFocus);
            pushButton_MV->setFlat(true);
            pushButton_MV->setCursor(QCursor(Qt::PointingHandCursor));
            connect(pushButton_MV, SIGNAL(clicked()), this, SLOT(rankPushButtonMVClicked()));
            tableWidget_songlistrank->setCellWidget(i,3,pushButton_MV);
        }
    }
    tableWidget_songlistrank->resizeColumnsToContents();
    repertory->setCurrentIndex(1);
}

void MainWindow::enterFullscreen()
{
    if(tabWidget->currentIndex()==3 || tabWidget->currentIndex()==6){
        showFullScreen();
        titleBar->hide();
        controlBar->hide();
        tabWidget->tabBar()->hide();
        navWidget->hide();
        lyricWidget->hide();
    }
}

void MainWindow::exitFullscreen()
{
    if(isFullScreen()){
        showNormal();
        titleBar->show();
        controlBar->show();
        tabWidget->tabBar()->show();
        if(controlBar->pushButton_playlist->isChecked())navWidget->show();
        if(controlBar->pushButton_lyric->isChecked())lyricWidget->show();
    }
}

void MainWindow::dialogDownload()
{
    QDialog *dialog = new QDialog(this);
    dialog->setFixedWidth(300);
    dialog->setWindowTitle("下载");
    QGridLayout *gridLayout = new QGridLayout;
    QLabel *label = new QLabel("歌名");
    gridLayout->addWidget(label,0,0,1,1);
    QLineEdit *lineEdit_songname = new QLineEdit;
    lineEdit_songname->setText(navWidget->label_songname->text().left(navWidget->label_songname->text().indexOf("\n")));
    gridLayout->addWidget(lineEdit_songname,0,1,1,1);
    label = new QLabel("下载地址");
    gridLayout->addWidget(label,1,0,1,1);
    QLineEdit *lineEdit_url = new QLineEdit;
    lineEdit_url->setText(player->media().canonicalUrl().toString());
    gridLayout->addWidget(lineEdit_url,1,1,1,1);
    //label = new QLabel("保存路径");
    //gridLayout->addWidget(label,2,0,1,1);
    QPushButton *pushbutton_openpath = new QPushButton("保存路径");
    pushbutton_openpath->setStyleSheet("border:none;");
    gridLayout->addWidget(pushbutton_openpath,2,0,1,1);
    pushButton_path = new QPushButton;
    pushButton_path->setObjectName("DownloadDialogPath");
    pushButton_path->setFocusPolicy(Qt::NoFocus);
    downloadPath = settings.value("DownloadPath").toString();
    pushButton_path->setText(downloadPath);
    pushButton_path->setToolTip(downloadPath);
    connect(pushButton_path, SIGNAL(pressed()), this, SLOT(chooseDownloadPath()));
    connect(pushbutton_openpath, &QPushButton::clicked, [=](){
         QDesktopServices::openUrl(QUrl(downloadPath));
    });
    gridLayout->addWidget(pushButton_path,2,1,1,1);
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
        download(lineEdit_url->text(), pushButton_path->text() + "/" + lineEdit_songname->text() + "." + QFileInfo(lineEdit_url->text()).suffix());
    }else
    if (result == QDialog::Rejected) {
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
    float p = (float)bytesReceived/bytesTotal;
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
    //painter.setRenderHint(QPainter::Antialiasing, true);  //无效
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    if(isFullScreen()){
        painter.drawRect(bitmap.rect());
    }else{
        painter.drawRoundedRect(bitmap.rect(),10,10);
    }
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