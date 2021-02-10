#include "controlbar.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

ControlBar::ControlBar(QWidget *parent) : QWidget(parent)
{
    setStyleSheet("QPushButton { color:gray; }"
                  "QPushButton:pressed { background:transparent; }"
                  //"QPushButton:checked { color:white; background:#5292FE; }"
                  "QComboBox { color:gray; background:rgb(255,255,255); border:1px solid gray; border-radius: 10px; }"
                  //"QComboBox::drop-down { border:1px solid gray; }"
                  //"QComboBox QListView { background: rgb(0,131,221); }"
                  "QSlider::groove:Horizontal { background: transparent; height:4px; }" //槽
                  "QSlider::handle:Horizontal { width:13px; height:13px; border:1px solid #5292FE; border-radius:7px; background:#5292FE; margin: -5px 0px -5px 0px; }" //柄
                  "QSlider::sub-page:Horizontal { background-color: #5292FE; }" //已滑过
                  "QSlider::add-page:Horizontal { background-color: #DEE2E6; }" //没滑过
                  );

    setFixedHeight(100);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setSpacing(0);
    vbox->setContentsMargins(0,0,0,0);

    slider_progress = new QSlider;
    slider_progress->setRange(0,1000);
    slider_progress->setOrientation(Qt::Horizontal);
    slider_progress->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    slider_progress->setFocusPolicy(Qt::NoFocus);
    vbox->addWidget(slider_progress);

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->setContentsMargins(10,0,10,0);

    pushButton_albumPic = new QPushButton;
    pushButton_albumPic->setFixedSize(50,50);
    pushButton_albumPic->setIcon(QIcon(":/icon/album.svg"));
    pushButton_albumPic->setIconSize(QSize(50,50));
    pushButton_albumPic->setFocusPolicy(Qt::NoFocus);
    pushButton_albumPic->setFlat(true);
    pushButton_albumPic->setCursor(Qt::PointingHandCursor);
    hbox->addWidget(pushButton_albumPic);

    QWidget *widget = new QWidget;
    widget->setFixedWidth(100);
    QVBoxLayout *vbox1 = new QVBoxLayout;
    pushButton_songname = new QPushButton("歌手 - 歌名", this);
    pushButton_songname->setStyleSheet("text-align:left;");
    pushButton_songname->setFocusPolicy(Qt::NoFocus);
    pushButton_songname->setFlat(true);
    vbox1->addWidget(pushButton_songname);

    QHBoxLayout *hbox1 = new QHBoxLayout;
    label_song_timeNow = new QLabel;
    label_song_timeNow->setText("00:00");
    hbox1->addWidget(label_song_timeNow);
    QLabel *label = new QLabel("/");
    hbox1->addWidget(label);
    label_song_duration = new QLabel;
    label_song_duration->setText("00:00");
    hbox1->addWidget(label_song_duration);
    hbox1->addStretch();
    vbox1->addLayout(hbox1);
    widget->setLayout(vbox1);
    hbox->addWidget(widget);

    pushButton_love = new QPushButton;
    pushButton_love->setFixedSize(25,25);
    pushButton_love->setIcon(QIcon(":/icon/heart.svg"));
    pushButton_love->setIconSize(QSize(20,20));
    pushButton_love->setFocusPolicy(Qt::NoFocus);
    pushButton_love->setFlat(true);
    pushButton_love->setCursor(Qt::PointingHandCursor);
    pushButton_love->setCheckable(true);
    pushButton_love->setStyleSheet("QPushButton:checked { border:none; }");
    connect(pushButton_love, &QPushButton::toggled, [=](bool b){
        if (b)
            pushButton_love->setIcon(QIcon(":/icon/heart1.svg"));
        else
            pushButton_love->setIcon(QIcon(":/icon/heart.svg"));
    });
    hbox->addWidget(pushButton_love);

    pushButton_download = new QPushButton;
    pushButton_download->setFixedSize(30,30);
    pushButton_download->setIcon(QIcon(":/icon/download.svg"));
    pushButton_download->setIconSize(QSize(25,25));
    pushButton_download->setFocusPolicy(Qt::NoFocus);
    pushButton_download->setFlat(true);
    pushButton_download->setCursor(Qt::PointingHandCursor);
    hbox->addWidget(pushButton_download);

    hbox->addStretch();

    pushButton_last = new QPushButton;
    pushButton_last->setFixedSize(48,48);
    pushButton_last->setIcon(QIcon(":/icon/last.svg"));
    pushButton_last->setIconSize(QSize(35,35));
    pushButton_last->setFocusPolicy(Qt::NoFocus);
    pushButton_last->setFlat(true);
    pushButton_last->setCursor(Qt::PointingHandCursor);
    hbox->addWidget(pushButton_last);

    pushButton_play = new QPushButton;
    pushButton_play->setFixedSize(48,48);
    pushButton_play->setIcon(QIcon(":/icon/play.svg"));
    pushButton_play->setIconSize(QSize(42,42));
    pushButton_play->setFocusPolicy(Qt::NoFocus);
    pushButton_play->setFlat(true);
    pushButton_play->setCursor(Qt::PointingHandCursor);
    hbox->addWidget(pushButton_play);

    pushButton_next = new QPushButton;
    pushButton_next->setFixedSize(48,48);
    pushButton_next->setIcon(QIcon(":/icon/next.svg"));
    pushButton_next->setIconSize(QSize(35,35));
    pushButton_next->setFocusPolicy(Qt::NoFocus);
    pushButton_next->setFlat(true);
    pushButton_next->setCursor(Qt::PointingHandCursor);
    hbox->addWidget(pushButton_next);

    hbox->addStretch();

    comboBox = new QComboBox;
    QStringList qualities;
    qualities << "无损" << "高品质" << "标准" << "低品质";
    comboBox->addItems(qualities);
    comboBox->setFixedWidth(80);
    comboBox->setFocusPolicy(Qt::NoFocus);
    int index = comboBox->findText("标准");
    comboBox->setCurrentIndex(index);
    hbox->addWidget(comboBox);

    pushButton_mute = new QPushButton;
    pushButton_mute->setFixedSize(25,25);
    pushButton_mute->setIcon(QIcon(":/icon/volume.svg"));
    pushButton_mute->setIconSize(QSize(20,20));
    pushButton_mute->setFocusPolicy(Qt::NoFocus);
    pushButton_mute->setFlat(true);
    pushButton_mute->setCursor(Qt::PointingHandCursor);
    hbox->addWidget(pushButton_mute);

    slider_volume = new QSlider;
    slider_volume->setOrientation(Qt::Vertical);
    slider_volume->setRange(0,100);
    slider_volume->setValue(100);
    slider_volume->setFixedWidth(100);
    slider_volume->setFocusPolicy(Qt::NoFocus);
    //hbox->addWidget(slider_volume);

    QPushButton *pushButton_loop = new QPushButton;
    pushButton_loop->setFixedSize(30,30);
    pushButton_loop->setIcon(QIcon(":/icon/looplist.svg"));
    pushButton_loop->setIconSize(QSize(25,25));
    pushButton_loop->setFocusPolicy(Qt::NoFocus);
    pushButton_loop->setFlat(true);
    pushButton_loop->setCursor(Qt::PointingHandCursor);
    hbox->addWidget(pushButton_loop);

    pushButton_lyric = new QPushButton;
    pushButton_lyric->setFixedSize(30,30);
    pushButton_lyric->setIcon(QIcon(":/icon/lyric.svg"));
    pushButton_lyric->setIconSize(QSize(25,25));
    pushButton_lyric->setFocusPolicy(Qt::NoFocus);
    pushButton_lyric->setFlat(true);
    pushButton_lyric->setCheckable(true);
    pushButton_lyric->setCursor(Qt::PointingHandCursor);
    hbox->addWidget(pushButton_lyric);

    pushButton_fullscreen = new QPushButton;
    pushButton_fullscreen->setFixedSize(30,30);
    pushButton_fullscreen->setIcon(QIcon(":/icon/fullscreen.svg"));
    pushButton_fullscreen->setIconSize(QSize(25,25));
    pushButton_fullscreen->setFocusPolicy(Qt::NoFocus);
    pushButton_fullscreen->setFlat(true);
    pushButton_fullscreen->setCursor(Qt::PointingHandCursor);
    //pushButton_fullscreen->setShortcut(QKeySequence::FullScreen); //不知道是哪个键
    pushButton_fullscreen->setShortcut(Qt::Key_F11);
    pushButton_fullscreen->setToolTip("全屏(F11)");
    hbox->addWidget(pushButton_fullscreen);

    vbox->addLayout(hbox);
    setLayout(vbox);
}

void ControlBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(255,255,255));
    p.drawRect(rect());
}