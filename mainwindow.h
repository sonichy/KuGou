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
#include <QSettings>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event);

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
    QString downloadPath;
    QPushButton *pushButton_font, *pushButton_fontcolor, *pushButton_path;
    QVideoWidget *videoWidget;
    void rankChineseNew();
    QSettings settings;

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
    void dialogSet();
    void chooseFont();
    void chooseFontColor();
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
    void seekBack();
    void seekForward();

};

#endif // MAINWINDOW_H