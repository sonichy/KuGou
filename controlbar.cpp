#include "controlbar.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

ControlBar::ControlBar(QWidget *parent) : QWidget(parent)
{
    setStyleSheet("QPushButton { color: white; }"
                  "QComboBox { color:white; background:rgb(0,131,221); border: 1px solid white; border-radius: 10px; }"
                  "QComboBox::drop-down { border:1px solid transparent; }"
                  "QComboBox QListView { background: rgb(0,131,221); }"
                  "QSlider::sub-page:Horizontal { background-color: #DEFFED; }"
                  "QSlider::add-page:Horizontal { background-color: #6EAFE7; }"
                  "QSlider::groove:Horizontal { background: transparent; height:4px; }"
                  "QSlider::handle:Horizontal { width:13px; height:13px; border:1px solid #DEFFED; border-radius:7px; background:#DEFFED; margin: -5px 0px -5px 0px; }"
                  "QToolTip{border-style:none; background-color:black;}"
                  "#pushButton_quality { border: 1px solid white; border-radius: 10px; }");

    setFixedHeight(70);

    QHBoxLayout *hbox = new QHBoxLayout;

    pushButton_last = new QPushButton;
    pushButton_last->setFixedSize(48,48);
    pushButton_last->setIcon(QIcon(":/last.svg"));
    pushButton_last->setIconSize(QSize(35,35));
    pushButton_last->setFocusPolicy(Qt::NoFocus);
    pushButton_last->setFlat(true);
    hbox->addWidget(pushButton_last);

    pushButton_play = new QPushButton;
    pushButton_play->setFixedSize(48,48);
    pushButton_play->setIcon(QIcon(":/play.svg"));
    pushButton_play->setIconSize(QSize(42,42));
    pushButton_play->setFocusPolicy(Qt::NoFocus);
    pushButton_play->setFlat(true);
    //connect(pushButton_play,SIGNAL(pressed()),this,SLOT(emitPlayPause()));
    hbox->addWidget(pushButton_play);

    pushButton_next = new QPushButton;
    pushButton_next->setFixedSize(48,48);
    pushButton_next->setIcon(QIcon(":/next.svg"));
    pushButton_next->setIconSize(QSize(35,35));
    pushButton_next->setFocusPolicy(Qt::NoFocus);
    pushButton_next->setFlat(true);
    hbox->addWidget(pushButton_next);

    QVBoxLayout *vbox = new QVBoxLayout;

    QHBoxLayout *hbox1 = new QHBoxLayout;
    hbox1->setSpacing(0);
    comboBox = new QComboBox;
    QStringList qualities;
    qualities << "无损" << "高品质" << "标准" << "低品质";
    comboBox->addItems(qualities);
    comboBox->setFixedWidth(80);
    comboBox->setFocusPolicy(Qt::NoFocus);
    int index = comboBox->findText("标准");
    comboBox->setCurrentIndex(index);
    hbox1->addWidget(comboBox);

    pushButton_songname = new QPushButton("歌手 - 歌名",this);
    //pushButton_songname->setFixedSize(40,20);
    pushButton_songname->setFocusPolicy(Qt::NoFocus);
    pushButton_songname->setFlat(true);
    hbox1->addWidget(pushButton_songname);
    hbox1->addStretch();

    label_song_timeNow = new QLabel;
    label_song_timeNow->setText("00:00");
    label_song_timeNow->setStyleSheet("color:white;");
    hbox1->addWidget(label_song_timeNow);

    label_song_duration = new QLabel;
    label_song_duration->setText("/00:00");
    label_song_duration->setStyleSheet("color:white;");
    hbox1->addWidget(label_song_duration);

    vbox->addLayout(hbox1);

    slider_progress = new QSlider;
    slider_progress->setRange(0,1000);
    slider_progress->setOrientation(Qt::Horizontal);
    slider_progress->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    slider_progress->setFocusPolicy(Qt::NoFocus);    
    vbox->addWidget(slider_progress);

    hbox->addLayout(vbox);

    pushButton_mute = new QPushButton;
    pushButton_mute->setFixedSize(25,25);
    pushButton_mute->setIcon(QIcon(":/volume.svg"));
    pushButton_mute->setIconSize(QSize(20,20));
    pushButton_mute->setFocusPolicy(Qt::NoFocus);
    pushButton_mute->setFlat(true);
    hbox->addWidget(pushButton_mute);

    slider_volume = new QSlider;
    slider_volume->setOrientation(Qt::Horizontal);
    slider_volume->setRange(0,100);
    slider_volume->setValue(100);
    slider_volume->setFixedWidth(100);
    slider_volume->setFocusPolicy(Qt::NoFocus);
    //hbox->addWidget(slider_volume);

    QPushButton *pushButton_loop = new QPushButton;
    pushButton_loop->setFixedSize(30,30);
    pushButton_loop->setIcon(QIcon(":/looplist.svg"));
    pushButton_loop->setIconSize(QSize(25,25));
    pushButton_loop->setFocusPolicy(Qt::NoFocus);
    pushButton_loop->setFlat(true);
    hbox->addWidget(pushButton_loop);

    pushButton_lyric = new QPushButton;
    pushButton_lyric->setFixedSize(30,30);
    pushButton_lyric->setIcon(QIcon(":/lyric.svg"));
    pushButton_lyric->setIconSize(QSize(25,25));
    pushButton_lyric->setFocusPolicy(Qt::NoFocus);
    pushButton_lyric->setFlat(true);
    pushButton_lyric->setCheckable(true);
    pushButton_lyric->setChecked(true);
    hbox->addWidget(pushButton_lyric);

    pushButton_playlist = new QPushButton;
    pushButton_playlist->setFixedSize(30,30);
    pushButton_playlist->setIcon(QIcon(":/playlist.svg"));
    pushButton_playlist->setIconSize(QSize(25,25));
    pushButton_playlist->setFocusPolicy(Qt::NoFocus);
    pushButton_playlist->setFlat(true);
    pushButton_playlist->setCheckable(true);
    pushButton_playlist->setChecked(true);
    hbox->addWidget(pushButton_playlist);

    setLayout(hbox);
}

void ControlBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0,131,221));
    p.drawRect(rect());
}
