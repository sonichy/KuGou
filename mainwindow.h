#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "titlebar.h"
#include "controlbar.h"
#include "lyricwidget.h"
#include <QMainWindow>
#include <QStackedWidget>
#include <QTableWidget>
#include <QMediaPlayer>
#include <QTextBrowser>
#include <QTime>
#include <QVideoWidget>
#include <QSettings>
#include <QListWidget>

#define RANK_ID Qt::UserRole + 1
#define RANK_NAME Qt::UserRole + 2
#define RANK_IMGURL Qt::UserRole + 3

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    TitleBar *titleBar;
    QStackedWidget *stackedWidget;
    QWidget *widget_mymusic, *widget_discovery, *widget_songrank;//*toplistWidget,
    QTabWidget *tabWidget;
    QListWidget *listWidget_mymusic, *listWidget_discovery, *listWidget_rank;
    QTableWidget *tableWidget_songlist, *tableWidget_songlist_rank;
    QTextBrowser *textBrowser;
    ControlBar *controlBar;
    QMediaPlayer *player;
    LyricWidget *lyricWidget;
    int volume, tabIndex;
    QByteArray getReply(QString surl);
    QByteArray postReply(QString surl,QString spost);
    void setLyric(QString s);
    struct Lyric{
        QTime time;
        QString sentence;
    };
    QList<Lyric> lyrics;
    QString downloadPath;
    QPushButton *pushButton_font, *pushButton_fontcolor, *pushButton_path;
    QVideoWidget *videoWidget;
    QSettings settings;
    void genRankList();
    QString genKey(int count);
    QByteArray kg_mid;
    QString dfid;
    QLabel *label_rankimg, *label_rankname;
    void getRankImage(QListWidgetItem *LWI);

private slots:
    void showNormalMaximize();
    void moveMe(QPoint point);
    void playSong(int,int);
    void durationChange(qint64 d);
    void positionChange(qint64 p);
    void stateChange(QMediaPlayer::State state);
    void volumeChange(int v);
    void playPause();
    void setMPPosition();
    void setVolume();
    void mute();
    void preSearch();
    void search();
    void lastPage();
    void nextPage();
    void dialogSet();
    void chooseFont();
    void chooseFontColor();
    void playLast();
    void playNext();
    void pushButtonMVClicked();
    void rankPushButtonMVClicked();
    void rankListItemClick(QListWidgetItem *LWI);
    void playSongRank(int row, int column);
    void enterFullscreen();
    void exitFullscreen();
    void dialogDownload();
    void download(QString surl, QString filepath);
    void updateProgress(qint64 bytesReceived, qint64 bytesTotal);
    void seekBack();
    void seekForward();

};

#endif // MAINWINDOW_H