#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "titlebar.h"
#include "navwidget.h"
#include "controlbar.h"
#include "lyricwidget.h"
#include <QMainWindow>
#include <QStackedWidget>
#include <QTableWidget>
#include <QMediaPlayer>
#include <QTextBrowser>
#include <QTime>
#include <QVideoWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    TitleBar *titleBar;
    NavWidget *navWidget;    
    QWidget *toplistWidget;
    QTabWidget *tabWidget, *repertory;
    QTableWidget *tableWidget_songlist, *tableWidget_songlistrank;
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
    QLineEdit *lineEdit_downloadPath;
    QString downloadPath;
    QString readSettings(QString path, QString group, QString key);
    void writeSettings(QString path, QString group, QString key, QString value);
    QPushButton *pushButton_fontcolor, *pushButton_path;
    QVideoWidget *videoWidget;
    void rankChineseNew();

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
    void swapLyric();
    void hideLyric();
    void showHideLyric(bool);
    void on_action_settings_triggered();
    void chooseFont();
    void chooseFontColor();
    void chooseDownloadPath();
    void playLast();
    void playNext();
    void pushButtonMVClicked();
    void rankPushButtonMVClicked();
    void showHidePlayList(bool b);
    void rankListItemClick(QListWidgetItem *item);
    void playSongRank(int row, int column);
    void enterFullscreen();
    void exitFullscreen();
    void dialogDownload();
    void download(QString surl, QString filepath);
    void updateProgress(qint64 bytesReceived, qint64 bytesTotal);
};

#endif // MAINWINDOW_H
