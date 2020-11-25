#ifndef CONTROLBAR_H
#define CONTROLBAR_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QPainter>
#include <QComboBox>

class ControlBar : public QWidget
{
    Q_OBJECT
public:
    explicit ControlBar(QWidget *parent = nullptr);
    QSlider *slider_progress, *slider_volume;
    QPushButton *pushButton_albumPic, *pushButton_love, *pushButton_last, *pushButton_play, *pushButton_next, *pushButton_quality, *pushButton_songname, *pushButton_mute, *pushButton_lyric, *pushButton_playlist, *pushButton_download, *pushButton_fullscreen;
    QComboBox *comboBox;
    QLabel *label_song_timeNow, *label_song_duration;

private:
     void paintEvent(QPaintEvent *event);

signals:
    //void playPause();

private slots:
    //void emitPlayPause();
};

#endif // CONTROLBAR_H